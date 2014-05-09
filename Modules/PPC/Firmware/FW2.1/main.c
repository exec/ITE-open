/*****************************************************
Date	: 02/May/2012
Chip type		   : ATmega64
Clock frequency	 : 14.745600 MHz
Data Stack size	 : 1024
*****************************************************/


#include <mega64.h>
#include <delay.h>

#define XTIME_USE_FAST_TIMER (0)
#include "../../../FirmwareLibs/XDataTypes.h"

#define USE_PWM_SEQUENCE32 (0)
#define USE_PWM_SEQUENCE27 (1)
#include "../../../FirmwareLibs/XPwmSequences.h"

#define DEFAULT_USART_0 (1)
#include "../../../FirmwareLibs/XUSART64.h"

#define XTIMERS_CUSTOM_DIVISOR (1)
const UCHAR TIMER_DIVISOR = 15;
#include "../../../FirmwareLibs/XTimers64.h"




#include "../../../FirmwareLibs/X64LedControl.h"
//#include "JumperControl.h"

#include "FanControl.h"
#include "RailControl.h"
#include "PsuControl.h"
#include "PullDownControl.h"
#include "InputControl.h"
#include "PPC_ADC64.h"
#include "BatChargeControl.h"
//#include "Temperature1W.h"


#include "XPPCNetProtocol.h"


/*
char debug_fan_power_step=PWR_FAN_MIN;
unsigned int lastFanStepTickCount = 0; // led update interval is 100 ms
const unsigned int FAN_STEP_INTERVAL = SYS_TICKS_PER_SECOND;
*/


typedef enum tagPPC_MODE
{
	PPC_MODE_WAITING,
	PPC_MODE_STARTING,
	PPC_MODE_ACTIVE,
	PPC_MODE_PARKING
} PPC_MODE;

typedef struct tagPPCSTATE
{
	// these are bit values (one or zero)
	PPC_MODE mode;
	unsigned char stage;

} PPCSTATE;

PPCSTATE ppcState;

void inline initPPC_State()
{
	ppcState.mode = PPC_MODE_WAITING;
	ppcState.stage = 0;
};

void inline setPPC_Mode(PPC_MODE new_mode)
{
	ppcState.mode = new_mode;
	ppcState.stage = 0;
};





// green led mst indicate incoming network activity (if in buffer, then green light on)


// Timer 0 overflow interrupt service routine
interrupt [TIM0_OVF] void timer0_ovf_isr(void)
{
	// Reinitialize Timer 0 value
    // Estimate frequency:
    // 20 KHz is Ok
    // 460.800 / 24 = 19.2 KHz
    TCNT0 = TIMER_COUNTER;

    sys_current_tick_count++;

	// 27-bit linear PWM (used for Power amplification)
	pwm27_idx++;
	if (pwm27_idx >= MAX_PWM27_LIN_INDEX) pwm27_idx = 0;

    // Perform Fan Controlling
	FansPWMUpdate();
	FansPortUpdate();
}



