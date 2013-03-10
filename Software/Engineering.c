#include "serial.h"
#include "ax25.h"
#include "flash.h"
#include <stdio.h>

void EngineeringConsole() {
    uint8_t temp = 0;

    if(serIndex) {
        switch(serBuffer[0]) {
            case 'h':
                putst("PICTrack Engineering Console\n");
                putst("1: Send APRS packet\n");
                putst("2: Read flash status reg\n");
                putst("3: Read flash JEDEC-ID\n");
                putst("4: Erase flash sector 0\n");
                putst("5: Program predtermined pattern to flash sector 0\n");
                putst("6: Read flash sector 0\n");
                //putst("7: Read block protection register\n");
                //putst("8: Write block protection register to all writeable\n");
                break;

            case '1':
                tncPreparePacket("$GPGGA,,3436.89881,N,11227.02683,W,7,00,,,,,,,*42");
                // Send it
                tncSendPacket();
                break;

            case '2':
                // read the status register
                while(Read_Status_Register() & 0x80);
                temp = Read_Status_Register();
                printf("Status reg: 0x%X\r\n", temp);
                break;

            case '3':
                // Read manufacturer's ID
                while(Read_Status_Register() & 0x80);
                QuadJ_ID_Read();
                printf("Manufacturer ID: 0x%X Dev type: 0x%X Dev ID: 0x%X\r\n", Manufacturer_Id, Device_Type, Device_Id);
                break;

            case '4':
                putst("Erasing block 0...");
                flashWREN();
                Block_Erase(0x000000);
                while(Read_Status_Register() & 0x80);
                putst("done!\n");
                break;

            case '5':
                putst("Programming block 0...");
                flashWREN();
                Page_Program(0x000000);
                while(Read_Status_Register() & 0x80);
                putst("done!\n");
                break;

            case '6':
                while(Read_Status_Register() & 0x80);
                for(int i = 0; i < 10; i++)
                    printf("%d", HighSpeed_Read(i));
                break;

            case '7':
                ReadBlockProtection();
                for (int i = 0; i < 5; i++)
                    printf("%X", block_protection_6[i]);
                putst("\n");
                break;

            case '8':
                block_protection_6[0] = 0;
                block_protection_6[1] = 0;
                block_protection_6[2] = 0;
                block_protection_6[3] = 0;
                block_protection_6[4] = 0;
                block_protection_6[5] = 0;

                flashWREN();
                WriteBlockProtection();
                putst("Wrote block protection reg to zeros!\n");
                break; 

            default:
                putst("Unknown command, press h for help\r\n");
                break;
        }


        serIndex = 0;
    }

}