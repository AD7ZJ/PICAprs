/*Software Driver

SST26VF016 16 Mbit(2M x 8) Serial Quad I/O (SQI) Flash Memory

January 16, 2009, Rev. 1.0

ABOUT THE SOFTWARE
This application note provides software driver examples for SST26VF016,
Serial Flash. Extensive comments are included in each routine to describe 
the function of each routine.  The interface coding uses polling method 
rather than the SPI protocol to interface with these serial devices.  The
functions are differentiated below in terms of the communication protocols
(uses Mode 0) and specific device operation instructions. This code has been 
designed to compile using the Keil compiler.


ABOUT THE SST26VF016

Companion product datasheets for the SST26VF016 should be reviewed in 
conjunction with this application note for a complete understanding 
of the device.


Device Communication Protocol(pinout related) functions:

Functions                    		Function
------------------------------------------------------------------
init				Initializes clock to set up mode 0.
Send_Byte			Sends one byte using SI pin to send and 
				shift out 1-bit per clock rising edge
Get_Byte			Receives one byte using SO pin to receive and shift 
				in 1-bit per clock falling edge

SendSQI_Byte		 	Sends one byte using SI pin, SO pin, SIO2 pin and SIO3 pin to send and 
				shift out 4-bit per clock rising edge

GetSQI_Byte			Receives one byte using SI pin, SO pin, SIO2 pin and SIO3 pin to receive and shift 
				in 4-bit per clock falling edge

Note:  The pin names of the SST26VF016 are used in this application note. The associated test code
will not compile unless these pinouts (SCK, SI, SO, SIO2, SIO3, CE) are pre-defined on your
software which should reflect your hardware interfaced. 	 




Device Operation Instruction functions:

Functions                    		Function
------------------------------------------------------------------

NoOp				No Operation
RSTEN				Enables acceptance of the RST (Reset) operation command 
RST				Resets the device in to normal operating Ready mode			
EQIO				Enables Quad I/O operation
RSTQIO				Resets the device to 1-bit SPI protocol operation

Read				Reads one byte from the serial flash and returns byte(max of 33 MHz CLK frequency)
Read_Cont			Reads multiple bytes(max of 33 MHz CLK frequency)
HighSpeed_Read			Reads one byte from the serial flash and returns byte(max of 80 MHz CLK frequency)
HighSpeed_Read_Cont		Reads multiple bytes(max of 80 MHz CLK frequency)

Set_Burst			Specifies the number of bytes (8,16,32 or 64 bytes) to output during a Read Burst command
Read_Burst			Reads multiple bytes as specified by Set_Burst
Read_PI				Jumps to address within 256Byte page and outputs data starting from the specified address
Read_Index			Jumps to address within 64KByte block and outputs data starting from the specified address
Read_BlockIndex		   	Jumps to address by specified number of 64KByte blocks and outputs data starting from the specified address

Jedec_ID_Read			Reads the Jedec ID using SPI protocol
Quad J-ID			Reads the Jedec ID using Quad I/O protocal

Sector_Erase			Erases one sector (4 KB) of the serial flash
Block_Erase			Erases 32 KByte block memory of the serial flash
Chip_Erase			Erases entire serial flash

Page_Program			Programs 1 to 256 Data Bytes

Write Suspend		   	Suspends Program/Erase  operation
Write Resume		   	Resumes Program/Erase operation
Read SID			Read Security ID
Prog SID			Program User Security ID area
Lock SID			Lockout Security ID Programming

RDSR				Reads the status register of the serial flash
flashWREN				Write enables the serial flash
WRDI				Write disables the serial flash

RBPR				Read Block Protection Register
WBPR				Write Block Protection Register
LBPR				Lock Block Protection Register

Wait_Busy			Polls status register and waits until busy bit is low



*/

                                                                     
//"C" LANGUAGE DRIVERS 

/********************************************************************/
/* Copyright Silicon Storage Technology, Inc. (SST), 1994-2009	    */
/* Example "C" language Driver of SST26VF016 Serial Flash	    */
/* Hardik Patel, Silicon Storage Technology, Inc.                   */
/*                                                                  */
/* Revision 1.0, January 16, 2009			  	    */   
/*                                                                  */
/*								    */
/********************************************************************/


  /* Function Prototypes */

void init();

void NoOp();

void Send_Byte(unsigned char out);

void SendSQI_Byte(unsigned char out);

