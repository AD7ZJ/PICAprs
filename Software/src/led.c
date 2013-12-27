#include <htc.h>
#include "led.h"

/**
 *
 * @defgroup library Generic Library Functions
 *
 * @{
 */

/**
 * Turns the specified LED on or off
 *
 * @param ledId LED to turn on or off (1 - 3)
 * @param state TRUE turns the LED on, FALSE turns it off
 */
void SetLED(uint8_t ledId, bool_t state) {
    if (ledId <= 3 && ledId >= 1) {
        if (state) {
            PORTB &= ~(1u << (ledId + 2));
        }
        else {
            PORTB |= 1u << (ledId + 2);
        }
    }
}

/**
 * Toggles the specified LED
 *
 * @param ledId LED to toggle (1 - 3)
 */
void ToggleLED(uint8_t ledId) {
    if (ledId <= 3 && ledId >= 1) {
        PORTB ^= 1u << (ledId + 2);
    }
}

/** @} */
