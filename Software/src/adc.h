/*
 * Analog conversion stuff for 16C71 - see adc.c for more info
 */

/*
 * Read the adc on the specified channel - result is in ADRES
 */
void adc_read(unsigned char channel);
