/*
 * Read the ADC on a 16F690.
 */

void adc_read(unsigned char channel) {
	// Right justify, channel select, ADON
	ADCON0 = ((channel & 0b00001111) << 2) | 0b10000001;
	// Aquisition time of 20uS
	_delay(200);
	ADCON0 |= 0b00000010;  // Start Conversion
	while(ADCON0 & 0b00000010)
		continue;	// wait for conversion complete
}