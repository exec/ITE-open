#ifndef H_LED_CONTROL_H
#define H_LED_CONTROL_H

#include "XDataTypes.h"
#include "XTimers64.h"

const XTIME LED_BLINKER_UPDATE_INTERVAL = SYS_TICKS_PER_SECOND; // led update interval is 1000 ms

typedef struct tagLED_CONTROL
{
	XTIME lastUpdateTickCount;
	UCHAR led_blinker_trigger;
	UCHAR state_Led;  // bits 3 (gr) & 4 (red)

} LED_CONTROL;

LED_CONTROL LedControl;

void inline initLeds()
{
	// configure port directions
	PORTG &= 0b00111; // turn off leds
	DDRG |= 0b11000; // set leds to outputs

	LedControl.lastUpdateTickCount = getTickCount();
	LedControl.led_blinker_trigger = 0;
	LedControl.state_Led = 0;
}

void inline setLedGreen(UCHAR state)
{
	if (state == 0)
	{
		LedControl.state_Led &= 0b10000;
	}
	else
	{
		LedControl.state_Led |= 0b01000; // set green led
	}
}

void inline setLedRed(UCHAR state)
{
	if (state == 0)
	{
		LedControl.state_Led &= 0b01000;
    }
	else
	{
		LedControl.state_Led |= 0b10000; // set red led
	}
}

void inline setLeds(UCHAR red, UCHAR green)
{
	LedControl.state_Led = ((red & 0x01) << 4) | ((green & 0x01) << 3);
}

void inline updateLeds()
{
	if (getTicksDelta(LedControl.lastUpdateTickCount) >= LED_BLINKER_UPDATE_INTERVAL)
    {
        LedControl.lastUpdateTickCount = getTickCount();
        //perform led processings

        // Debug
		if (LedControl.led_blinker_trigger == 0)
		{
			setLeds(1,0);
			LedControl.led_blinker_trigger = 1;
		}
		else
		{
			setLeds(0,1);
			LedControl.led_blinker_trigger = 0;
		}

	} // LED timer

	PORTG = LedControl.state_Led;
}

void inline rainbowLeds()
{
	setLedRed(1);
    updateLeds();
    delay_ms(40);

    setLedGreen(1);
    updateLeds();
    delay_ms(50);

    setLedRed(0);
    updateLeds();
    delay_ms(30);

    setLedGreen(0);
    updateLeds();
    delay_ms(70);

    setLedGreen(1);
    updateLeds();
    delay_ms(33);


    setLeds(0,0);
    updateLeds();
    delay_ms(100);

    setLeds(1,1);
    updateLeds();
    delay_ms(50);

    setLeds(0,0);
    updateLeds();
    delay_ms(100);

    setLeds(1,1);
    updateLeds();
    delay_ms(50);

    setLeds(0,0);
    updateLeds();
}



#endif