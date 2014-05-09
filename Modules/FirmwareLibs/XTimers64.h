#ifndef H_ATM64_TIMER0_H
#define H_ATM64_TIMER0_H

#include "XDataTypes.h"

// Timer is used for software PWM

// Frequency Clock = 14745600 Hz = 14745.6 kHz
// Clock value: 460.800 kHz (divider = CLK/32)
// Mode: Normal top=FFh
// divisor = 17 -> 600us timer interval (1.66 KHz)
// divisor = 20 -> 45us timer interval
// divisor = 30 -> 90us timer interval
#if (XTIMERS_CUSTOM_DIVISOR)

#else
const UCHAR TIMER_DIVISOR = 25;
#endif
const UCHAR TIMER_COUNTER = 256 - TIMER_DIVISOR;

XTIME const SYS_TICKS_PER_SECOND = (11500000/32)/TIMER_DIVISOR;
XTIME sys_current_tick_count = 0; // 11.764K ticks per second

// just for reference
//XTIME const SYS_TIMER_RESOLUTION_US = 1000*1000/SYS_TICKS_PER_SECOND;

XTIME inline getTickCount()
{
    return sys_current_tick_count;
}

XTIME inline getTicksDelta(XTIME lastTickCount)
{
    XTIME current_ticks = getTickCount();
    if (lastTickCount <= current_ticks)
    {
        return (XTIME)current_ticks - (XTIME)lastTickCount;
    }
    else
    {
        return ((XTIME)MAX_XTIME - (XTIME)lastTickCount) + (XTIME)(current_ticks);
    };
}

/*
bool inline isTimerEventUpdated(XTIME *outLastUpdateTime,XTIME updateInterval)
{
	XTIME updateTime = *outLastUpdateTime;
	if (getTicksDelta(updateTime) >= updateInterval)
	{
		*outLastUpdateTime = getTickCount();
		return true;
	}
	return false;
}
*/





void inline initTimer0()
{
	// Timer/Counter 0 initialization
	// Clock source: System Clock
	// Clock value: 460.800 kHz
	// Mode: Normal top=FFh
	// OC0 output: Disconnected
	ASSR=0x00;
	TCCR0=0x03;
	TCNT0=TIMER_COUNTER;
	OCR0=0x00;
}

// Disabled timer
void initTimer1()
{
	// Timer/Counter 1 initialization
	// Clock source: System Clock
	// Clock value: Timer 1 Stopped
	// Mode: Normal top=FFFFh
	// OC1A output: Discon.
	// OC1B output: Discon.
	// OC1C output: Discon.
	// Noise Canceler: Off
	// Input Capture on Falling Edge
	// Timer 1 Overflow Interrupt: Off
	// Input Capture Interrupt: Off
	// Compare A Match Interrupt: Off
	// Compare B Match Interrupt: Off
	// Compare C Match Interrupt: Off
	TCCR1A=0x00;
	TCCR1B=0x00;
	TCNT1H=0x00;
	TCNT1L=0x00;
	ICR1H=0x00;
	ICR1L=0x00;
	OCR1AH=0x00;
	OCR1AL=0x00;
	OCR1BH=0x00;
	OCR1BL=0x00;
	OCR1CH=0x00;
	OCR1CL=0x00;
}

void initTimer2()
{
	// Timer/Counter 2 initialization
	// Clock source: System Clock
	// Clock value: Timer 2 Stopped
	// Mode: Normal top=FFh
	// OC2 output: Disconnected
	TCCR2=0x00;
	TCNT2=0x00;
	OCR2=0x00;
}

void initTimer3()
{
	// Timer/Counter 3 initialization
	// Clock source: System Clock
	// Clock value: Timer 3 Stopped
	// Mode: Normal top=FFFFh
	// Noise Canceler: Off
	// Input Capture on Falling Edge
	// OC3A output: Discon.
	// OC3B output: Discon.
	// OC3C output: Discon.
	// Timer 3 Overflow Interrupt: Off
	// Input Capture Interrupt: Off
	// Compare A Match Interrupt: Off
	// Compare B Match Interrupt: Off
	// Compare C Match Interrupt: Off
	TCCR3A=0x00;
	TCCR3B=0x00;
	TCNT3H=0x00;
	TCNT3L=0x00;
	ICR3H=0x00;
	ICR3L=0x00;
	OCR3AH=0x00;
	OCR3AL=0x00;
	OCR3BH=0x00;
	OCR3BL=0x00;
	OCR3CH=0x00;
	OCR3CL=0x00;
}


void inline enableTimers()
{
	// Timers Interrupts initialization
	TIMSK=0x01;
	ETIMSK=0x00;
}

#endif