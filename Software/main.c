/** 
 * @mainpage PICTrak
 *
 * @section overview_sec Overview
 *
 * The PICTrak aims to be a self-contained APRS based tracking system that generates modulated audio (AFSK) to interface
 * with an FM transmitter.  It uses an on-board 4 bit resistor DAC followed by an RC LPF to generate the audio.  It expect
 * NMEA (GGA and RMC strings) in from a GPS on the hardware UART ports.  Currently it's designed to run on a Microchip 
 * PIC18F14K22 clocked at 32 Mhz although porting to another architecture shouldn't be too difficult (tm) :-)  
 * 
 *
 * @section copyright_sec Copyright
 *
 * Copyright (c) 2011 Elijah Brown, AD7ZJ
 *
 *
 * @section gpl_sec GNU General Public License
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 * 
 *
 * @section history_sec Revision History 
 *
 * 		@subsection v001 V0.01
 * 		30 Mar 2011, fullofbugsandcrap release.
 *
 */

#include <htc.h>
#include <stdio.h>
#include <stdlib.h>
#include "ax25.h"
#include "serial.h"

#define _XTAL_FREQ 32000000
__CONFIG(1, OSC_INT_RC_CLKOUT_ON_OSC2 & PLLEN_OSCILLATOR_MULTIPLIED_BY_4 & PCLKEN_PRIMARY_CLOCK_DISABLED);
__CONFIG(2, WDTEN_DISABLED_CONTROLLED_BY_SWDTEN_BIT);
__CONFIG(3, MCLRE_MCLR_DISABLED_RA3_ENABLED);
//__CONFIG(4, XINST_DISABLED & BACKBUG_DISABLED);

unsigned char test = 0x20;
unsigned char testChar = 0;

void sysInit(void) {
	// port directions: 1=input, 0=output
	LATA = 0x00;
	TRISA = 0b00000000;
	LATB = 0x00;
	TRISB = 0b00100000;
	LATC = 0x00;
	TRISC = 0b00000000;
	OSCCON = 0b01100000;
	PLLEN = 0x01;
	// Enable ADC channel 8 Disable channel 2
	ANSEL = 0b00000000;
	ANSELH = 0b00000000;

	/*ResetEn();
	Reset();
	En_QIO();
	flashWREN();
	WriteBlockProtection();
	while(Read_Status_Register() & 0x80);
	flashWREN();
	//test = Read_Status_Register();
	QuadJ_ID_Read();
	while(Read_Status_Register() & 0x80);

	flashWREN();
	Page_Program(0x000000);
	//while(Read_Status_Register() & 0x80);
	test = Read_Status_Register();
	//while(Read_Status_Register() & 0x80);
	testChar = HighSpeed_Read(0x000000);
    */

	// Clear the timers
	TMR1H = 0x00;
	TMR1L = 0x00;
	// Timer 1 setup
	T1CON = 0b00110001;
	TMR1IE = 0x01;
	T2CON = 0b00000100;

	// GIE, PEIE Interrupts
	INTCON = 0b11000000;
	RCIE = 0x01;
	secCount = 0;
}

volatile char buff = 0;

void main(void) {
	int i = 0;
	sysInit();
	serial_setup();
	
	// configure the TNC
	configDefault();

	

	while(1) {
		if(secCount > 100) {
			// Right now it's just in a continuous loop sending packets every 5 seconds for debugging
			// Prepare a packet with this message
			tncPreparePacket("$GPGGA,,3436.89881,N,11227.02683,W,7,00,,,,,,,*42");
			// Send it
			tncSendPacket();
			// Testing serial crap
			printf("This is a test: %c Timer: %d\r\n", buff, TMR1L);
			printf("Status reg:%X\r\n", test);
			//printf("Manufacturer ID:%X Dev type: %X Dev ID: %X\r\nRead:%X\r\n", Manufacturer_Id, Device_Type, Device_Id, testChar);
			secCount = 0;
		}
		if(gpsMode == GPS_COMPLETE_STRING) {
			putst(serBuffer);
			gpsMode = GPS_RECEIVING_STRING;
			serIndex = 0;
		}
	}
}
	

interrupt isr(void) {
	// Serial receive interrupt
	if(RCIF) {
		buff = RCREG;
		serBuffer[serIndex] = buff;
		serIndex++;
		if(buff == '\r') {
			// Assume a complete packet
			serBuffer[serIndex] = '\0';
			gpsMode = GPS_COMPLETE_STRING;
		}
	}

	// Timer 1 interrupt every 50ms
	if(TMR1IF) {
		secCount++;
		// Clear interrupt flag & reload timer
		TMR1IF = 0x00;
		TMR1H = 0xC3;
		TMR1L = 0x50;
	}
}