#ifndef H_PPC_SIGNAL_CONTROL_H
#define H_PPC_SIGNAL_CONTROL_H

// dependencies
// none

/*
	Inputs control

 */

typedef struct tagSIGNALSTATE
{
	// these are bit values (one or zero)
	unsigned char port_value;

	unsigned char current_signal_state[4];
	unsigned char previous_signal_state[4];

	unsigned char signal_charged_state;

} SIGNALSTATE;

// === Public interface ===
void inline InitSignalControl();

void inline SignalReadUpdate();

// === Private interface ===

// ========= Implementation =============


SIGNALSTATE signalState;

void inline InitSignalControl()
{
	signalState.port_value = 0;

	signalState.current_signal_state[0] = 0;
	signalState.current_signal_state[1] = 0;
	signalState.current_signal_state[2] = 0;
	signalState.current_signal_state[3] = 0;

	signalState.previous_signal_state[0] = 0;
	signalState.previous_signal_state[1] = 0;
	signalState.previous_signal_state[2] = 0;
	signalState.previous_signal_state[3] = 0;

	signalState.signal_charged_state = 0;

	// SIGNAl1 = Input: SPInt_PWR_TRIGGER
	// SIGNAl2 = Input: SPInt_CHARGER_GOOD_TRIGGER
	// SIGNAl3 = Output: SPInt_CHARGED
	// SIGNAl4 = Input: PC_STARTED trigger

	// setup portC inputs and SIGNAL4 (PC.1 as output)
	PORTC	= 0b11100001;
	DDRC	= 0b00000100 | 0b11100001; // set other port pins to outputs to reduce standby power
}

void inline SignalReadUpdate()
{
	signalState.port_value = PINC; // read PINC state
	PORTC.2 = signalState.signal_charged_state;

	signalState.previous_signal_state[0] = signalState.current_signal_state[0];
	signalState.previous_signal_state[1] = signalState.current_signal_state[1];
	signalState.previous_signal_state[2] = signalState.current_signal_state[2];
	signalState.previous_signal_state[3] = signalState.current_signal_state[3];

	signalState.current_signal_state[0] = (signalState.port_value >> 4) & 0b1;
	signalState.current_signal_state[1] = (signalState.port_value >> 3) & 0b1;
	signalState.current_signal_state[2] = (signalState.port_value >> 2) & 0b1;
	signalState.current_signal_state[3] = (signalState.port_value >> 1) & 0b1;
}

const unsigned char SIGNAL_INDEX_SPINT_POWER = 0;
const unsigned char SIGNAL_INDEX_SPINT_CHARGER_GOOD = 1;
//const unsigned char SIGNAL_INDEX_SPINT_CHARGED = 2; // this is output
const unsigned char SIGNAL_INDEX_PC_POWERED_UP = 3; // PC led input


unsigned char inline InputsTransientHigh(unsigned char PinIndex)
{
	if ((signalState.current_signal_state[PinIndex] == 1) &&
		(signalState.previous_signal_state[PinIndex] == 0))
	{
		return 1;
	}
	else
		return 0;
};

unsigned char inline InputsTransientLow(unsigned char PinIndex)
{
	if ((signalState.current_signal_state[PinIndex] == 0) &&
		(signalState.previous_signal_state[PinIndex] == 1))
	{
		return 1;
	}
	else
		return 0;
};

void inline SignalsIndicateCharged(unsigned char isCharged)
{
	signalState.signal_charged_state = isCharged;
};

#endif