#ifndef H_PPC_FAN_CONTROL_H
#define H_PPC_FAN_CONTROL_H

// dependencies
#include "../../../FirmwareLibs/XPwmSequences.h"

// FAN PWM power range
const unsigned char PWR_FAN_MIN = 0; // 8/27 * 27
const unsigned char PWR_FAN_MAX = 27; // 100%

unsigned char allow_fans = 1;

typedef struct tagFANSTATE
{
	unsigned char enabled;
	unsigned char pwm_mask_current;
	unsigned char pwm_output_current;
	unsigned char power_current;
} FANSTATE;

FANSTATE fan1;
FANSTATE fan2;
FANSTATE fan3;
FANSTATE fan4;


void inline private_FanInit(FANSTATE *fan)
{
	fan->enabled = 0;
	fan->pwm_mask_current = 0;
	fan->power_current = PWR_FAN_MAX;
};

void inline FanEnable(FANSTATE *fan)
{
	fan->enabled = 1;
}

void inline FanDisable(FANSTATE *fan)
{
	fan->enabled = 0;
}

// min = 0, max = 24
void inline FanSetPower(FANSTATE *fan, unsigned char fan_pwr_setup)
{
	if (fan_pwr_setup < PWR_FAN_MIN)
	{
		fan->power_current = PWR_FAN_MIN;
	}
	else
	if (fan_pwr_setup >  PWR_FAN_MAX)
	{
		fan->power_current = PWR_FAN_MAX;
	}
	else
		fan->power_current = fan_pwr_setup;
}

void inline private_FanUpdatePWM(FANSTATE *fan)
{
    fan->pwm_mask_current = pwm_seq_lin_27[fan->power_current][pwm27_idx];
	fan->pwm_output_current = fan->pwm_mask_current & fan->enabled;
}

void inline InitFans()
{
	private_FanInit(&fan1);
	private_FanInit(&fan2);
	private_FanInit(&fan3);
	private_FanInit(&fan4);

	// Initialize ports and their values
    DDRD = 0b11110000; // set up PORT configuration
	PORTD = 0b00001111; // set up pull-up to PORTD-inputs
}

// must be called each cycle (timer cycle or main cycle)
void inline FansPWMUpdate()
{
    private_FanUpdatePWM(&fan1);
	//private_FanUpdatePWM(&fan2);
	private_FanUpdatePWM(&fan3);
	private_FanUpdatePWM(&fan4);
};

void inline FansPortUpdate()
{
	PORTD = (fan1.pwm_output_current << 4) | (fan2.pwm_output_current << 5) |
		(fan3.pwm_output_current << 6) | (fan4.pwm_output_current << 7);
}

#endif