unsigned char Get_Byte();

unsigned char GetSQI_Nibble();

unsigned char GetSQI_Byte();

void CE_High(); 

void CE_Low(); 

void ResetEn();

void Reset();

void En_QIO();

void Reset_QIO();

unsigned char Read(unsigned long Dst); 

void Read_Cont(unsigned long Dst, unsigned long no_bytes);

unsigned char HighSpeed_Read(unsigned long Dst); 

void HighSpeed_Read_Cont(unsigned long Dst, unsigned long no_bytes);

void Set_Burst(unsigned char byte);

void Read_Burst(unsigned long Dst, unsigned char burstlength); 

void Read_PI(unsigned char Dst, unsigned char datalength); 

void Read_Index(unsigned long Dst, unsigned char datalength); 

void Read_BlockIndex(unsigned char Dst, unsigned char datalength); 

void Jedec_ID_Read(); 

void QuadJ_ID_Read();

void Sector_Erase(unsigned long Dst);

void Block_Erase(unsigned long Dst);

void Chip_Erase();

void Page_Program(unsigned long Dst);

void Write_Suspend();

void Write_Resume();

void ReadSID(unsigned char Dst, unsigned char security_length); 

void ProgSID(); 

void LockSID(); 

unsigned char Read_Status_Register();

void flashWREN();

void WRDI();

void ReadBlockProtection(); 

void WriteBlockProtection(); 

void LockBlockProtection(); 

void Wait_Busy();

 
unsigned char data_128[80] = {'h', 'i', ',', 't', 'h', 'e', 'r', 'e'};	/* global array to store read data */

unsigned char block_protection_6[6] = {0, 0, 0, 0, 0, 0};	/* global array to store block_protection data */

unsigned char security_id_32[32];	/* global array to store security_id data */

unsigned char  Manufacturer_Id, Device_Type, Device_Id; /* global array to store Manufacturer and Device id information */	

#define SCK RB6
#define SI	RB4
#define	CE	RC6
#define	SO	RC4
#define	SIO2 RC5
#define	SIO3 RC7
				

/************************************************************************/
/* PROCEDURE: init							*/
/*									*/
/* This procedure initializes the SCK to low. Must be called prior to 	*/
/* setting up mode 0.							*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Output:								*/
/*		SCK							*/
/************************************************************************/
void init()
{
	SCK = 0;	/* set clock to low initial state */
}

/************************************************************************/
/* PROCEDURE: Send_Byte							*/
/*									*/
/* This procedure outputs a byte shifting out 1-bit per clock rising	*/
/* edge on the the SI pin(LSB 1st).					*/
/*									*/
/* Input:								*/
/*		out							*/
/*									*/
/* Output:								*/
/*		SI							*/
/************************************************************************/
void Send_Byte(unsigned char out)
{
	
	unsigned char i = 0;
	for (i = 0; i < 8; i++)
	{
		
		if ((out & 0x80) == 0x80)	/* check if MSB is high */
			SI = 1;
		else
			SI = 0;			/* if not, set to low */
		SCK = 1;			/* toggle clock high */
		out = (out << 1);		/* shift 1 place for next bit */
		SCK = 0;			/* toggle clock low */
	}
}


