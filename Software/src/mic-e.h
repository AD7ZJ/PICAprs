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
 *               (c) Copyright, 1997-2012, ANSR                            *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 * Filename:     MIC-E.h                                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MIC_ENCODER_H
#define MIC_ENCODER_H

#include "main.h"




/**
 *  @defgroup ax25packet AX.25 Packet Creation
 *
 *  @{
 */

/**
 *  Generate a MIC-E type message
 */
void MicEEncode (GPSData *gps);

/**
 * Get the AX.25 info field from the MIC-E encoded data
 * @return pointer to the array
 */
char * MicEGetInfoField();

/**
 * Get the AX.25 dest address from the MIC-E encoded data
 * @return
 */
char * MicEGetDestAddress();


/** @} */

#endif  // #ifndef MIC-MIC_ENCODER_H