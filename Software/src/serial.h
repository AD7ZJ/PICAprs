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
 *               (c) Copyright, 2013, AD7ZJ                                *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 * Filename:     serial.h                                                     *
 *                                                                         *
 ***************************************************************************/

#ifndef SERIAL_H
#define SERIAL_H

#include "main.h"

/**
 *
 * @defgroup library Generic Library Functions
 *
 * @{
 */

uint8_t getch(void);
void SerialInit(void);
void putch(unsigned char c);
void SerialPutst(register const char * str);
void SerialPutCharDec(unsigned char c);
void SerialPutCharHex(unsigned char c);
void SerialPutIntHex(unsigned int c);
void SerialClearErrors(void);

/** @} */

#endif /* SERIAL_H */
