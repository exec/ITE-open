#ifndef H_PPC_BATCHARGE_CONTROL_H
#define H_PPC_BATCHARGE_CONTROL_H

// dependencies
#include "../../../FirmwareLibs/XDataTypes.h"

#include "PPC_ADC64.h"
#include "InputControl.h" // for CHARGER_GOOD signal

/*
	Battery Charge Controlling logic

Stage1: Constant Current Stage.


Refer: http://batteryuniversity.com/learn/article/charging_lithium_ion_batteries
	for details

 */

#define NUM_OF_BATTERIES (6)
//const UCHAR NUM_OF_BATTERIES = 6;

#define NUM_OF_BATINBLOCK (6)

// Battery Voltage Level sensing (CHARGER)
const float BATTERY_VOLTAGE_PRESENT	=	2.0*NUM_OF_BATINBLOCK; // 12 Volts - battery block is present if higher than this
const float BATTERY_VOLTAGE_MINIMAL	=	3.0*NUM_OF_BATINBLOCK; // 18 Volts
const float BATTERY_VOLTAGE_LOW		=	3.9*NUM_OF_BATINBLOCK; // 6*3.7;	// 22.2 Volts - this will hold battery in charged state
const float BATTERY_VOLTAGE_TRIM	=	4.2*NUM_OF_BATINBLOCK; // 25.02 Volts (trim charging at this level)

// These values are retrieved from schematic design. Voltage divisor:
const float BATTERY_DIVIDER_INPUT_MAX = 30.0f;  // 30 Volts max
const float BATTERY_DIVIDER_OUTPUT_MAX = 4.57f; // 4.57 volts may come to ADC (at 30V input)

// this indicates what ADC code will be when there is MAXIMAL voltage at ADC = BATTERY_DIVIDER_VOLTAGE_MAX
const XUINT16 BATTERY_ADC_CODE_MAX = (XUINT16)((BATTERY_DIVIDER_OUTPUT_MAX/MAX_ADC_INPUT_VOLTAGE)*MAX_ADC_VALUE);

// these are final ADC codes
const XUINT16 BATTERY_ADC_CODE_PRESENT = (XUINT16)((BATTERY_VOLTAGE_PRESENT/BATTERY_DIVIDER_INPUT_MAX)*BATTERY_ADC_CODE_MAX);
const XUINT16 BATTERY_ADC_CODE_MINIMAL = (XUINT16)((BATTERY_VOLTAGE_MINIMAL/BATTERY_DIVIDER_INPUT_MAX)*BATTERY_ADC_CODE_MAX);
const XUINT16 BATTERY_ADC_CODE_LOW		= (XUINT16)((BATTERY_VOLTAGE_LOW/BATTERY_DIVIDER_INPUT_MAX)*BATTERY_ADC_CODE_MAX);
const XUINT16 BATTERY_ADC_CODE_TRIM	= (XUINT16)((BATTERY_VOLTAGE_TRIM/BATTERY_DIVIDER_INPUT_MAX)*BATTERY_ADC_CODE_MAX);


typedef struct tagBATCHARGECONTROL
{
	UCHAR enable_charge;

	XUINT16 voltage_levels[NUM_OF_BATTERIES]; // ADC values. 10 bits

	UCHAR charge_rail_enabled[NUM_OF_BATTERIES];

	XUINT16 initial_levels[NUM_OF_BATTERIES]; // helps determine if batery was present
	UCHAR bat_present[NUM_OF_BATTERIES]; //

	UCHAR charged_battery_count;
	UCHAR all_batteries_charged;

	UCHAR present_battery_count;

	UCHAR first_run_with_correct_values;

   /*
	UCHAR debug;

	XUINT16 adc_code_present;
	UCHAR debug1;

	XUINT16 adc_code_minimal;
	UCHAR debug2;

	XUINT16 adc_code_low;
	UCHAR debug3;

	XUINT16 adc_code_trim;
	UCHAR debug4;
    */

} BATCHARGECONTROL;

//int voltages @0x80;


// === Public interface ===
void inline InitBatChargeControl();

void inline BatChargeUpdate();

// === Private interface ===

// ========= Implementation =============


//BATCHARGECONTROL batChargeState @ 0x80;
BATCHARGECONTROL batChargeState @ 0x0C00;

void inline InitBatChargeControl()
{
	UCHAR i;
	for (i = 0; i < NUM_OF_BATTERIES; i++)
	{
		batChargeState.voltage_levels[i] = 0;
		batChargeState.charge_rail_enabled[i] = 0;
		batChargeState.initial_levels[i] = 0;
		batChargeState.bat_present[i] = 0;
	};

	// setup portB battery control outputs
	PORTB	&= 0b00000011;
	DDRB	|= 0b11111100; // direction - output (higher 6 bits)

	batChargeState.enable_charge = 1;

	batChargeState.first_run_with_correct_values = 1;
	batChargeState.present_battery_count = 0;

	/*
	batChargeState.debug = 0xFF;

	batChargeState.adc_code_present = BATTERY_ADC_CODE_PRESENT;
	batChargeState.adc_code_minimal = BATTERY_ADC_CODE_MINIMAL;
	batChargeState.adc_code_low = BATTERY_ADC_CODE_LOW;
	batChargeState.adc_code_trim = BATTERY_ADC_CODE_TRIM;
    */

}