/************************************************************************/
/* PROCEDURE: SendSQI_Nibble						*/
/*									*/
/* This procedure outputs a byte shifting out 1-bit per clock rising	*/
/* edge on the the SI pin(LSB 1st).					*/
/*									*/
/* Input:								*/
/*		out							*/
/*									*/
/* Output:								*/
/*		SI, SO, SIO2, SIO3					*/
/************************************************************************/
void SendSQI_Byte(unsigned char out)
{


	if ((out & 0x80) == 0x80)		/* check if MSB is high */
			{SIO3 = 1;}
		else						 
			{SIO3 = 0;}
		
			
		if ((out & 0x40) ==0x40)
			{SIO2 = 1;}
		else
			{SIO2 = 0;}
		
		if ((out & 0x20) ==0x20)
			{SO = 1;}
		else
			{SO = 0;}
		
		if ((out & 0x10) ==0x10)
			{SI = 1;}
		else
			{SI = 0;}
				
		SCK = 1;			/* toggle clock high */
					
		SCK = 0;			/* toggle clock low */

			
	if ((out & 0x08) == 0x08)	        /* check if MSB is high */
			{SIO3 = 1;}
		else
			{SIO3 = 0;}
					
		if ((out & 0x04) ==0x04)
			{SIO2 = 1;}
		else
			{SIO2 = 0;}
		
		if ((out & 0x02) ==0x02)
			{SO = 1;}
		else
			{SO = 0;}
		
		if ((out & 0x01) ==0x01)
			{SI = 1;}
		else
			{SI = 0;}
				
		SCK = 1;			/* toggle clock high */
		
		SI=1;  //Set them as Inputs
		SO=1;
		SIO2=1;
		SIO3=1;			
				
		SCK = 0;			/* toggle clock low */

		   	
}

   
/************************************************************************/
/* PROCEDURE: Get_Byte							*/
/*									*/
/* This procedure inputs a byte shifting in 1-bit per clock falling	*/
/* edge on the SO pin(LSB 1st).						*/
/*									*/
/* Input:								*/
/*		SO							*/
/*									*/
/* Output:								*/
/*		None							*/
/************************************************************************/
unsigned char Get_Byte()
{
	unsigned char i = 0, in = 0, temp = 0;
	for (i = 0; i < 8; i++)
	{
		in = (in << 1);		/* shift 1 place to the left or shift in 0 */
		temp = SO;		/* save input */
		SCK = 1;		/* toggle clock high */
	if (temp == 1)			/* check to see if bit is high */
		in = in | 0x01;		/* if high, make bit high */

		SCK = 0;		/* toggle clock low */

	}
	return in;
}

      	  
   
/************************************************************************/
/* PROCEDURE: GetSQI_Nibble						*/
/*									*/
/* This procedure inputs a byte shifting in 1-bit per clock falling	*/
/* edge on the SI pin, SO pin, SIO2 pin and SIO3 pin.			*/
/*									*/
/* Input:								*/
/*		SI, SO, SIO2, SIO3					*/
/*									*/
/* Output:								*/
/*		None							*/
/************************************************************************/
unsigned char GetSQI_Nibble()
{
	unsigned char in = 0, temp3 = 0, temp2 =0, temp1 = 0, temp0=0;
	
	temp3=SIO3;
	temp2=SIO2;
	temp1=SO;
	temp0=SI;
	SCK=1;
	
	if (temp3==1)
	{in=1;}
	in = (in<<1);
	if (temp2==1)
	{in=in|0x01;}
	in = (in<<1);
	if (temp1==1)
	{in = in|0x01;}
	in = (in<<1);
	if (temp0==1)
	{in = in|0x01;}
	
	SCK = 0;			/* toggle clock low */

	return in;
}

/************************************************************************/
/* PROCEDURE: GetSQI_Byte						*/
/*									*/
/* This procedure call GetSQI_Nibble function twice 			*/
/* to get 1 byte of data						*/
/*									*/
/* Input:								*/
/*		1 byte of data						*/
/*									*/
/* Output:								*/
/*		None							*/
/************************************************************************/
unsigned char GetSQI_Byte()
{
	unsigned char temp, in = 0;
	
	in=GetSQI_Nibble();
	in=(in<<4);
	temp=GetSQI_Nibble();
	in=in|temp;
	
	return in;	

}



/************************************************************************/
/* PROCEDURE: CE_High							*/
/*									*/
/* This procedure set CE = High.					*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Output:								*/
/*		CE							*/
/*									*/
/************************************************************************/
void CE_High() 
{
	CE = 1;				/* set CE high */
}

/************************************************************************/
/* PROCEDURE: CE_Low							*/
/*									*/
/* This procedure drives the CE of the device to low.  			*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Output:								*/
/*		CE							*/
/*									*/
/************************************************************************/
void CE_Low() 
{	
	CE = 0;				/* clear CE low */
}



/************************************************************************/
/* PROCEDURE: Read_Status_Register					*/
/*									*/
/* This procedure reads the status register and returns the byte.	*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		byte							*/
/************************************************************************/
unsigned char Read_Status_Register()
{
	unsigned char byte = 0;
	CE_Low();			/* enable device */
	SendSQI_Byte(0x05);		/* send RDSR command */
	byte = GetSQI_Byte();		/* receive byte */
	CE_High();			/* disable device */
	return byte;
}

 

/************************************************************************/
/* PROCEDURE: WREN							*/
/*									*/
/* This procedure enables the Write Enable Latch.  			*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void flashWREN()
{
	CE_Low();			/* enable device */
	SendSQI_Byte(0x06);		/* send WREN command */
	CE_High();			/* disable device */
}