void main(void)
{
	PORTA=0x00;
	DDRA=0x00;

	PORTB=0x00;
	DDRB=0x00;

	PORTC=0x00;
	DDRC=0x00;

	PORTD=0x00;
	DDRD=0x00;

	PORTE=0x00;
	DDRE=0x00;

	PORTF=0x00;
	DDRF=0x00;

	PORTG=0x00;
	DDRG=0x00;

	initTimer0();
    initTimer1();
    initTimer2();
    initTimer3();

	enableTimers();

	// External Interrupt(s) initialization
	// INT0-INT7: Off
	EICRA=0x00;
	EICRB=0x00;
	EIMSK=0x00;

	// Analog Comparator initialization
	// Analog Comparator: Off
	// Analog Comparator Input Capture by Timer/Counter 1: Off
	ACSR=0x80;
	SFIOR=0x00;

	// ====================== TRUE INITIALIZATION =============================

    initLeds();
	//initJumpers();

	InitFans();
	// configure Fans
    FanSetPower(&fan1, 5);
	FanSetPower(&fan2, 0);
	FanSetPower(&fan3, 0);
	FanSetPower(&fan4, 0); // max is 27

	FanEnable(&fan2);
	FanEnable(&fan3);
	FanEnable(&fan4);




	InitRailControl();

	InitPsuControl();

	InitPullDownControl();

	//InitTemperature(); // no temperature-based control yet // depend on jumper J1

	InitSignalControl();

	InitADC();
	InitBatChargeControl();

	initPPC_State();

    initUSART();


	#asm("sei")

	while(1)
	{
		// read jumpers
		// read inputs
		// update fans
		// update outputs
		// enter sleep

		/*
		// test fan speed debug
		if (getTicksDelta(lastFanStepTickCount) > FAN_STEP_INTERVAL)
		{
			lastFanStepTickCount = getTickCount();
			debug_fan_power_step++;
			if (debug_fan_power_step > PWR_FAN_MAX)
				debug_fan_power_step = PWR_FAN_MIN;
            FanSetPower(&fan1, debug_fan_power_step);
		};
		*/

        BatChargeUpdate();
		//batChargeState.all_batteries_charged = 1; // !!!
		SignalsIndicateCharged(batChargeState.all_batteries_charged);

		#if 1
        SignalReadUpdate();


		switch (ppcState.mode)
		{
		case(PPC_MODE_WAITING):
		{
			if (InputsTransientHigh(SIGNAL_INDEX_SPINT_POWER))
			{
				setPPC_Mode(PPC_MODE_STARTING);
			};
			break;
		}; // waiting mode state

		case(PPC_MODE_STARTING):
		{
            FanEnable(&fan1);
            switch(ppcState.stage)
			{
			case(0):
                PsuSet(0,0,1,0);
				PsuUpdateOutput();
				PORTA = railState.port_value | psuState.port_value | pullDownState.portA_value;
				delay_ms(200);

				break;

			case(1):
                PsuSet(1,0,1,0);
				PsuUpdateOutput();
				PORTA = railState.port_value | psuState.port_value | pullDownState.portA_value;
				delay_ms(200);

				break;

			case(2):
                PsuSet(1,0,1,1);
				PsuUpdateOutput();
				PORTA = railState.port_value | psuState.port_value | pullDownState.portA_value;
				delay_ms(200);

				break;

			case(3):
                RailSet(1,0);
				RailUpdateOutput();
				PORTA = railState.port_value | psuState.port_value | pullDownState.portA_value;
				delay_ms(200);

				break;

			case(4):
                RailSet(1,1);
				RailUpdateOutput();
				PORTA = railState.port_value | psuState.port_value | pullDownState.portA_value;
				delay_ms(1000);

				break;

			case(5):
                PullDownSet(1,0,0);
				PullDownUpdateOutput();
				PORTA = railState.port_value | psuState.port_value | pullDownState.portA_value;
				delay_ms(100);

				break;

			case(6):
                PullDownSet(0,0,0);
				PullDownUpdateOutput();
				PORTA = railState.port_value | psuState.port_value | pullDownState.portA_value;
				delay_ms(100);

				break;

			default:
				setPPC_Mode(PPC_MODE_ACTIVE);
				break;

			};
			ppcState.stage++;
			break;
		}; // starting mode state

		case(PPC_MODE_ACTIVE):
		{
			FanEnable(&fan1);

			if (signalState.current_signal_state[SIGNAL_INDEX_SPINT_POWER] == 0)
			{
				setPPC_Mode(PPC_MODE_PARKING);
			};

			//if (InputsTransientLow(SIGNAL_INDEX_SPINT_POWER))

			break;
		}; // acitve mode state

		case(PPC_MODE_PARKING):
		{
			FanDisable(&fan1);
			switch(ppcState.stage)
			{
			case(0): // turn off PSU power
                PsuSet(0,0,0,0);
				PsuUpdateOutput();
				PORTA = railState.port_value | psuState.port_value | pullDownState.portA_value;
				delay_ms(200);

				break;
			case(1): // park computer (pull-in)
                PullDownSet(1,0,0);
				PullDownUpdateOutput();
				PORTA = railState.port_value | psuState.port_value | pullDownState.portA_value;
				delay_ms(100);

				break;
			case(2): // park computer (pull-out)
                PullDownSet(0,0,0);
				PullDownUpdateOutput();
				PORTA = railState.port_value | psuState.port_value | pullDownState.portA_value;
				delay_ms(1000); // wait before clamping power rail

				break;

			case(3): // turn-off rails
                RailSet(0,0);
				RailUpdateOutput();
				PORTA = railState.port_value | psuState.port_value | pullDownState.portA_value;
				delay_ms(200);

				break;
			default:
				setPPC_Mode(PPC_MODE_WAITING);
				break;
			};

			ppcState.stage++;
			break;
		}; // Parking mode state

		}; // MODE Switch




		// This function locks for 0.2 second minimum
		// This must be read in the end of control loop
		/*
		TemperatureReadUpdate();
		if (temperatureState.temperature1 != 0)
		{
			if (temperatureState.temperature1 <= 30)
			{
				FanDisable(&fan1);
			}
			else
			{
				FanEnable(&fan1);
				FanSetPower(&fan1,(unsigned char)(PWR_FAN_MIN + ((signed int)(PWR_FAN_MAX - PWR_FAN_MIN))*temperatureState.temperature1/60));
			};
		}
		else
		{
			FanDisable(&fan1);
		};
		*/


		RailUpdateOutput();
		PsuUpdateOutput();
		PullDownUpdateOutput();

		networkUpdate();

		PORTA = railState.port_value | psuState.port_value | pullDownState.portA_value;

        #endif


		/*
		if (ppcState.mode == PPC_MODE_WAITING)
		{
			#asm("sleep")
		}
		*/
	}



} // main