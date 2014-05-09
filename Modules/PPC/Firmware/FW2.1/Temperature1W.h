#ifndef H_PPC_TEMPERATURE_1WIRE_H
#define H_PPC_TEMPERATURE_1WIRE_H

// dependencies
/* specify the Atmega64A port and bit used for the 1 Wire bus */
#asm
    .equ __w1_port=0x18 ;PORTB
    .equ __w1_bit=0
#endasm

/* include the DS1820/DS18S20 functions prototypes */
#include <ds18b20.h>

/*
	Temperature reader
 */
#define FAIL_TEMPERATURE (-9999)

typedef struct tagTEMPERATURESTATE
{
	unsigned char enabled;

	// this is ROM-code
	unsigned char rom_code1[8];
	unsigned char rom_code2[8];
	unsigned char rom_code3[8];

	signed int temperature1;
	signed int temperature2;
	signed int temperature3;

	float temperature1_result;
	float temperature2_result;
	float temperature3_result;

	// these are port output values (will be masked)
	unsigned char portA_value;
	unsigned char portG_value;
} TEMPERATURESTATE;

// === Public interface ===
void inline InitTemperature();
void inline TemperatureReadUpdate();

// === Private interface ===


// ========= Implementation =============


TEMPERATURESTATE temperatureState;

void inline InitTemperature()
{
	temperatureState.temperature1 = 0;
	temperatureState.temperature2 = 0;
	temperatureState.temperature3 = 0;

	//B8C530
	temperatureState.rom_code1[0] = 0x30;
	temperatureState.rom_code1[1] = 0xC5;
	temperatureState.rom_code1[2] = 0xB8;
	temperatureState.rom_code1[3] = 0x0;
	temperatureState.rom_code1[4] = 0x0;
	temperatureState.rom_code1[5] = 0x0;
	temperatureState.rom_code1[6] = 0x0;
	temperatureState.rom_code1[7] = 0x0;

	// setup portB 1-Wire input-output
	PORTB.0 = 0;
	DDRB.0 = 1;

	ds18b20_init(0, 0, 0, DS18B20_9BIT_RES);

	// now we can enable temperature processing
	temperatureState.enabled = 1;
}

void inline TemperatureReadUpdate()
{
	if (temperatureState.enabled != 0)
	{
		temperatureState.temperature1_result = ds18b20_temperature(0);
		if (temperatureState.temperature1_result != FAIL_TEMPERATURE)
		{
			temperatureState.temperature1 = (signed int)(temperatureState.temperature1_result);
		};

		temperatureState.temperature2 = 0;
		temperatureState.temperature3 = 0;
	};
}

#endif