/************************************************************************/
/* PROCEDURE: WRDI							*/
/*									*/
/* This procedure disables the Write Enable Latch.			*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void WRDI()
{
	CE_Low();			/* enable device */
	SendSQI_Byte(0x04);		/* send WRDI command */
	CE_High();			/* disable device */
}

/************************************************************************/
/* PROCEDURE: QuadJ_ID							*/
/*									*/
/* This procedure Reads the manufacturer's ID, device Type  		*/
/* and device ID.  It will use AFh as the command to read the ID.	*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		Manufacture's ID = BFh, Device Type =26h,		*/
/*		Device ID = 01h						*/
/*									*/
/************************************************************************/

void QuadJ_ID_Read()
{

	CE_Low();			/* enable device */
	SendSQI_Byte(0xAF);		/* send read ID command (90h or ABh) */
   	Manufacturer_Id = GetSQI_Byte();/* receive byte */
  	Device_Type = GetSQI_Byte();	/* receive byte */
  	Device_Id = GetSQI_Byte();	/* receive byte */
	CE_High();			/* disable device */
	
}

/************************************************************************/
/* PROCEDURE: Jedec_ID_Read						*/
/*									*/
/* This procedure Reads the manufacturer's ID (BFh), memory type (26h)  */
/* and device ID (01h).  It will use 9Fh as the JEDEC ID command.    	*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		Manufacture's ID = BFh, Device Type (26h), 		*/
/*		and Device ID (01h)					*/
/*									*/
/************************************************************************/
void Jedec_ID_Read() 
{


	CE_Low();			/* enable device */
	Send_Byte(0x9F);		/* send JEDEC ID command (9Fh) */
   	Manufacturer_Id = Get_Byte();	/* receive byte */
  	Device_Type = Get_Byte();	/* receive byte */
  	Device_Id = Get_Byte();		/* receive byte */

	CE_High();			/* disable device */
} 


/************************************************************************/
/* PROCEDURE:	Read							*/
/*									*/		
/* This procedure reads one address of the device.  It will return the 	*/
/* byte read in variable byte.						*/
/*									*/
/*									*/
/*									*/
/* Input:								*/
/*		Dst:	Destination Address 000000H - 1FFFFFH		*/
/*      								*/
/*									*/
/* Returns:								*/
/*		byte							*/
/*									*/
/************************************************************************/
unsigned char Read(unsigned long Dst) 
{
	unsigned char byte = 0;	

	CE_Low();			/* enable device */
	Send_Byte(0x03); 		/* read command */
	Send_Byte(((Dst & 0xFFFFFF) >> 16));	/* send 3 address bytes */
	Send_Byte(((Dst & 0xFFFF) >> 8));
	Send_Byte(Dst & 0xFF);
	byte = Get_Byte();
	CE_High();			/* disable device */
	return byte;			/* return one byte read */
}


/************************************************************************/
/* PROCEDURE:	Read_Cont						*/
/*									*/		
/* This procedure reads multiple addresses of the device and stores	*/
/* data into 128 byte buffer. Maximum number of bytes read 		*/
/* is limited 128 bytes							*/
/*									*/
/* Input:								*/
/*		Dst:		Destination Address 000000H - 1FFFFFH	*/
/*      	no_bytes	Number of bytes to read	(max = 128)	*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/*									*/
/************************************************************************/
void Read_Cont(unsigned long Dst, unsigned long no_bytes)
{
	unsigned long i = 0;
	CE_Low();				/* enable device */
	Send_Byte(0x03); 			/* read command */
	Send_Byte(((Dst & 0xFFFFFF) >> 16)); 	/* send 3 address bytes */
	Send_Byte(((Dst & 0xFFFF) >> 8));
	Send_Byte(Dst & 0xFF);
	if (no_bytes>128)
	{no_bytes=128;}
	for (i = 1; i < no_bytes; i++)		/* read until no_bytes is reached */
	{
		data_128[i-1] = Get_Byte();	
	}
	CE_High();				/* disable device */

}



