#ifndef H_NET_ATM64_PPC_PROTOCOL_H
#define H_NET_ATM64_PPC_PROTOCOL_H

#ifdef __CODEVISIONAVR__
#include "../../../FirmwareLibs/XDataTypes.h"
#include "../../../FirmwareLibs/XNetTypes.h"
#endif

const UCHAR NET_DEVICE_CLASS = DEVICE_CLASS_PPC;
const UCHAR NET_DEVICE_NAME[] = "PPC 2.5 (FW2.1)";

#ifdef __CODEVISIONAVR__
#include "../../../FirmwareLibs/XUSART64.h"
#include "../../../FirmwareLibs/XNetFuncs.h"
#include "string.h"

#endif



/*
UART protocol

Stages:
I - Enumeration
	1 - ECHO sync
	2 - Get Device code
	3 - Get Device name

II - Operation
	4 - set target values
	5 - get current state

III - Shutdown

*/







// _________________________________ EXTRA PACKET BYTES __________________________________

// =======================================================================================
// ======================== DEVICE-SPECIFIC PACKETS ======================================
// =======================================================================================

#ifndef __CODEVISIONAVR__
#pragma pack(push,1)
#endif

// ========================= DEVICE-SPECIFIC COMMANDS

const UCHAR NET_CMD_PPC_REQ_GETALL = NET_CMD_REQ_DEVICE_SPECIFIC + 0; // asks MCU to report most PPC status (battery, rails etc)
const UCHAR NET_CMD_PPC_REQ_SETALL = NET_CMD_REQ_DEVICE_SPECIFIC + 1; // update all settings
const UCHAR NET_CMD_PPC_REQ_SETFAN = NET_CMD_REQ_DEVICE_SPECIFIC + 2; // update FAN speed

const UCHAR NET_CMD_PPC_REQ_FORCECHARGE = NET_CMD_REQ_DEVICE_SPECIFIC + 0xF; // force all rails to be charge enabled

// ======== GETPOS  =====
typedef struct tagNET_PACKET_PPC_ANS_GETALL
{
    UCHAR current_level[NUM_OF_RAILS]; // current level for all 3 rails
    
    // Bat info
    UCHAR bat_voltage[NUM_OF_BATTERIES];
    UCHAR bat_present[NUM_OF_BATTERIES];
    UCHAR charge_rail_enabled[NUM_OF_BATTERIES];
    
} NET_PACKET_PPC_ANS_GETALL;

// ======== SETALL =====
typedef struct tagNET_PACKET_PPC_REQ_SETALL
{
	UCHAR random;
} NET_PACKET_PPC_REQ_SETALL;

// ======== SETFAN =====
typedef struct tagNET_PACKET_PPC_REQ_SETFAN
{
	UCHAR fan_power[4];	// 0..0xFF. 0 - disable fan. 0xFF - max power
} NET_PACKET_PPC_REQ_SETFAN;


const UCHAR NET_CMD_SIZE_PPC_REQ_GETALL = 0;
const UCHAR NET_CMD_SIZE_PPC_ANS_GETALL = sizeof(NET_PACKET_PPC_ANS_GETALL);

const UCHAR NET_CMD_SIZE_PPC_REQ_SETALL = sizeof(NET_PACKET_PPC_REQ_SETALL);
const UCHAR NET_CMD_SIZE_PPC_ANS_SETALL = 0;

const UCHAR NET_CMD_SIZE_PPC_REQ_SETFAN = sizeof(NET_PACKET_PPC_REQ_SETFAN);
const UCHAR NET_CMD_SIZE_PPC_ANS_SETFAN = 0;

const UCHAR NET_CMD_SIZE_PPC_REQ_FORCECHARGE = 0;
const UCHAR NET_CMD_SIZE_PPC_ANS_FORCECHARGE = 0;

#ifndef __CODEVISIONAVR__
#pragma pack(pop)
#endif


#ifdef __CODEVISIONAVR__





