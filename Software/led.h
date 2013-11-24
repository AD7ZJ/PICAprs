/*
 * LED driver module
 */

#include "main.h"


/**
 * Turns the specified LED on or off
 *
 * @param ledId LED to turn on or off (1 - 3)
 * @param state TRUE turns the LED on, FALSE turns it off
 */
void SetLED(uint8_t ledId, bool_t state);

/**
 * Toggles the specified LED
 *
 * @param ledId LED to toggle (1 - 3)
 */
void ToggleLED(uint8_t ledId);
