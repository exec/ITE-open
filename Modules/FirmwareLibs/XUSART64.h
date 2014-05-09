#ifndef H_ATM64USART_H
#define H_ATM64USART_H

#include "XDataTypes.h"


// Standard Input/Output functions
#include <stdio.h>

// Value = fosc/(16*BAUD)-1 = 14745600/(16*57600) - 1
//const UCHAR USART_9600 = 95;	// 9600 (value = 95) = 14745600/(16*9600) - 1
//const UCHAR USART_19200 = 47;	// 19200 (value = ) = 14745600/(16*19200) - 1
//const UCHAR USART_38400 = 23;	// 38400 (value = ) = 14745600/(16*38400) - 1
const UCHAR USART_57600 = 15;	// 57600 (value = 15) = 14745600/(16*57600) - 1
//const UCHAR USART_115200 = 7;	// 115200 (value = ) = 14745600/(16*115200) - 1







#define RXB8 1
#define TXB8 0
#define UPE 2
#define OVR 3
#define FE 4
#define UDRE 5
#define RXC 7

#define FRAMING_ERROR (1<<FE)
#define PARITY_ERROR (1<<UPE)
#define DATA_OVERRUN (1<<OVR)
#define DATA_REGISTER_EMPTY (1<<UDRE)
#define RX_COMPLETE (1<<RXC)

#define RX_BUFFER_SIZE 196
#define TX_BUFFER_SIZE 196

//===========================================================================================
//===========================================================================================
//===========================================================================================
#if (DEFAULT_USART_0)
//-----------------------------------
// USART0 Receiver buffer
#define RX_BUFFER_SIZE0 RX_BUFFER_SIZE
char rx_buffer0[RX_BUFFER_SIZE0];

#if RX_BUFFER_SIZE0<256
UCHAR rx_wr_index0,rx_rd_index0,rx_counter0;
UCHAR rx_rd_index0copy; // this is a copy of rx_rd_index0 for copy functions
#else
XUINT16 rx_wr_index0,rx_rd_index0,rx_counter0;
XUINT16 rx_rd_index0copy;
#endif

// This flag is set on USART0 Receiver buffer overflow
bit rx_buffer_overflow0;

#else // USART1
//-----------------------------------
// USART1 Receiver buffer
#define RX_BUFFER_SIZE1 RX_BUFFER_SIZE
char rx_buffer1[RX_BUFFER_SIZE1];

#if RX_BUFFER_SIZE1<256
UCHAR rx_wr_index1,rx_rd_index1,rx_counter1;
UCHAR rx_rd_index1copy; // this is a copy of rx_rd_index0 for copy functions
#else
XUINT16 rx_wr_index1,rx_rd_index1,rx_counter1;
XUINT16 rx_rd_index1copy;
#endif

// This flag is set on USART0 Receiver buffer overflow
bit rx_buffer_overflow1;

#endif
//==========================================================================



#if (DEFAULT_USART_0)
//-----------------------------------
// USART0: Receiver interrupt service routine
interrupt [USART0_RXC] void usart0_rx_isr(void)
{
	char status,data;
	status=UCSR0A;
	data=UDR0;
	if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
	{
		rx_buffer0[rx_wr_index0]=data;
		if (++rx_wr_index0 == RX_BUFFER_SIZE0) rx_wr_index0=0;
		if (++rx_counter0 == RX_BUFFER_SIZE0)
		{
			rx_counter0=0;
			rx_buffer_overflow0=1;
		};
	};
}
#else
//-----------------------------------
// USART1: Receiver interrupt service routine
interrupt [USART1_RXC] void usart1_rx_isr(void)
{
	char status,data;
	status=UCSR1A;
	data=UDR1;
	if ((status & (FRAMING_ERROR | PARITY_ERROR | DATA_OVERRUN))==0)
	{
		rx_buffer1[rx_wr_index1]=data;
		if (++rx_wr_index1 == RX_BUFFER_SIZE1) rx_wr_index1=0;
		if (++rx_counter1 == RX_BUFFER_SIZE1)
		{
			rx_counter1=0;
			rx_buffer_overflow1=1;
		};
	};
}
#endif

