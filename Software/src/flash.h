//*****************
// Public functions

void FlashInit();

void ResetEn();

void ResetFlash();

unsigned char Read_Status_Register();

void flashWREN();

void QuadJ_ID_Read();

unsigned char HighSpeed_Read(unsigned long Dst);

void Page_Program(unsigned long Dst);

void WriteBlockProtection();

void En_QIO();

void Block_Erase(unsigned long Dst);

void ReadBlockProtection();

/* global array to store Manufacturer and Device id information */
unsigned char Manufacturer_Id, Device_Type, Device_Id;

extern unsigned char block_protection_6[6];