/************************************************************************/
/* PROCEDURE:	HighSpeed_Read						*/
/*									*/		
/* This procedure reads one address of the device.  It will return the 	*/
/* byte read in variable byte.						*/
/*									*/
/*									*/
/*									*/
/* Input:								*/
/*		Dst:	Destination Address 000000H - 1FFFFFH		*/
/*      								*/
/*									*/
/* Returns:								*/
/*		byte							*/
/*									*/
/************************************************************************/
unsigned char HighSpeed_Read(unsigned long Dst) 
{
	unsigned char byte = 0;	

	CE_Low();				/* enable device */
	Send_Byte(0x0B); 			/* read command */
	Send_Byte(((Dst & 0xFFFFFF) >> 16));	/* send 3 address bytes */
	Send_Byte(((Dst & 0xFFFF) >> 8));
	Send_Byte(Dst & 0xFF);
	Send_Byte(0xFF);			/*dummy byte*/
	byte = Get_Byte();
	CE_High();				/* disable device */
	return byte;				/* return one byte read */
}

 
/************************************************************************/
/* PROCEDURE:	HighSpeed_Read_Cont					*/
/*									*/		
/* This procedure reads multiple addresses of the device and stores	*/
/* data into 128 byte buffer. Maximum number of bytes read is 		*/
/* limited to 128 bytes							*/
/*									*/
/* Input:								*/
/*		Dst:		Destination Address 000000H - 1FFFFFH	*/
/*      	no_bytes	Number of bytes to read	(max = 128)	*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/*									*/
/************************************************************************/
void HighSpeed_Read_Cont(unsigned long Dst, unsigned long no_bytes)
{
	unsigned long i = 0;
	CE_Low();				/* enable device */
	Send_Byte(0x0B); 			/* read command */
	Send_Byte(((Dst & 0xFFFFFF) >> 16)); 	/* send 3 address bytes */
	Send_Byte(((Dst & 0xFFFF) >> 8));
	Send_Byte(Dst & 0xFF);
	Send_Byte(0xFF);			/* dummy byte*/
	if (no_bytes>128)
	{no_bytes=128;}

	for (i = 1; i < no_bytes; i++)		/* read until no_bytes is reached */
	{
		data_128[i-1] = Get_Byte();	
	}
	CE_High();				/* disable device */
}

/************************************************************************/
/* PROCEDURE:	Set_Burst						*/
/*									*/
/* This procedure sets the burst length to either 8bytes or 		*/
/* 16bytes or 32bytes or 64bytes.					*/
/*									*/
/*									*/
/*									*/
/* Input:								*/
/*		byte:	00h,01h,02h or 03h for setting burst length	*/
/*      								*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/*									*/
/************************************************************************/
void Set_Burst(unsigned char byte)
{
	CE_Low();				/* enable device */
	SendSQI_Byte(0xC0); 			/* send Byte Program command */
	SendSQI_Byte(byte);			/* send byte to be programmed */
	CE_High();				/* disable device */
}


/************************************************************************/
/* PROCEDURE:	Read_Burst						*/
/*									*/		
/* This procedure reads multiple (burst) address of the device.		*/
/* The data is stored in an array. 					*/
/*									*/
/*									*/
/*									*/
/* Input:								*/
/*		Dst:	Destination Address 000000H - 1FFFFFH		*/
/*      								*/
/*									*/
/* Returns:								*/
/*		byte							*/
/*									*/
/************************************************************************/
void Read_Burst(unsigned long Dst, unsigned char burstlength) 
{
	unsigned char i;
	i=0;	

	CE_Low();				/* enable device */
	SendSQI_Byte(0x0C); 			/* read command */
	SendSQI_Byte(((Dst & 0xFFFFFF) >> 16));	/* send 3 address bytes */
	SendSQI_Byte(((Dst & 0xFFFF) >> 8));
	SendSQI_Byte(Dst & 0xFF);
	SendSQI_Byte(Dst & 0xFF);	   	/* Dummy cycle */
	
	if (burstlength>128)
	{burstlength=128;}	

	for (i=0;i<=(burstlength-1);i++)
	{
	data_128[i]=GetSQI_Byte();		
	}

	CE_High();				/* disable device */

}