#ifndef _DEBUG_TERMINAL_IO_
// Get a character from the USART0 Receiver buffer
#define _ALTERNATE_GETCHAR_
#pragma used+

#if (DEFAULT_USART_0)
//-----------------------------------
// USART0: this is a blocking UART0 function
char getchar(void)
{
	char data;
	while (rx_counter0==0); // wait until single char comes to buffer
	data=rx_buffer0[rx_rd_index0];
	if (++rx_rd_index0 == RX_BUFFER_SIZE0) rx_rd_index0=0;
	#asm("cli")
	--rx_counter0;
	#asm("sei")
	return data;
}
#else
//-----------------------------------
// USART1:
char getchar(void)
{
	char data;
	while (rx_counter1==0); // wait until single char comes to buffer
	data=rx_buffer1[rx_rd_index1];
	if (++rx_rd_index1 == RX_BUFFER_SIZE1) rx_rd_index1=0;
	#asm("cli")
	--rx_counter1;
	#asm("sei")
	return data;
}
#endif

#pragma used-
#endif


#ifndef _DEBUG_TERMINAL_IO_


#if (DEFAULT_USART_0)
//-----------------------------------
// USART0: Get a character from the USART0 Receiver buffer
char inline isInputChar(void)
{
	if (rx_counter0==0) return 0;
	return 1;
}

// USART0:
char inline getcharForce(void) // gets char, assumes char is already in buf. Non-blocking
{
	char data;
	data=rx_buffer0[rx_rd_index0];
	if (++rx_rd_index0 == RX_BUFFER_SIZE0) rx_rd_index0=0;
	#asm("cli")
	if (rx_counter0) --rx_counter0;
	#asm("sei")
	return data;
}

// USART0: this just resets non-touching buffer index
void inline resetGetCharCopy()
{
	rx_rd_index0copy = rx_rd_index0;
}

// there must be a char already
char inline getcharCopy(void) // it does not remove character from the buffer
{
	char data;
	data=rx_buffer0[rx_rd_index0copy]; // just return char, do not modify buffer
	if (++rx_rd_index0copy == RX_BUFFER_SIZE0) rx_rd_index0copy=0;
	return data;
}

UCHAR inline inputBufferCountReady(void)
{
	return rx_counter0;
}
#else
//-----------------------------------
// USART1: Get a character from the USART1 Receiver buffer
char inline isInputChar(void)
{
	if (rx_counter1==0) return 0;
	return 1;
}

// USART1
char inline getcharForce(void) // gets char, assumes char is already in buf. Non-blocking
{
	char data;
	data=rx_buffer1[rx_rd_index1];
	if (++rx_rd_index1 == RX_BUFFER_SIZE1) rx_rd_index1=0;
	#asm("cli")
	if (rx_counter1) --rx_counter1;
	#asm("sei")
	return data;
}

// USART1: this just resets non-touching buffer index
void inline resetGetCharCopy()
{
	rx_rd_index1copy = rx_rd_index1;
}

// USART1: there must be a char already
char inline getcharCopy(void) // it does not remove character from the buffer
{
	char data;
	data=rx_buffer1[rx_rd_index1copy]; // just return char, do not modify buffer
	if (++rx_rd_index1copy == RX_BUFFER_SIZE1) rx_rd_index1copy=0;
	return data;
}

// USART1:
UCHAR inline inputBufferCountReady(void)
{
	return rx_counter1;
}
#endif






#endif // DEBUG_TERMINAL_IO



//===========================================================================================
//===========================================================================================
//===========================================================================================

#if (DEFAULT_USART_0)

// USART0 Transmitter buffer
#define TX_BUFFER_SIZE0 TX_BUFFER_SIZE
char tx_buffer0[TX_BUFFER_SIZE0];

#if TX_BUFFER_SIZE0<256
UCHAR tx_wr_index0,tx_rd_index0,tx_counter0;
#else
XUINT16 tx_wr_index0,tx_rd_index0,tx_counter0;
#endif

// USART0 Transmitter interrupt service routine
interrupt [USART0_TXC] void usart0_tx_isr(void)
{
	if (tx_counter0)
	{
		--tx_counter0;
		UDR0=tx_buffer0[tx_rd_index0];
		if (++tx_rd_index0 == TX_BUFFER_SIZE0) tx_rd_index0=0;
	};
}

