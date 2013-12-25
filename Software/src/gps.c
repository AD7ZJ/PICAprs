#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "gps.h"
#include "fifo.h"
#include "serial.h"

#define MAX_CMD_LEN     8               // maximum command length (NMEA address)
#define MAX_DATA_LEN    255             // maximum data length
#define MAX_CHAN        36              // maximum number of channels
#define WAYPOINT_ID_LEN 32              // waypoint max string len
#define MAXFIELD        25

static uint8_t calcChecksum;                        // Calculated NMEA sentence checksum
static uint8_t receivedChecksum;                    // Received NMEA sentence checksum (if exists)
static uint16_t index;                              // Index used for command and data
static uint8_t commandBuffer[MAX_CMD_LEN];          // NMEA command
static uint8_t dataBuffer[MAX_DATA_LEN];            // NMEA data
GPS_PARSE_STATE_MACHINE gpsParseState;
NMEA_PACKET_TYPE gpsPacketType;

// GPS data structure
GPSData data;
/// Flag that is set when a data set has been parsed.
static bool_t dataReadyFlag;

void ProcessCommand(uint8_t *pCommand, uint8_t *pData);
bool_t GetField(uint8_t *pData, uint8_t *pField, int8_t nFieldNum, int8_t nMaxFieldLen);
void ProcessGPGGA(uint8_t *pData);
void ProcessGPRMC(uint8_t *pData);

GPSData * GpsGetData() {
    return &data;
}

/**
 *   Determine if new GPS message is ready to process.  This function is a one shot and
 *   typically returns true once a second for each GPS position fix.
 *
 *   @return true if new message available; otherwise false
 */
bool_t GpsIsDataReady() {
    if (dataReadyFlag) {
        dataReadyFlag = FALSE;
        return TRUE;
    }

    return FALSE;
}

/**
 *   Read the serial FIFO and process complete GPS messages.
 */
void GpsUpdate() {
    uint8_t value;

    while (FifoHasData()) {
        value = FifoRead();

        // This state machine handles each character as it is read from the GPS serial port.

        //this->gpsBuffer[this->gpsIndex++] = value;
        switch (gpsParseState) {
            ///////////////////////////////////////////////////////////////////////
            // Search for start of message '$'
            case STARTOFMESSAGE:
                if (value == '$') {
                    calcChecksum = 0; // reset checksum
                    index = 0; // reset index
                    gpsParseState = COMMAND;
                }
                break;

                ///////////////////////////////////////////////////////////////////////
                // Retrieve command (NMEA Address)
            case COMMAND:
                if (value != ',' && value != '*') {
                    commandBuffer[index++] = value;
                    calcChecksum ^= value;

                    // Check for command overflow
                    if (index >= MAX_CMD_LEN)
                        gpsParseState = STARTOFMESSAGE;
                } else {
                    commandBuffer[index] = '\0'; // terminate command
                    calcChecksum ^= value;
                    index = 0;
                    gpsParseState = DATA; // goto get data state
                }
                break;

                // Store data and check for end of sentence or checksum flag
            case DATA:
                if (value == '*') { // checksum flag?
                    dataBuffer[index] = '\0';
                    gpsParseState = CHECKSUM_1;
                } else {
                    // Check for end of sentence with no checksum
                    if (value == '\r') {
                        dataBuffer[index] = '\0';
                        ProcessCommand(commandBuffer, dataBuffer);
                        gpsParseState = STARTOFMESSAGE;
                        return;
                    }

                    //
                    // Store data and calculate checksum
                    //
                    calcChecksum ^= value;
                    dataBuffer[index] = value;
                    if (++index >= MAX_DATA_LEN) // Check for buffer overflow
                        gpsParseState = STARTOFMESSAGE;
                }
                break;

            case CHECKSUM_1:
                if ((value - '0') <= 9)
                    receivedChecksum = (value - '0') << 4;
                else
                    receivedChecksum = (value - 'A' + 10) << 4;

                gpsParseState = CHECKSUM_2;
                break;

            case CHECKSUM_2:
                if ((value - '0') <= 9)
                    receivedChecksum |= (value - '0');
                else
                    receivedChecksum |= (value - 'A' + 10);

                //FIXME: re-add verifying the checksum after testing is finished
                //if (calcChecksum == receivedChecksum)
                    ProcessCommand(commandBuffer, dataBuffer);

                gpsParseState = STARTOFMESSAGE;
                printf("OK secs: %d\r\n", data.seconds);
                break;

                ///////////////////////////////////////////////////////////////////////
            default:
                gpsParseState = STARTOFMESSAGE;
                break;
        }
    }
}