UCHAR inline isBatShallStopCharge(XUINT16 adc_value_code)
{
	if (adc_value_code > BATTERY_ADC_CODE_TRIM)
		return 1;
	return 0;
};

UCHAR inline isBatLevelLow(XUINT16 adc_value_code)
{
	if (adc_value_code <= BATTERY_ADC_CODE_LOW)
    	return 1;
	return 0;
};

UCHAR inline isBatPresent(XUINT16 adc_value_code)
{
	if (adc_value_code >= BATTERY_ADC_CODE_PRESENT)
    	return 1;
	return 0;
};

void inline BatChargeUpdate()
{
    UCHAR batIdx;
    
    // We wait until every ADC result is retrieved 
    if (adcState.all_results_estimated == 0)
		return;

	batChargeState.enable_charge = signalState.current_signal_state[SIGNAL_INDEX_SPINT_CHARGER_GOOD];

	// store ADC conversion result into private structure
	for (batIdx = 0; batIdx < NUM_OF_BATTERIES; batIdx++)
	{
		batChargeState.voltage_levels[batIdx] = adcState.input_values[ADC_INPUT_BSNS1+batIdx];
    };

	// Perform Battery-present detection
	if (batChargeState.first_run_with_correct_values == 1)
	{
		batChargeState.present_battery_count = 0;
		for (batIdx = 0; batIdx < NUM_OF_BATTERIES; batIdx++)
		{
            batChargeState.initial_levels[batIdx] = batChargeState.voltage_levels[batIdx];
			if (isBatPresent(batChargeState.voltage_levels[batIdx]))
			{
				batChargeState.bat_present[batIdx] = 1;
				batChargeState.present_battery_count++;
			};
		}
		batChargeState.first_run_with_correct_values = 0;
	};

    // Allow detection of previously not inserted batteries and
    // add them to the available list
	for (batIdx = 0; batIdx < NUM_OF_BATTERIES; batIdx++)
	{
		if (batChargeState.bat_present[batIdx] == 0)
		{
			if (isBatPresent(batChargeState.voltage_levels[batIdx]))
			{
				batChargeState.bat_present[batIdx] = 1;
				batChargeState.present_battery_count++;
			}
		}
	}

	// Count number of charged batteries
	batChargeState.charged_battery_count = 0;

	for (batIdx = 0; batIdx < NUM_OF_BATTERIES; batIdx++)
	{
		// different logic for battery in-charging process and stand-by
		if (batChargeState.charge_rail_enabled[batIdx]) // is charging
		{
        	if (isBatShallStopCharge(batChargeState.voltage_levels[batIdx]))
			{
				batChargeState.charge_rail_enabled[batIdx] = 0; // disable battery charge
				if (batChargeState.bat_present[batIdx]) // this battery is charged
					batChargeState.charged_battery_count++;
			};
		}
		else
		{
			if (isBatLevelLow(batChargeState.voltage_levels[batIdx]))
			{
				batChargeState.charge_rail_enabled[batIdx] = 1;
			}
			else
			{
				// no, battery is not charged and it's level is not low
				if (batChargeState.bat_present[batIdx])
					batChargeState.charged_battery_count++;
			}
		}
	}

	// this will trigger SPI (Side Panel) "CHARGED" indication later
	if (batChargeState.charged_battery_count >= batChargeState.present_battery_count)
		batChargeState.all_batteries_charged = 1;
	else
		batChargeState.all_batteries_charged = 0;

	// === TODO: !!! ADD Transistor PORTB.x TIME Hysteresis (1 second)
	// !!! DO NOT ALLOW enabling/disabling transistor in less than 1 second !!!

    // NOW all charger ENABLE/DISABLE transistor logic is performed
	// and we can modify transistor state
	if (batChargeState.enable_charge)
	{
		UCHAR rail[6];
        UCHAR tmpPB;
        
        rail[0] = batChargeState.charge_rail_enabled[0] & batChargeState.bat_present[0];
		rail[1] = batChargeState.charge_rail_enabled[1] & batChargeState.bat_present[1];
		rail[2] = batChargeState.charge_rail_enabled[2] & batChargeState.bat_present[2];
		rail[3] = batChargeState.charge_rail_enabled[3] & batChargeState.bat_present[3];
		rail[4] = batChargeState.charge_rail_enabled[4] & batChargeState.bat_present[4];
		rail[5] = batChargeState.charge_rail_enabled[5] & batChargeState.bat_present[5];
        
        tmpPB = PORTB;
        tmpPB &= 0b00000011;
        tmpPB = (rail[0] << 2) | 
                (rail[1] << 3) |
                (rail[2] << 4) |
                (rail[3] << 5) |
                (rail[4] << 6) |
                (rail[5] << 7);
		PORTB = tmpPB;
	}
	else
	{
    	// disable all rails
        PORTB &= 0b00000011;
	};
}



#endif