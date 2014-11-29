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
 *               (c) Copyright, 2013 AD7ZJ                                 *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 * Filename:     gps.h                                                     *
 *                                                                         *
 ***************************************************************************/

#ifndef GPS_H
#define GPS_H

#include "main.h"

/**
 *  @defgroup GPS GPS Parsing
 *
 *  @{
 */


/// Enumeration that describes the fix type.
typedef enum
{
    /// No GPS fix.
    NoFix = 0x00,

    /// 2D only (no altitude) fix.
    Fix2D = 0x02,

    /// 3D (with altitude) fix.
    Fix3D = 0x03
} FixType;

/// Structure that contains all the GPS data
typedef struct GPSData {
    /// UTC time in hours in the range 0 to 23.
    uint8_t hours;

    /// UTC time in minutes in the range 0 to 59.
    uint8_t minutes;

    /// UTC time in seconds in the range 0 to 59.
    uint16_t seconds;

    /// UTC date in month in the range 1 to 12.
    uint8_t month;

    /// UTC date in day of month in the range 1 to 31.
    uint8_t day;

    /// UTC date in years.
    uint16_t year;

    /// Latitude in degrees * 10 ^ 7, where + is North, - is South.
    int32_t latitude;

    /// Longitude in degrees * 10 ^ 7, where + is East, - is West.
    int32_t longitude;

    /// Altitude from MSL in cm.
    int32_t altitude;

    /// Speed over the ground in knots * 10.
    uint16_t speed;

    /// Heading clockwise from north in units of 0.01 degrees.
    uint16_t heading;

    /// DOP (Dilution of Precision)
    uint16_t dop;

    /// Number of tracked satellites used in the fix position.
    uint16_t trackedSats;

    /// Number of visible satellites.
    uint16_t visibleSats;

    /// GPS fix type, None/2D/3D.
    FixType fixType;

    /// Time in seconds until the first 2D or 3D fix with a 1-PPS aligned Time of Day.
    uint8_t timeToFirstFix;
} GPSData;

/// enumeration of the NMEA parser's states
typedef enum {
    STARTOFMESSAGE,
    COMMAND,
    DATA,
    CHECKSUM_1,
    CHECKSUM_2
} GPS_PARSE_STATE_MACHINE;


GPSData * GpsGetData();
bool_t GpsIsDataReady();
void GpsUpdate();


/** @} */

#endif  // #ifndef GPS_H