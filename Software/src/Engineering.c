#include "main.h"
#include "serial.h"
#include "tnc.h"
#include "fifo.h"
#include <stdio.h>
#include <htc.h>

#define _XTAL_FREQ 32000000

/**
 *
 * @defgroup library Generic Library Functions
 *
 * @{
 */

/**
 * Called to process commands from the serial port when in 'console' mode
 */
void EngineeringConsole() {
    uint8_t buff;

    buff = FifoRead();

    if (buff) {
        switch(buff) {
            case 'h':
                SerialPutst("PICTrack Engineering Console\n");
                SerialPutst("1: Send APRS packet\n");
                SerialPutst("2: Calibrate the mark tone\n");
                SerialPutst("3: Calibrate the space tone\n");
                break;

            case '1':
                RadioTX();
                TncPreparePacket("$GPGGA,155146,3515.5466,N,11211.0917,W,1,10,0.9,2059.0,M,,M,,*74", "APRS  ");
                // Send it
                TncSendPacket();

                __delay_ms(10);
                RadioRX();
                break;

            case '2':
                TncCalTones(1);
                break;

            case '3':
                TncCalTones(0);
                break;

            default:
                SerialPutst("Unknown command, press h for help\r\n");
                break;
        }
    }
}

/** @} */