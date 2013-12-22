#include <htc.h>
#include "led.h"



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

void ToggleLED(uint8_t ledId) {
    if (ledId <= 3 && ledId >= 1) {
        PORTB ^= 1u << (ledId + 2);
    }
}