/**
 * Process NMEA sentence - switch on the NMEA command and call the
 * appropriate processor
 */
void ProcessCommand(uint8_t *pCommand, uint8_t *pData) {
    /*
     * GPGGA
     */
    if (!strcmp((char *) pCommand, "GPGGA"))
        ProcessGPGGA(pData);

    /*
     * GPRMC
     */
    else if (!strcmp((char *) pCommand, "GPRMC"))
        ProcessGPRMC(pData);

    //m_dwCommandCount++;
    //return TRUE;
}

/**
 * This function will get the specified field in a NMEA string.
 *
 * @param pData Pointer to NMEA string
 * @param pField Pointer to returned field
 * @param nfieldNum Field offset to get
 * @param nMaxFieldLen Maximum number of bytes pField can handle
 */
bool_t GetField(uint8_t *pData, uint8_t *pField, int8_t nFieldNum, int8_t nMaxFieldLen) {
    /*
     * Validate params
     */
    if (pData == NULL || pField == NULL || nMaxFieldLen <= 0) {
        return FALSE;
    }

    /*
     * Go to the beginning of the selected field
     */
    int i = 0;
    int nField = 0;
    while (nField != nFieldNum && pData[i]) {
        if (pData[i] == ',') {
            nField++;
        }

        i++;

        if (pData[i] == NULL) {
            pField[0] = '\0';
            return FALSE;
        }
    }

    if (pData[i] == ',' || pData[i] == '*') {
        pField[0] = '\0';
        return FALSE;
    }

    /*
     * copy field from pData to Field
     */
    int i2 = 0;
    while (pData[i] != ',' && pData[i] != '*' && pData[i]) {
        pField[i2] = pData[i];
        i2++;
        i++;

        /*
         * check if field is too big to fit on passed parameter. If it is,
         * crop returned field to its max length.
         */
        if (i2 >= nMaxFieldLen) {
            i2 = nMaxFieldLen - 1;
            break;
        }
    }
    pField[i2] = '\0';

    return TRUE;
}

void ProcessGPGGA(uint8_t *pData) {
    uint8_t pField[MAXFIELD];
    char pBuff[10];

    // Time (currently not used)
    /*if (GetField(pData, pField, 0, MAXFIELD)) {
        // Hour
        pBuff[0] = pField[0];
        pBuff[1] = pField[1];
        pBuff[2] = '\0';
        data.hours = atoi(pBuff);

        // minute
        pBuff[0] = pField[2];
        pBuff[1] = pField[3];
        pBuff[2] = '\0';
        data.minutes = atoi(pBuff);

        // Second
        pBuff[0] = pField[4];
        pBuff[1] = pField[5];
        pBuff[2] = '\0';
        data.seconds = atoi(pBuff);
    } */

    // Latitude (currently not used)
    /*if (GetField(pData, pField, 1, MAXFIELD)) {
        data.latitude = lroundf(10000000 * atof((char *) pField + 2) / 60.0);
        pField[2] = '\0';
        data.latitude += lroundf(10000000 * atof((char *) pField));

    }
    if (GetField(pData, pField, 2, MAXFIELD)) {
        if (pField[0] == 'S')
            data.latitude = -data.latitude;
    }

    // Longitude (currently not used)
    if (GetField(pData, pField, 3, MAXFIELD)) {
        data.longitude = lroundf(10000000 * atof((char *) pField + 3) / 60.0);
        pField[3] = '\0';
        data.longitude += lroundf(10000000 * atof((char *) pField));
    }
    if (GetField(pData, pField, 4, MAXFIELD)) {
        if (pField[0] == 'W')
            data.longitude = -data.longitude;
    } */

    // Satellites in use
    if (GetField(pData, pField, 6, MAXFIELD)) {
        pBuff[0] = pField[0];
        pBuff[1] = pField[1];
        pBuff[2] = '\0';
        data.trackedSats = atoi(pBuff);
    }

    // HDOP
    if (GetField(pData, pField, 7, MAXFIELD)) {
        data.dop = (uint16_t)round(atof((char *) pField) * 10); // FIXME: is this the right units?
    }

    // Altitude
    if (GetField(pData, pField, 8, MAXFIELD)) {
        data.altitude = (int32_t)round(atof((char *) pField) * 100);
    }

    // Set the data-ready flag.
    dataReadyFlag = TRUE;
}

