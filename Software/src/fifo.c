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
 * Filename:     FIFO.cpp                                                  *
 *                                                                         *
 ***************************************************************************/

#include "fifo.h"

/// Size of serial port FIFO in bytes.  It must be a power of 2, i.e. 2, 4, 8, 16, etc.
#define FIFOBUFFERSIZE 256

/// Mask to wrap around at end of circular buffer.  (SERIAL_BUFFER_SIZE - 1)
static const uint32_t FIFOBufferMask = 0x00ff;

/// Index to the next free location in the buffer.
uint32_t head;

/// Index to the next oldest data in the buffer.
uint32_t tail;

/// Circular buffer (FIFO) to hold serial data.
uint8_t buffer[FIFOBUFFERSIZE];

/**
 * Clear the FIFO contents.
 */
void FifoClear()
{
    head = 0;
    tail = 0;
}

/**
 * Determine if the FIFO contains data.
 *
 * @return true if data present; otherwise false
 */
bool_t FifoHasData()
{
    if (head == tail)
        return FALSE;

    return TRUE;
}

/**
 * Get the oldest character from the FIFO.
 *
 * @return oldest character; 0 if FIFO is empty
 */
uint8_t FifoRead()
{
    uint8_t value;

    // Make sure we have something to return.
    if (head == tail)
        return 0;

    // Save the value.
    value = buffer[tail];

    // Update the pointer.
    tail = (tail + 1) & FIFOBufferMask;

    return value;
}

/**
 * Store a new character in the FIFO.
 *
 * @param value character to add to FIFO.
 */
void FifoWrite(uint8_t value)
{
    // Save the value in the FIFO.
    buffer[head] = value;

    // Move the pointer to the next open space.
    head = (head + 1) & FIFOBufferMask;
}