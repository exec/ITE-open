#ifndef H_PPC_XADC64_H
#define H_PPC_XADC64_H

#include "../../../FirmwareLibs/XDataTypes.h"


const UCHAR NUM_CONVERSIONS_SKIP = 10; // skip first 10 conversions
const XUINT16 MAX_ADC_VALUE = 1024;
const float MAX_ADC_INPUT_VOLTAGE = 5.0f; // 5.0 volts may come to ADC when possible

#define FIRST_ADC_INPUT 0
#define LAST_ADC_INPUT 3
#define ADC_VREF_TYPE 0x40 // AVCC


/*
	Battery Charge Controlling logic
 */

const UCHAR NUM_OF_BATTERIES = 6;

typedef struct tagXADC
{
	UCHAR all_results_estimated;
	volatile XUINT16 input_values[9];
	UCHAR current_index; // from 0 to 9

	UCHAR current_mux; // 0 to 6
	UCHAR enable_mux;

	UCHAR mcu_channel_index; // hardware ADC channel

	UCHAR num_first_convertions;
	volatile XUINT16 last_conversion_result;
} XADC;

// ADC input indexes
const UCHAR ADC_INPUT_BSNS1 = 0;
const UCHAR ADC_INPUT_BSNS2 = 1;
const UCHAR ADC_INPUT_BSNS3 = 2;
const UCHAR ADC_INPUT_BSNS4 = 3;
const UCHAR ADC_INPUT_BSNS5 = 4;
const UCHAR ADC_INPUT_BSNS6 = 5;

const UCHAR ADC_INPUT_OSNS1 = 6;
const UCHAR ADC_INPUT_OSNS2 = 7;
const UCHAR ADC_INPUT_OSNS3 = 8;



// === Public interface ===
void inline InitADC();

// === Private interface ===
void inline private_setADCMuxChannel(UCHAR idx);

// ========= Implementation =============


XADC adcState;

void inline private_setADCMuxChannel(UCHAR idx)
{
	if (idx <= 5)
	{
		adcState.current_mux = idx; // mux channel is IDX
		adcState.current_index = idx; // real channel is IDX
		adcState.mcu_channel_index = 0; // adc hardware mux is channel 0
	}
	else
	{
		adcState.current_mux = 0; // reset mux
		adcState.current_index = idx;
		adcState.mcu_channel_index = idx - 5;
	};

	// mux enable logic is inverted (pulled-high by default = disabled)
	if (adcState.enable_mux)
	{
		PORTF = ((adcState.current_mux & 0b111) << 5);
	}
	else
	{
		PORTF = (1 << 4) | ((adcState.current_mux & 0b111) << 5);
	}
};

// ADC interrupt service routine
// with auto input scanning
interrupt [ADC_INT] void adc_isr(void)
{
	// skip first iterations
	if (adcState.num_first_convertions < NUM_CONVERSIONS_SKIP)
	{
		adcState.last_conversion_result = ADCW;
		adcState.num_first_convertions++;

		// Start the AD conversion
		ADCSRA|=0x40;

		return;
	}

	// Otherwise read the AD conversion result

	// save ADC result to storage variable
	adcState.input_values[adcState.current_index] = ADCW;

	// Select next ADC input
	// now iterate multiplexed channel
	adcState.current_index++;
	if (adcState.current_index >= 9)
	{
		adcState.current_index = 0;
		adcState.all_results_estimated = 1;
	};

	// this will change ADC input if required
	private_setADCMuxChannel(adcState.current_index);

	// Setup hardware MCU_ADC multiplexer input
	ADMUX=(FIRST_ADC_INPUT | (ADC_VREF_TYPE & 0xff)) + adcState.mcu_channel_index;

	// Delay needed for the stabilization of the ADC input voltage
	delay_us(10);

	// Start the AD conversion
	ADCSRA|=0x40;
}

void inline InitADC()
{
	UCHAR i;
	for (i = 0; i < 9; i++)
	{
		adcState.input_values[i] = 0;
	}

	adcState.enable_mux = 1;

	private_setADCMuxChannel(0);

	adcState.num_first_convertions = 0;
	adcState.last_conversion_result = 0;

	PORTF	= 0b00000000;
	DDRF	= 0b11110000;

	// this will be 1 when first series of results for all ADC inputs is ready
	adcState.all_results_estimated = 0;

	// ADC initialization
	// ADC Clock frequency: 115.200 kHz
	// ADC Voltage Reference: AVCC pin
	ADMUX=FIRST_ADC_INPUT | (ADC_VREF_TYPE & 0xff);
	ADCSRA=0xCF;
}

#endif