/*  
	TODO: replace with proper UART commands example for PPC
                                  
    // This is a sample debug string for camera controller
	PPC.REQ_GETALL	= $FF$FE$00$20
	PPC.REQ_SETALL	= $FF$FE$00$21
    PPC.REQ_SETFAN	= $FF$FE$04$22$FF$FF$FF$FF
    PPC.REQ_FORCECHARGE = $FF$FE$00$2F
*/
void networkUpdateDeviceSpecific(NET_PACKETHEADER *pNetHdr,UCHAR input_command, UCHAR *in_buffer)
{
	switch(input_command)
	{
		case(NET_CMD_PPC_REQ_GETALL):
		{
			NET_PACKET_PPC_ANS_GETALL ans_getall;
			pNetHdr->length = NET_CMD_SIZE_PPC_ANS_GETALL;
			uart_send_buf((UCHAR*)pNetHdr,sizeof(NET_PACKETHEADER));
                 
            {
				char i;
                for (i = 0; i < NUM_OF_RAILS; i++)
                {
            		ans_getall.current_level[i] = (UCHAR)(railState.rail_current_adc_code[i] >> 2);
                }
                
                for (i = 0; i < NUM_OF_BATTERIES; i++)
                {
                	ans_getall.bat_voltage[i] = (UCHAR)(batChargeState.voltage_levels[i] >> 2);
                    
                    ans_getall.bat_present[i]         = batChargeState.bat_present[i];
                    ans_getall.charge_rail_enabled[i] = batChargeState.charge_rail_enabled[i];
                }
            }

			uart_send_buf((UCHAR*)&ans_getall,NET_CMD_SIZE_PPC_ANS_GETALL);

			break;
		}

		case(NET_CMD_PPC_REQ_SETALL):
		{
			NET_PACKET_PPC_REQ_SETALL * data_setAll = (NET_PACKET_PPC_REQ_SETALL *)in_buffer;
			
			// ... change state

			pNetHdr->length = NET_CMD_SIZE_PPC_ANS_SETALL;
			uart_send_buf((UCHAR*)pNetHdr,sizeof(NET_PACKETHEADER));

			break;
		}
        
        case(NET_CMD_PPC_REQ_SETFAN):
		{
			NET_PACKET_PPC_REQ_SETFAN * data_setFan = (NET_PACKET_PPC_REQ_SETFAN *)in_buffer;
			
			// Set Fan status
            {
            	/*
                UCHAR power1 = (UCHAR)((XUINT16)data_setFan->fan_power[0] * PWR_FAN_MAX) >> 8;
            	UCHAR power2 = (UCHAR)((XUINT16)data_setFan->fan_power[1] * PWR_FAN_MAX) >> 8;
            	UCHAR power3 = (UCHAR)((XUINT16)data_setFan->fan_power[2] * PWR_FAN_MAX) >> 8;
            	UCHAR power4 = (UCHAR)((XUINT16)data_setFan->fan_power[3] * PWR_FAN_MAX) >> 8;
                */
                UCHAR power1 = data_setFan->fan_power[0];
                UCHAR power2 = data_setFan->fan_power[1];
                UCHAR power3 = data_setFan->fan_power[2];
                UCHAR power4 = data_setFan->fan_power[3];
            	
            
            	FanSetPower(&fan1, power1);
            	FanSetPower(&fan2, power2);
            	FanSetPower(&fan3, power3);
            	FanSetPower(&fan4, power4);
                
                if (power1)
                	FanEnable(&fan1);
                if (power2)
                	FanEnable(&fan2);
                if (power3)
                	FanEnable(&fan3);
                if (power4)
                	FanEnable(&fan4);
                    
            }

			pNetHdr->length = NET_CMD_SIZE_PPC_ANS_SETFAN;
			uart_send_buf((UCHAR*)pNetHdr,sizeof(NET_PACKETHEADER));

			break;
		}
        
        // Sets all charger rails to ALLOW CHARGE
        case(NET_CMD_PPC_REQ_FORCECHARGE):
		{
			// We add this here just to force all rails
			{
            	char i;
                for (i = 0; i < NUM_OF_BATTERIES; i++)
                {
            		batChargeState.bat_present[i] = 1;
                    batChargeState.charge_rail_enabled[i] = 1;
				}
            }                    	
        
			pNetHdr->length = NET_CMD_SIZE_PPC_ANS_FORCECHARGE;
			uart_send_buf((UCHAR*)pNetHdr,sizeof(NET_PACKETHEADER));

			break;
		}
        
		default:
			break;
	}
}




#endif






#endif
