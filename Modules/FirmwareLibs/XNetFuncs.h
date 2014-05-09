#ifndef H_NET_ATM64_PROTOCOL_FUNCS_H
#define H_NET_ATM64_PROTOCOL_FUNCS_H

#include "../../../FirmwareLibs/XDataTypes.h"
#include "../../../FirmwareLibs/XNetTypes.h"
#include "../../../FirmwareLibs/XUSART64.h"

#include "string.h"



// forward declaration of device-specific command handler
void networkUpdateDeviceSpecific(NET_PACKETHEADER *pNetHdr,UCHAR input_command, UCHAR *in_buffer);

//
void networkUpdate()
{
	UCHAR i;
	UCHAR input_command;
	UCHAR in_buffer[RX_BUFFER_SIZE];

	NET_PACKETHEADER net_hdr;


	char inSize = inputBufferCountReady();

#if (0)
	if (isInputChar())
	{
		UCHAR c = getchar();
		putchar(c);
	};
	return;
#endif

	// 1) we read signature
	// 2) if signature fails - invalidate all input buffer
	if (inSize >= sizeof(NET_PACKETHEADER))
	{
	    UCHAR extra_bytes = inSize - sizeof(NET_PACKETHEADER);

		resetGetCharCopy();

        net_hdr.signature1 = getcharCopy();
		net_hdr.signature2 = getcharCopy();
		net_hdr.length = getcharCopy();
		net_hdr.cmd = getcharCopy();

		if ((net_hdr.signature1 != NET_SIGNATURE1) ||
			(net_hdr.signature2 != NET_SIGNATURE2))
		{
			uart_invalidateInputBuffer();
			return;
		}

		if (extra_bytes < net_hdr.length)
		{
			return; // we have just to wait until next command comes
		}
	}
	else
		return; // wait until enough data comes

	// now we're certain that we will have enough data
	getcharForce(); // signature1
	getcharForce(); // signature2
	getcharForce(); // length
	getcharForce(); // cmd

	// Recieve input Extra packet data
	for (i=0; i < net_hdr.length; i++)
	{
		in_buffer[i] = getcharForce(); // cmd
	}

	input_command = net_hdr.cmd;
	net_hdr.cmd |= NET_ANS_COMMAND_MASK;

	// Process command
 	switch (input_command)
	{
		case(NET_CMD_REQ_ECHO):
		{
			net_hdr.length = NET_CMD_SIZE_ANS_ECHO;
			uart_send_buf((UCHAR*)&net_hdr,sizeof(net_hdr));

			break;
		}

		case(NET_CMD_REQ_DEVICECLASS):
		{
			NET_PACKET_ANS_DEVICECLASS ans_deviceclass;
			net_hdr.length = NET_CMD_SIZE_ANS_DEVICECLASS;
			uart_send_buf((UCHAR*)&net_hdr,sizeof(net_hdr));

			ans_deviceclass.device_class = NET_DEVICE_CLASS;
			uart_send_buf((UCHAR*)&ans_deviceclass,NET_CMD_SIZE_ANS_DEVICECLASS);

			break;
		}

		case(NET_CMD_REQ_DEVICENAME):
		{
			NET_PACKET_ANS_DEVICENAME ans_devicename;
			net_hdr.length = NET_CMD_SIZE_ANS_DEVICENAME;
			uart_send_buf((UCHAR*)&net_hdr,sizeof(net_hdr));

			strcpy(ans_devicename.devicename,NET_DEVICE_NAME);
			uart_send_buf((UCHAR*)&ans_devicename,NET_CMD_SIZE_ANS_DEVICENAME);

			break;
		}
		default:
			networkUpdateDeviceSpecific(&net_hdr,input_command,in_buffer);
			// just ignore packet
			// or send "unknown_command" reply
			break;
	}
}










#endif