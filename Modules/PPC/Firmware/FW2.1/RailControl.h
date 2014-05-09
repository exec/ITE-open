#ifndef H_PPC_RAIL_CONTROL_H
#define H_PPC_RAIL_CONTROL_H

// dependencies
// none
#include "PPC_ADC64.h"

#define NUM_OF_RAILS (3)

typedef struct tagRAILSTATE
{
	unsigned char power_enabled;

	// these are bit values (one or zero)
	unsigned char rail_computer;
	unsigned char rail_motion;

	unsigned char port_value;

	XUINT16 rail_current_adc_code[NUM_OF_RAILS];
} RAILSTATE;

// === Public interface ===
void inline InitRailControl();
void inline RailSet(unsigned char rail_computer, unsigned char rail_motion);
void inline RailUpdateOutput();

// === Private interface ===



// === Implementation ===



RAILSTATE railState;

// must be called each cycle (timer cycle or main cycle)
void inline RailUpdateOutput()
{
	if (adcState.all_results_estimated != 0)
	{
		railState.rail_current_adc_code[0] = adcState.input_values[ADC_INPUT_OSNS1];
		railState.rail_current_adc_code[1] = adcState.input_values[ADC_INPUT_OSNS2];
		railState.rail_current_adc_code[2] = adcState.input_values[ADC_INPUT_OSNS3];
	};

	if (railState.power_enabled != 0)
		railState.port_value = (railState.rail_computer) | (railState.rail_motion << 1);
	else
		railState.port_value = 0;
}

void inline RailSet(unsigned char rail_computer, unsigned char rail_motion)
{
	railState.rail_computer	= rail_computer;
	railState.rail_motion	= rail_motion;

	RailUpdateOutput();
}



void inline InitRailControl()
{
	railState.rail_computer = 0;
	railState.rail_motion = 0;

	railState.port_value = 0;

	PORTA	&= 0b11111100; // set POWER rails state
	DDRA	|= 0b00000011; // set POWER rails to outputs

	railState.power_enabled = 1;
}

#endif