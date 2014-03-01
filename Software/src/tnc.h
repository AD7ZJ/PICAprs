/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *  You should have received a copy of the GNU General Public License      *
 *  along with this program; if not, write to the Free Software            *
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111 USA    *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 *               (c) Copyright, 1997-2013, AD7ZJ                           *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 * Filename:     tnc.h                                                     *
 *                                                                         *
 ***************************************************************************/
#ifndef AX25_H
#define    AX25_H

/**
 * @defgroup ax25packet AX.25 Packet Creation
 *
 * @{
 */

void TncConfigDefault(); // Configure the TNC
void TncPreparePacket(uint8_t * message, uint8_t * destaddr); // Prepare a packet to send
void TncSendPacket(void); // Send a packet via the 4 bit DAC
void RadioRX(void);
void RadioTX(void);
void TncCalTones(unsigned bitValue); // generate a mark or space tone to allow calibration
uint16_t CRC16(uint8_t *buffer, uint16_t length); // Generate a 16 bit CRC

/*
 * Declare global vars and data structures
 */
/// Configuration Structure

typedef struct {
    /// Sets how many sync flags we'll send at the beginning of the packet
    uint8_t txDelay;
    /// The Beacon's Callsign
    uint8_t callSign[7];
    /// Destination Callsign
    uint8_t destCallSign[7];
    /// Relay Callsign1
    uint8_t relayCallSign1[7];
    /// Relay Callsign2
    uint8_t relayCallSign2[7];
    /// SSID of our callsign
    uint8_t callSignSSID;
    /// SSID to be used for the first relay
    uint8_t relayCallSignSSID1;
    /// SSID to be used with the second relay
    uint8_t relayCallSignSSID2;
    /// Keeps track of the total flight time
    uint16_t flightTime;
} CONFIG_STRUCT;


/// The maximum number of characters we can send through the TNC
#define TNC_MAX_TX 128
/// The TNC is currently not sending
#define TNC_RX_FLAG 0
/// We're currently preparing a packet to send
#define TNC_TX_PREPARE 3
/// We're sending sync flags to start the packet(0x7E)
#define TNC_TX_SYNC 4
/// We're sending the packet data
#define TNC_TX_DATA 5
/// We're closing the packet
#define TNC_TX_END 6

/// 1200 Hz tone for a mark.  Calculated by (Fosc/4/2)/(1200*16)
#define     MARK    207
/// 2200 Hz tone for a space.  Calculated by (Fosc/4/2)/(2200*16)
#define     SPACE   113
/// 1200 Baud.  In units of timer 2 (no pre, post scalar 1:2), so calculated by (Fosc/4/2)/(1200)
#define     BAUD    3333

/** @} */

#endif /* AX25_H */