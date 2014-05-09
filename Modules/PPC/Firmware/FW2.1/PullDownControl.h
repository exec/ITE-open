#ifndef H_PPC_PullDown_CONTROL_H
#define H_PPC_PullDown_CONTROL_H

// dependencies
// none

/*
	Pull Down outputs control


 */

typedef struct tagPULLDOWNSTATE
{
	unsigned char enabled;

	// these are bit values (one or zero)
	unsigned char pull1_power;
	unsigned char pull2_reset;
	unsigned char pull3;

	// these are port output values (will be masked)
	unsigned char portA_value;
	unsigned char portG_value;
} PULLDOWNSTATE;

// === Public interface ===
void inline InitPullDownControl();
void inline PullDownSet(unsigned char pull1_power, unsigned char pull2_reset,
	unsigned char pull3);

// === Private interface ===
void inline PullDownUpdateOutput();

// ========= Implementation =============


PULLDOWNSTATE pullDownState;

void inline InitPullDownControl()
{
	pullDownState.pull1_power = 0;
	pullDownState.pull2_reset = 0;
	pullDownState.pull3 = 0;

	pullDownState.portA_value = 0;
	pullDownState.portG_value = 0;

	// setup portA outputs
	PORTA	&= 0b00111111;
	DDRA	|= 0b11000000;

	// setup portG outputs - portG is not bit-modificable
	PORTG	&= 0b11011;
	DDRG	|= 0b00100; // set PG.2 to output

	// now we can enable Pull-Down
	pullDownState.enabled = 1;
}




void inline PullDownSet(unsigned char pull1_power, unsigned char pull2_reset,
	unsigned char pull3)
{
	pullDownState.pull1_power	= pull1_power;
	pullDownState.pull2_reset	= pull2_reset;
	pullDownState.pull3		= pull3;

	PullDownUpdateOutput();
}

void inline PullDownUpdateOutput()
{
	if (pullDownState.enabled)
	{
		pullDownState.portA_value = (pullDownState.pull1_power << 6) | (pullDownState.pull2_reset << 7);
		pullDownState.portG_value = (pullDownState.pull3 << 2);
	}
	else
	{
		pullDownState.portA_value = 0;
		pullDownState.portG_value = 0;
	}

}

#endif