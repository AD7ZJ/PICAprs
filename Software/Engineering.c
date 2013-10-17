#include "main.h"
#include "serial.h"
#include "ax25.h"
#include "fifo.h"
#include <stdio.h>
#include <htc.h>

#define _XTAL_FREQ 32000000
void EngineeringConsole() {
    uint8_t buff;

    buff = FifoRead();

    if (buff) {
        switch(buff) {
            case 'h':
                putst("PICTrack Engineering Console\n");
                putst("1: Send APRS packet\n");
                putst("2: Read flash status reg\n");
                putst("3: Read flash JEDEC-ID\n");
                putst("4: Erase flash sector 0\n");
                putst("5: Program predtermined pattern to flash sector 0\n");
                putst("6: Read flash sector 0\n");
                putst("7: Read block protection register\n");
                putst("8: Write block protection register to all writeable\n");
                putst("9: Calibrate the mark tone\n");
                putst("a: Calibrate the space tone\n");
                break;

            case '1':
                RA2 = 1;
                tncPreparePacket("$GPGGA,,3436.89881,N,11227.02683,W,7,00,,,,,,,*42");
                // Send it
                tncSendPacket();

                __delay_ms(10);
                RA2 = 0;
                break;

            case '9':
                calTones(1);
                break;

            case 'a':
                calTones(0);
                break;

            default:
                putst("Unknown command, press h for help\r\n");
                break;
        }
    }

}