/************************************************************************/
/* PROCEDURE:	Read_PI							*/
/*									*/		
/* This procedure increments the address counter to within a page of 256*/
/* bytes and begins reading from the new address location. The number of*/
/* address read is determined by the datalenght				*/
/*									*/
/*									*/
/*									*/
/* Input:								*/
/*	Dst: witin 256 bytes.  Data length: Number of bytes to read	*/
/*      								*/
/*									*/
/* Returns:								*/
/*	 data array							*/
/*									*/
/************************************************************************/
void Read_PI(unsigned char Dst, unsigned char datalength) 
{
	unsigned char i;
	i=0;	

	CE_Low();			/* enable device */
	SendSQI_Byte(0x08); 		/* read command */
	SendSQI_Byte(Dst & 0xFF);
	SendSQI_Byte(Dst & 0xFF); 	/* Dummy cycle */

	if (datalength>128)
	{datalength=128;}	
	

	for (i=0;i<=(datalength-1);i++)
	{
		data_128[i]=GetSQI_Byte();
	}

	CE_High();			/* disable device */

}


/************************************************************************/
/* PROCEDURE:	Read_Index						*/
/*									*/		
/* This procedure increments the address counter to within a 		*/
/* 64Kbyte block and begins reading from the new address location.	*/							
/*  The number of address read is determined by the datalenght		*/
/*									*/
/*									*/
/* Input:								*/
/*		Dst: Witing 64 Kbyte block. 				*/
/* 		Data length: Number of bytes to read			*/
/*      								*/
/*									*/
/* Returns:								*/
/*		data array						*/
/*									*/
/************************************************************************/
void Read_Index(unsigned long Dst, unsigned char datalength) 
{
	unsigned char i;
	i=0;	

	CE_Low();			/* enable device */
	SendSQI_Byte(0x09); 		/* read command */
	SendSQI_Byte(((Dst & 0xFFFF) >> 8));
	SendSQI_Byte(Dst & 0xFF);
	SendSQI_Byte(Dst & 0xFF); // Dummy cycle
	SendSQI_Byte(Dst & 0xFF); // Dummy cycle
	
	if (datalength>128)
	{datalength=128;}	


	for (i=0;i<=(datalength-1);i++)
	{
	data_128[i]=GetSQI_Byte();	
	}

	CE_High();			/* disable device */

}



/************************************************************************/
/* PROCEDURE:	Read_BlockIndex						*/
/*									*/		
/* This procedure increments the address counter a specified number 	*/
/* of 64Kbyte block and begins reading from the new address location.	*/
/*  The number of address read is determined by the datalenght		*/
/*									*/
/* Input:								*/
/*		Dst: A multiple of 64Kbyte block increment.		*/
/*	Data length: Number of bytes to read				*/
/*      								*/
/*									*/
/* Returns:								*/
/*		data array						*/
/*									*/
/************************************************************************/
void Read_BlockIndex(unsigned char Dst, unsigned char datalength) 
{
	unsigned char i;
	i=0;	

	CE_Low();			/* enable device */
	SendSQI_Byte(0x10); 		/* read command */
	SendSQI_Byte(Dst & 0xFF);
	SendSQI_Byte(Dst & 0xFF); // Dummy cycle
	SendSQI_Byte(Dst & 0xFF); // Dummy cycle

	if (datalength>128)
	{datalength=128;}	
	
	for (i=0;i<=(datalength-1);i++)
	{
 	data_128[i]=GetSQI_Byte();	
	}


	CE_High();			/* disable device */

}


/************************************************************************/
/* PROCEDURE:	Page_Program						*/
/*									*/
/* This procedure does page programming.  The destination		*/
/* address should be provided.						*/
/* The data array of 128 bytes contains the data to be programmed.  	*/
/* Since the size of the data array is 128 bytes rather than 256 bytes, */
/* this page program procedure programs 128 bytes			*/
/*									*/
/*									*/
/* Assumption:  Address being programmed is already erased and is NOT	*/
/*		block protected.					*/
/*									*/
/*									*/
/*									*/
/* Input:								*/
/*		Dst:		Destination Address 000000H - 1FFFFFH	*/
/*		data_128[128] contains 128 bytes of data to program.    */
/*      								*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/*									*/
/************************************************************************/
void Page_Program(unsigned long Dst)
{
	unsigned char i;
	i=0;	
  
	CE_Low();				/* enable device */
	SendSQI_Byte(0x02); 			/* send Byte Program command */
	SendSQI_Byte(((Dst & 0xFFFFFF) >> 16));	/* send 3 address bytes */
	SendSQI_Byte(((Dst & 0xFFFF) >> 8));
	SendSQI_Byte(Dst & 0xFF);
	for (i=0;i<=50;i++)
	{	SendSQI_Byte(data_128[i]);	/* send byte to be programmed */
	}
	CE_High();				/* disable device */
}
 
