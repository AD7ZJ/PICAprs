#include <htc.h>
/*
 * Read the ADC on most of the 8bit PICs.
 */

void AdcRead(unsigned char channel) {
    // Right justify, channel select, ADON
    ADCON0 = ((channel & 0b00001111) << 2) | 0b10000001;
    // Aquisition time of 20uS
    _delay(200);
    ADCON0 |= 0b00000010;  // Start Conversion
    while(ADCON0 & 0b00000010)
        continue;    // wait for conversion complete
}