#ifndef _DEBUG_TERMINAL_IO_
// Write a character to the USART0 Transmitter buffer
#define _ALTERNATE_PUTCHAR_
#pragma used+
void putchar(char c)
{
	while (tx_counter0 == TX_BUFFER_SIZE0);
	#asm("cli")
	if (tx_counter0 || ((UCSR0A & DATA_REGISTER_EMPTY)==0))
	{
		tx_buffer0[tx_wr_index0]=c;
		if (++tx_wr_index0 == TX_BUFFER_SIZE0) tx_wr_index0=0;
		++tx_counter0;
	}
	else
		UDR0=c;
	#asm("sei")
}
#pragma used-
#endif


#else
//-----------------------------------

// USART1 Transmitter buffer
#define TX_BUFFER_SIZE1 TX_BUFFER_SIZE
char tx_buffer1[TX_BUFFER_SIZE1];

#if TX_BUFFER_SIZE1<256
UCHAR tx_wr_index1,tx_rd_index1,tx_counter1;
#else
XUINT16 tx_wr_index1,tx_rd_index1,tx_counter1;
#endif

// USART0 Transmitter interrupt service routine
interrupt [USART1_TXC] void usart1_tx_isr(void)
{
	if (tx_counter1)
	{
		--tx_counter1;
		UDR1=tx_buffer1[tx_rd_index1];
		if (++tx_rd_index1 == TX_BUFFER_SIZE1) tx_rd_index1=0;
	};
}

#ifndef _DEBUG_TERMINAL_IO_
// Write a character to the USART0 Transmitter buffer
#define _ALTERNATE_PUTCHAR_
#pragma used+
void putchar(char c)
{
	while (tx_counter1 == TX_BUFFER_SIZE1);
	#asm("cli")
	if (tx_counter1 || ((UCSR1A & DATA_REGISTER_EMPTY)==0))
	{
		tx_buffer1[tx_wr_index1]=c;
		if (++tx_wr_index1 == TX_BUFFER_SIZE1) tx_wr_index1=0;
		++tx_counter1;
	}
	else
		UDR1=c;
	#asm("sei")
}
#pragma used-
#endif





#endif





void uart_send_buf(char *buf, UCHAR size_bytes)
{
	UCHAR idx;
	for (idx = 0; idx < size_bytes; idx++)
	{
		putchar(*buf);
		buf++;
	}
}

void uart_get_bufForce(char *buf_receiver, UCHAR size_bytes)
{
	UCHAR idx;
	for (idx = 0; idx < size_bytes; idx++)
	{
		*buf_receiver = getcharForce();
		buf_receiver++;
	}
}

void uart_invalidateInputBuffer()
{
	// !!! Shall we put here #asm("cli"), #asm("sei") sequence ???
	while(1)
	{
		if (!isInputChar())
			break; // buffer is empty now
		getcharForce(); // not empty, get char now
	}
}



#if (DEFAULT_USART_0)

void initUSART()
{
	// USART0 initialization
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART0 Receiver: On
	// USART0 Transmitter: On
	// USART0 Mode: Asynchronous
	UCSR0A=0x00;
	UCSR0B=0xD8;
	UCSR0C=0x06;

	// Clock = f_osc = 14.7456 Mhz
	// UBRR0 = fosc/(16*BAUD)-1 = 14745600/(16*57600) - 1
	UBRR0H=0x00;
	UBRR0L=USART_57600;

	// UART automatically overrides RXD+TXD functions
}

#else

void initUSART()
{
	// USART0 initialization
	// Communication Parameters: 8 Data, 1 Stop, No Parity
	// USART1 Receiver: On
	// USART1 Transmitter: On
	// USART1 Mode: Asynchronous
	UCSR1A=0x00;
	UCSR1B=0xD8;
	UCSR1C=0x06;

	// Clock = f_osc = 14.7456 Mhz
	// UBRR0 = fosc/(16*BAUD)-1 = 14745600/(16*57600) - 1
	UBRR1H=0x00;
	UBRR1L=USART_57600;

	// UART automatically overrides RXD+TXD functions
}

#endif



#endif