/************************************************************************/
/* PROCEDURE: Chip_Erase						*/
/*									*/
/* This procedure erases the entire Chip.				*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void Chip_Erase()
{						
	CE_Low();				/* enable device */
	SendSQI_Byte(0xC7);			/* send Chip Erase command (C7h) */
	CE_High();				/* disable device */
}

/************************************************************************/
/* PROCEDURE: Sector_Erase						*/
/*									*/
/* This procedure Sector Erases the Chip.				*/
/*									*/
/* Input:								*/
/*		Dst:		Destination Address 000000H - 1FFFFFH	*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void Sector_Erase(unsigned long Dst)
{


	CE_Low();				/* enable device */
	SendSQI_Byte(0x20);			/* send Sector Erase command */
	SendSQI_Byte(((Dst & 0xFFFFFF) >> 16)); /* send 3 address bytes */
	SendSQI_Byte(((Dst & 0xFFFF) >> 8));
	SendSQI_Byte(Dst & 0xFF);
	CE_High();				/* disable device */
}

/************************************************************************/
/* PROCEDURE: Block_Erase						*/
/*									*/
/* This procedure Block Erases 8Kbyte, 32 KByte or 64 KByte of the Chip.*/
/*									*/
/* Input:								*/
/*		Dst:		Destination Address 000000H - 1FFFFFH	*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void Block_Erase(unsigned long Dst)
{
	CE_Low();				/* enable device */
	SendSQI_Byte(0xD8);			/* send Block Erase command */
	SendSQI_Byte(((Dst & 0xFFFFFF) >> 16));	/* send 3 address bytes */
	SendSQI_Byte(((Dst & 0xFFFF) >> 8));
	SendSQI_Byte(Dst & 0xFF);
	CE_High();				/* disable device */
}



/************************************************************************/
/* PROCEDURE: NoOp							*/
/*									*/
/* No operation is performed.						*/
/*									*/
/* Input:								*/
/*	Nothing								*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void NoOp()
{
	CE_Low();				/* enable device */
	SendSQI_Byte(0x00);		
	CE_High();				/* disable device */
}


/************************************************************************/
/* PROCEDURE: ResetEn							*/
/*									*/
/* This procedure Enables acceptance of the RST (Reset) operation.	*/
/*									*/
/* Input: None								*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void ResetEn()
{
	CE_Low();				/* enable device */
	SendSQI_Byte(0x66);			
	CE_High();				/* disable device */
}



/************************************************************************/
/* PROCEDURE: Reset							*/
/*									*/
/* This procedure resets the device in to normal operating Ready mode.	*/
/*									*/
/* Input:  None								*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void Reset()
{
	CE_Low();				/* enable device */
	SendSQI_Byte(0x99);		
	CE_High();				/* disable device */
}

			  
/************************************************************************/
/* PROCEDURE: En_QIO							*/
/*									*/
/* This procedure enables quad I/O operation.				*/
/*									*/
/* Input: None								*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void En_QIO()
{
	CE_Low();				/* enable device */
	Send_Byte(0x38);		
	CE_High();				/* disable device */
}


/************************************************************************/
/* PROCEDURE: Reset_QIO							*/
/*									*/
/* This procedure resets the device to 1-bit SPI protocol operation.	*/
/*									*/
/* Input: None								*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void Reset_QIO()
{
	CE_Low();				/* enable device */
	SendSQI_Byte(0xff);		
	CE_High();				/* disable device */
}




/************************************************************************/
/* PROCEDURE: Write_Suspend						*/
/*									*/
/* This procedure suspends Program/Erase operation.			*/
/*									*/
/* Input: None								*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void Write_Suspend()
{
	CE_Low();				/* enable device */
	SendSQI_Byte(0xb0);		
	CE_High();				/* disable device */
}



/************************************************************************/
/* PROCEDURE: Write_Resume						*/
/*									*/
/* This procedure resumes Program/Erase operation.			*/
/*									*/
/* Input: None								*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void Write_Resume()
{
	CE_Low();				/* enable device */
	SendSQI_Byte(0x30);			
	CE_High();				/* disable device */
}


 /***********************************************************************/
