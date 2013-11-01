#include "gps.h"
#include "math.h"
#include "stdlib.h"
#include "mic-e.h"

/// Destination address as part of the AX.25 message.
char destAddress[7];

/// Informational text field as part of the AX.25 message.
char information[20];

void MicEEncode (GPSData *gps)
{
    int32_t value;

    // NOTE: The Message A/B/C bits are hard coded as 110 for Enroute.

    // Convert to units of decimal degrees.
    value = labs(gps->latitude);
    value /= 10000000;

    // Encode the 10s and 1s of the decimal degrees.
    destAddress[0] = 'P' + (value / 10);
    destAddress[1] = 'P' + (value % 10);

    // Convert to units of 0.0001 minutes.
    value = 6 * (labs(gps->latitude) % 10000000);

    // Encode the decimal minutes.
    destAddress[2] = '0' + (value / 10000000) % 10;
    destAddress[3] = '0' + (value / 1000000) % 10;
    destAddress[4] = '0' + (value / 100000) % 10;
    destAddress[5] = '0' + (value / 10000) % 10;

    // Adjust for the N/S ordinal.
    if (gps->latitude > 0)
        destAddress[3] += 'P' - '0';

    // Adjust for the E/W ordinal.
    if (gps->longitude < 0)
        destAddress[5] += 'P' - '0';

    // NULL terminate the string.
    destAddress[6] = 0;


    // Set the Data Type ID.
    information[0] = '`';

    // Convert to units of decimal degrees.
    value = labs(gps->longitude) / 10000000;

    // Adjust the destination for the +100 longitude.
    if (value <= 9 || value >= 100)
        destAddress[4] += 'P' - '0';

    // Encode the longitude in degrees.
    if (value <= 9)
        information[1] = value + 118;
    else if (value <= 99)
        information[1] = value + 28;
    else if (value <= 109)
        information[1] = value + 8;
    else
        information[1] = value - 72;

    // Convert to units of 1 minute.
    value = (6 * (labs(gps->longitude) % 10000000)) / 1000000;

    // Encode the longitude in minutes.
    if (value <= 9)
        information[2] = value + 88;
    else
        information[2] = value + 28;

    // Convert to units of 0.01 minutes.
    value = ((6 * (labs(gps->longitude) % 10000000)) / 10000) % 100;

    // Encode the longitude in decimal minutes.
    information[3] = value + 28;

    // Convert to units of 1 knot.
    value = gps->speed / 10;

    // Encode the speed in knots and heading in degrees.
    information[4] = 28 + (value / 10);
    information[5] = 28 + ((value % 10) * 10);
    information[5] += (gps->heading / 10000);
    information[6] = 28 + (gps->heading / 100) % 100;

    // APRS symbol setting for balloon.
    information[7] = 'O';
    information[8] = '/';

    // Encode the altitude in meters above 10KM datum.
    value = (gps->altitude / 100) + 10000;

    information[9] = 33 + (value / 8281);
    information[10] = 33 + ((value / 91) % 91);
    information[11] = 33 + (value % 91);
    information[12] = '}';

    // NULL terminate the string.
    information[13] = 0;
}

/**
 * Get the destination address encoded as a 6-character string.
 *
 * @return NULL terminated string
 */
char * MicEGetDestAddress()
{
    return destAddress;
}

/**
 * Get the information field text that contains the MIC-E encoded text.
 *
 * @return NULL terminate string
 */
char * MicEGetInfoField()
{
    return information;
}