void ProcessGPRMC(uint8_t *pData)
{
    char pBuff[10];
    uint8_t pField[MAXFIELD];

    // Time
    if(GetField(pData, pField, 0, MAXFIELD))
    {
        // Hour
        pBuff[0] = pField[0];
        pBuff[1] = pField[1];
        pBuff[2] = '\0';
        data.hours = atoi(pBuff);

        // minute
        pBuff[0] = pField[2];
        pBuff[1] = pField[3];
        pBuff[2] = '\0';
        data.minutes = atoi(pBuff);

        // Second
        pBuff[0] = pField[4];
        pBuff[1] = pField[5];
        pBuff[2] = '\0';
        data.seconds = atoi(pBuff);
    }

    //
    // Data valid
    //
    if(GetField(pData, pField, 1, MAXFIELD)) {
        if(pField[0] == 'A') {
            if(data.altitude > 0)
                data.fixType = Fix3D;
            else
                data.fixType = Fix2D;
        }
        else
            data.fixType = NoFix;
    }

    //
    // latitude
    //
    if(GetField(pData, pField, 2, MAXFIELD))
    {
        data.latitude = (int32_t)round(10000000 * atof((char *) pField + 2) / 60.0);
        pField[2] = '\0';
        data.latitude += (int32_t)round(10000000 * atof((char *) pField));

    }
    if(GetField(pData, pField, 3, MAXFIELD))
    {
        if(pField[0] == 'S')
        {
            data.latitude = -data.latitude;
        }
    }

    //
    // Longitude
    //
    if(GetField(pData, pField, 4, MAXFIELD))
    {
        data.longitude = (int32_t)round(10000000 * atof((char *)pField+3) / 60.0);
        pField[3] = '\0';
        data.longitude += (int32_t)round(10000000 * atof((char *)pField));
    }
    if(GetField(pData, pField, 5, MAXFIELD))
    {
        if(pField[0] == 'W')
        {
            data.longitude = -data.longitude;
        }
    }

    //
    // Ground speed
    //
    if(GetField(pData, pField, 6, MAXFIELD))
    {   // convert to cm/s
        data.speed = (int16_t)round(51.4444 * atof((char *)pField));
    }
    else
    {
        data.speed = 0;
    }

    //
    // course over ground, degrees true converted 0.01 degree
    //
    if(GetField(pData, pField, 7, MAXFIELD))
    {
        data.heading = (int16_t)round(100 * atof((char *)pField));
    }
    else
    {
        data.heading = 0;
    }

    //
    // Date
    //
    if(GetField(pData, pField, 8, MAXFIELD))
    {
        // Day
        pBuff[0] = pField[0];
        pBuff[1] = pField[1];
        pBuff[2] = '\0';
        data.day = atoi(pBuff);

        // Month
        pBuff[0] = pField[2];
        pBuff[1] = pField[3];
        pBuff[2] = '\0';
        data.month = atoi(pBuff);

        // Year (Only two digits. I wonder why?)
        pBuff[0] = pField[4];
        pBuff[1] = pField[5];
        pBuff[2] = '\0';
        data.year = atoi(pBuff);
        data.year += 2000;             // make 4 digit date
    }
}