/* PROCEDURE:	ReadSID	(Read Security ID)				*/
/*									*/		
/* This procedure reads the security ID					*/
/*									*/
/*									*/
/*									*/
/* Input:								*/
/*		Security Id destination address, Security Id length	*/
/*      								*/
/*									*/
/* Returns:								*/
/*		None							*/
/*									*/
/************************************************************************/
void ReadSID(unsigned char Dst, unsigned char security_length) 
{
	unsigned char byte = 0;	
	unsigned char i;
	i=0;	
	 	if (security_length>32)
	{ security_length=32;}

	CE_Low();				/* enable device */
	SendSQI_Byte(0x88); 		
	SendSQI_Byte(Dst & 0xFF);
	SendSQI_Byte(Dst & 0xFF);  //dummy
	


	for (i=0;i<security_length;i++)
	{ 
		security_id_32[i] = GetSQI_Byte(); 
	}
	CE_High();				/* disable device */
}



/************************************************************************/
/* PROCEDURE:	ProgSID	(Program Security ID)				*/
/*									*/		
/* This procedure programs the security ID				*/
/*									*/
/*									*/
/*									*/
/* Input:								*/
/*		None							*/
/*      								*/
/*									*/
/* Returns:								*/
/*	None								*/
/*									*/
/************************************************************************/
void ProgSID() 
{
	unsigned char byte = 0;	
	unsigned char i;
	i=0;	

	CE_Low();			/* enable device */
	SendSQI_Byte(0xa5); 		
	SendSQI_Byte(0x08);	  	/*address of user programmable area*/


	for (i=0;i<24;i++)
	{	
	SendSQI_Byte(security_id_32[i+8]);
	}
	CE_High();			/* disable device */
}


/************************************************************************/
/* PROCEDURE:	LockSID							*/
/*									*/		
/* This procedure Locks the security ID setting				*/
/*									*/
/*									*/
/*									*/
/* Input:								*/
/*	None								*/
/*      								*/
/*									*/
/* Returns:								*/
/*		None							*/
/*									*/
/************************************************************************/
void LockSID() 
{

	CE_Low();			/* enable device */
	SendSQI_Byte(0x85); 		
	CE_High();			/* disable device */
}




  /**********************************************************************/
/* PROCEDURE:	ReadBlockProtection					*/
/*									*/		
/* This procedure reads block protection register			*/
/*									*/
/*									*/
/*									*/
/* Input:								*/
/*		None							*/
/*      								*/
/*									*/
/* Returns:								*/
/*		None							*/
/*									*/
/************************************************************************/
void ReadBlockProtection() 
{
	unsigned char byte = 0;	
 	unsigned char i;
	i=0;	

	CE_Low();			/* enable device */
	SendSQI_Byte(0x72); 		

	for (i=6;i>0;i--)
	{	block_protection_6[i-1] = GetSQI_Byte();
	}
	CE_High();			/* disable device */
}


 
/************************************************************************/
/* PROCEDURE:	WriteBlockProtection					*/
/*									*/		
/* This procedure writes to block protection register			*/
/*									*/
/*									*/
/*									*/
/* Input:								*/
/*		None							*/
/*      								*/
/*									*/
/* Returns:								*/
/*	None								*/
/*									*/
/************************************************************************/
void WriteBlockProtection() 
{
	unsigned char byte = 0;	
   	unsigned char i;
	i=0;	

	CE_Low();			/* enable device */
	SendSQI_Byte(0x42); 		/* read command */

	for (i=6;i>0;i--)
	{	
		SendSQI_Byte(block_protection_6[i-1]);
	}
	CE_High();			/* disable device */
}


/************************************************************************/
/* PROCEDURE:	LockBlockProtection					*/
/*									*/		
/* This procedure locks the block protection register			*/
/*									*/
/*									*/
/*									*/
/* Input:								*/
/*	None								*/
/*      								*/
/*									*/
/* Returns:								*/
/*		None							*/
/*									*/
/************************************************************************/
void LockBlockProtection() 
{

	CE_Low();			/* enable device */
	SendSQI_Byte(0x8d); 		/* read command */
	CE_High();			/* disable device */
}



/************************************************************************/
/* PROCEDURE: Wait_Busy							*/
/*									*/
/* This procedure waits until device is no longer busy.			*/ 	
/* 									*/
/*									*/
/* Input:								*/
/*		None							*/
/*									*/
/* Returns:								*/
/*		Nothing							*/
/************************************************************************/
void Wait_Busy()
{
	while ((Read_Status_Register()& 0x80) == 0x80)	/* wait until not busy */
		Read_Status_Register();
}

