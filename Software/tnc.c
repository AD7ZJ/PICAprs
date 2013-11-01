#include <htc.h>
#include <string.h>
#include "main.h"
#include "tnc.h"
#include "fifo.h"

/**
 * @defgroup AX25 Packet Creation
 *
 * Contains functions to control the 4-bit resistor DAC, create packets, and setup the TNC
 * @{
 */

/**
 * Table of values to generate a sin wave.  Each value represents the value
 * to jump to after an equal increment in time.  Note this is highly dependant
 * on the particular HW configuration :-)
 */
static unsigned char sinDAC[] = {
    0b1110, 0b0101, 0b1011, 0b0111, 0b1111, 0b0111,
    0b1011, 0b0101, 0b0001, 0b1010, 0b0100,
    0b1000, 0b0000, 0b1000, 0b0100, 0b1010
};

/*
 * Define global variables
 */
uint16_t tncTimerCompare, tncIndex, tncLength;
uint8_t tncBitCount, tncBitTime, tncShift, tncRx, tncLastBit, tncMode, tncTransmit;
uint8_t tncBitStuff, tncBuffer[TNC_MAX_TX], tncRemoteTick, index;
volatile static unsigned int timeElapsed = 0;
CONFIG_STRUCT config;

/**
 *    Configure the TNC's callsign, SSID, and digipeat path.
 */
void TncConfigDefault() {
    // Station ID, relay path, and destination call sign and SSID.
    strcpy(config.callSign, "AD7ZJ ");
    config.callSignSSID = 11;
    strcpy(config.destCallSign, "APRS  ");
    strcpy(config.relayCallSign1, "WIDE2 ");
    strcpy(config.relayCallSign2, "      ");
    config.relayCallSignSSID1 = 2;
    config.relayCallSignSSID2 = 0;

    // Number of TNC flag bytes sent before data stream starts.  (350mS) 1 byte = 6.6mS
    config.txDelay = 53;

    // Flight operation time.
    config.flightTime = 0;
}

/**
 *    Calculate the CRC-16 CCITT of <b>buffer</b> that is <b>length</b> bytes long.
 *
 *    @param buffer Pointer to data buffer.
 *    @param length number of bytes in data buffer
 *
 *    @return CRC-16 of buffer[0 .. length]
 */
uint16_t CRC16(uint8_t *buffer, uint16_t length) {
    uint16_t i, dbit, crc, value;

    crc = 0xffff;

    for (i = 0; i < length; ++i) {
        value = buffer[i];

        for (dbit = 0; dbit < 8; ++dbit) {
            crc ^= (value & 0x01);
            crc = (crc & 0x01) ? (crc >> 1) ^ 0x8408 : (crc >> 1);
            value = value >> 1;
        }
    }

    return crc ^ 0xffff;
}

/**
 *   Prepare an AX.25 packet for transmission.  This function takes a char buffer as
 *   input and operates on tncBuffer.
 *
 *   @param message pointer to NULL terminate message string
 *   @param destaddr pointer to the destination address
 */

void TncPreparePacket(uint8_t * message, uint8_t * destaddr) {
    uint16_t i, crc;
    uint8_t *outBuffer;

    // If we are currently sending, then ignore this packet.
    if (tncMode == TNC_TX_PREPARE || tncMode == TNC_TX_SYNC || tncMode == TNC_TX_DATA || tncMode == TNC_TX_END)
        return;

    tncMode = TNC_TX_PREPARE;

    // Set a pointer to our output buffer.
    outBuffer = tncBuffer;

    // Includes source (7), dest (7), control field (1), protocol ID (1), and end of message (1).
    tncLength = 17;

    // Set the destination address.  AX.25 requires all callsigns to be ASCI shifted one left.  This
    // essentially just drops the parity bit
    for (i = 0; i < 6; ++i)
        *outBuffer++ = destaddr[i] << 1;

    // Set destination to SSID-0
    *outBuffer++ = 0x60;

    // Set the source address, again shifted one left
    for (i = 0; i < 6; ++i)
        *outBuffer++ = config.callSign[i] << 1;

    // Set the SSID.
    *outBuffer++ = 0x60 | (config.callSignSSID << 1);

    // Add relay path 1.
    if (*config.relayCallSign1 != 0) {
        for (i = 0; i < 6; ++i)
            *outBuffer++ = config.relayCallSign1[i] << 1;
        *outBuffer++ = 0x60 | (config.relayCallSignSSID1 << 1);
        tncLength += 7;
    }

    // Add relay path 2.
    if (*config.relayCallSign2 != 0) {
        for (i = 0; i < 6; ++i)
            *outBuffer++ = config.relayCallSign2[i] << 1;
        *outBuffer++ = 0x60 | (config.relayCallSignSSID2 << 1);
        tncLength += 7;
    }

    // Set bit-0 of the last SSID.
    *(outBuffer - 1) |= 0x01;

    // Set the control field (UI) and protocol ID.
    *outBuffer++ = 0x03;
    *outBuffer++ = 0xf0;

    // Add the message to the buffer
    while (*message != 0) {
        *outBuffer++ = *message++;
        ++tncLength;
    }

    // Add the end of message character.
    *outBuffer++ = 0x0d;

    // Calculate and append the CRC.
    crc = CRC16(tncBuffer, tncLength);
    *outBuffer++ = crc & 0xff;
    *outBuffer = (crc >> 8) & 0xff;

    // Update the length to include the 2 CRC bytes.
    tncLength += 2;

    // Prepare to send the packet
    tncBitCount = 0;
    tncShift = 0x7e;
    tncLastBit = 0;
    tncIndex = 0;
    tncMode = TNC_TX_SYNC;
}

