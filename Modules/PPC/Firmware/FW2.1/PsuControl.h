#ifndef H_PPC_PSU_CONTROL_H
#define H_PPC_PSU_CONTROL_H

// dependencies
// none

/*
	POWER Supply control


 */

typedef struct tagPSUSTATE
{
	unsigned char power_enabled;

	// these are bit values (one or zero)
	unsigned char rail1V8;
	unsigned char rail3V3;
	unsigned char rail5V;
	unsigned char rail12V;

	unsigned char port_value;
} PSUSTATE;

// === Public interface ===
void inline InitPsuControl();
void inline PsuSet(unsigned char rail1V8, unsigned char rail3V3,
	unsigned char rail5V, unsigned char rail12V);

// === Private interface ===
void inline PsuUpdateOutput();

// ========= Implementation =============


PSUSTATE psuState;

void inline InitPsuControl()
{
	psuState.rail1V8 = 0;
	psuState.rail3V3 = 0;
	psuState.rail5V = 0;
	psuState.rail12V = 0;

	psuState.port_value = 0;

	PORTA	&= 0b11000011;
	DDRA	|= 0b00111100;

	psuState.power_enabled = 1;
}




void inline PsuSet(unsigned char rail1V8, unsigned char rail3V3,
	unsigned char rail5V, unsigned char rail12V)
{
	psuState.rail1V8	= rail1V8;
	psuState.rail3V3	= rail3V3;
	psuState.rail5V		= rail5V;
	psuState.rail12V	= rail12V;

	PsuUpdateOutput();
}

void inline PsuUpdateOutput()
{
	if (psuState.power_enabled)
		psuState.port_value =  (psuState.rail1V8 << 2) | (psuState.rail3V3 << 3) |
			(psuState.rail5V << 4) | (psuState.rail12V << 5);
	else
		psuState.port_value = 0;

}

#endif