void TncCalTones(unsigned bitValue) {
    while (FifoRead() != 'q') {
        // Output the next step of the sin wave.  The rest of the code in this function determines the
        // frequency of this wave.
        PORTC = sinDAC[index];
        index++;
        index &= 0x0F;

        if (bitValue)
            PR2 = MARK;
        else
            PR2 = SPACE;

        while (!TMR2IF) {
            // wait for the timer to overflow.  This wait time determines the frequency of the sin
        }
        TMR2IF = 0;
    } // end while loop
}

/**
 * Send the prepared packet via the onboard 4-bit resistor DAC.  Depends on the following global vars
 * (local vars are not used to save on function overhead)
 * uint16_t tncIndex, tncLength
 * uint8_t tncBitCount, tncShift, tncLastBit, tncMode, tncBitStuff, index
 * uint8_t tncBuffer[TNC_MAX_TX]
 */
void TncSendPacket(void) {
    while (tncMode != TNC_RX_FLAG) {
        // Output the next step of the sin wave.  The rest of the code in this function determines the
        // frequency of this wave.
        PORTA = sinDAC[index];
        index++;
        //index &= 0x1F;
        index &= 0x0F;

        if (timeElapsed >= BAUD) {
            timeElapsed = timeElapsed - BAUD;

            switch (tncMode) {
                    // Send the flag 0x7E to begin the packet.  No bitstuffing here.  This lets the RX end sync up, and
                    // is the only time it'll see 5 1's in a row
                case TNC_TX_SYNC:
                    // The variable tncShift contains the lastest data byte.  Data is sent LSB first
                    // NRZI enocde the data stream.
                    if ((tncShift & 0x01) == 0x00) {
                        if (tncLastBit == 0) {
                            tncLastBit = 1;
                            PR2 = MARK;
                        } else {
                            tncLastBit = 0;
                            PR2 = SPACE;
                        }
                    }
                    // When the flag is done, determine if we need to send more or data.
                    if (++tncBitCount == 8) {
                        tncBitCount = 0;
                        tncShift = 0x7e;

                        // Once we transmit x mS of flags, send the data.
                        // txDelay bytes * 8 bits/byte * 833uS/bit = x mS
                        if (++tncIndex == config.txDelay) {
                            tncIndex = 0;
                            tncShift = tncBuffer[0];
                            tncBitStuff = 0;
                            tncMode = TNC_TX_DATA;
                        } // END if
                    } else
                        tncShift = tncShift >> 1;
                    break;

                case TNC_TX_DATA:
                    // Start sending the prepared message.  This includes src and dest callsigns
                    // Bitstuffing; If there's been 5 ones in a row, send a zero
                    if (tncBitStuff == 0x1f) {
                        if (tncLastBit == 0) {
                            tncLastBit = 1;
                            PR2 = MARK;
                        } else {
                            tncLastBit = 0;
                            PR2 = SPACE;
                        }
                        tncBitStuff = 0x00;
                        //timeElapsed = 0;
                    } else {
                        // The variable tncShift contains the lastest data byte.
                        // NRZI enocde the data stream.
                        if ((tncShift & 0x01) == 0x00) {
                            if (tncLastBit == 0) {
                                tncLastBit = 1;
                                PR2 = MARK;
                            } else {
                                tncLastBit = 0;
                                PR2 = SPACE;
                            }
                        }

                        // Save the data stream so we can determine if bit stuffing is
                        // required on the next bit time.
                        tncBitStuff = ((tncBitStuff << 1) | (tncShift & 0x01)) & 0x1f;

                        // If all the bits were shifted, get the next byte.
                        if (++tncBitCount == 8) {
                            tncBitCount = 0;

                            // If everything was sent, transmit closing flags.
                            if (++tncIndex == tncLength) {
                                tncShift = 0x7e;
                                tncIndex = 0;
                                tncMode = TNC_TX_END;
                            } else {
                                tncShift = tncBuffer[tncIndex];
                            }
                        } else {
                            tncShift = tncShift >> 1;
                        }
                    }
                    break;

                case TNC_TX_END:
                    // Transmit the closing flags.  No bitstuffing here either
                    // NRZI enocde the data stream.
                    if ((tncShift & 0x01) == 0x00) {
                        if (tncLastBit == 0) {
                            tncLastBit = 1;
                            PR2 = MARK;
                        } else {
                            tncLastBit = 0;
                            PR2 = SPACE;
                        }
                    }
                    // If all the bits were shifted, get the next one.
                    if (++tncBitCount == 8) {
                        tncBitCount = 0;
                        tncShift = 0x7e;

                        // Transmit two closing flags.
                        if (++tncIndex == 2) {
                            // Reset to the receive mode.
                            tncIndex = 0;
                            tncShift = 0;
                            tncMode = TNC_RX_FLAG;
                        }
                    } else
                        tncShift = tncShift >> 1;
                    break;
            } // end switch
        }

        while (!TMR2IF) {
            // wait for the timer to overflow.  This wait time determines the frequency of the sin
        }
        TMR2IF = 0;
        timeElapsed += PR2;
    } // end while loop
}

void RadioRX(void) {
    PORTC &= ~(1u << 1);
}

void RadioTX(void) {
    PORTC |= (1u << 1);
}

/** @} */