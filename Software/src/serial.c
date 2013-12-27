#include <htc.h>
#include "serial.h"

/**
 *
 * @defgroup library Generic Library Functions
 *
 * @{
 */

/* relates crystal freq to baud rate - see above and PIC16F87x data sheet under 'USART async. modes'

BRGH=1, Fosc=3.6864MHz        BRGH=1, Fosc=4MHz      BRGH=1, Fosc=8MHz    BRGH=1, Fosc=16MHz
----------------------      -----------------      -----------------    ------------------
Baud        SPBRG             Baud        SPBRG          Baud        SPBRG         Baud        SPBRG
1200        191               1200        207.3          1200        415.7         9600        103
2400        95                2400        103.2          2400        207.3         19200       51
4800        47                4800        51.1           4800        103.2         38400       25
9600        23                9600        25.0           9600        51.1          57600       16
19200       11                19200       12.0           19200       25.0          115200      8
38400       5                 38400       5.5            38400       12.0
57600       3                 57600       3.3            57600       7.7
115200      1                 115200      1.2            115200      3.3

 */

/// CPU clock speed in Hz
#define PIC_CLK 32000000

/// desired serial baud rate
#define BAUD 9600

/// calculate the baud rate generator divider
#define DIVIDER ((PIC_CLK/(16UL * BAUD) -1))

/// defines whether to use high speed baud rates or not (setting 0 changes the divider calc)
#define HIGH_SPEED 1

static unsigned char dummy;

/**
 * Initializes the onboard serial port hardware
 */
void SerialInit(void) {
    SPBRG = DIVIDER;    //using the baudrate generator in 8-bit mode
    BRGH  = HIGH_SPEED; //data rate for sending
    SYNC  = 0; //asynchronous
    SPEN  = 1; //enable serial port pins
    CREN  = 1; //enable reception
    TXIE  = 0; //disable tx interrupts
    RCIE  = 1; //enable rx interrupts
    TX9   = 0;  //8-bit transmission
    RX9   = 0;  //8-bit reception
    TXEN  = 0; //reset transmitter
    TXEN  = 1; //enable the transmitter
}

/// macro for clearing any UART errors
#define clear_usart_errors_inline    \
if (OERR)                \
{                        \
    TXEN=0;              \
    TXEN=1;              \
    CREN=0;              \
    CREN=1;              \
}                        \
if (FERR)                \
{                        \
    dummy=RCREG;         \
    TXEN=0;              \
    TXEN=1;              \
}

/**
 * Write a character to the serial port (necessary to use embedded printf calls).
 *
 * @param c character to write
 */
void putch(unsigned char c) {
    while (!TXIF); //set when register is empty
    {
        clear_usart_errors_inline;
        CLRWDT();
    }
    TXREG = c;
    _delay(240);
}

/**
 * Get a character from the serial port without timeout (neccessary to use
 * embedded scanf calls).
 *
 * @return character received from the UART
 */
unsigned char getch(void) {
    while (!RCIF) {
        CLRWDT();
        clear_usart_errors_inline;
    }
    return RCREG;
}

/**
 * Public function for clearing any framing or overrun errors from the serial
 * port hardware.
 */
void SerialClearErrors(void) {
    clear_usart_errors_inline;
}

/*
 * 
 */

/**
 * Write a character to the serial port in hex.
 *
 * @param c 8 bit value to output in hex
 */
void SerialPutCharHex(unsigned char c) {
    unsigned char temp;

    // transmits in hex
    temp = c;

    c = (c >> 4);
    if (c < 10) c += 48;
    else c += 55;
    putch(c);

    c = temp;

    c = (c & 0x0F);
    if (c < 10) c += 48;
    else c += 55;
    putch(c);
}

/**
 * Write an integer (16 bit) out to the serial port in hex.
 *
 * @param c Integer value to be written
 */
void SerialPutIntHex(unsigned int c) {
#define ramuint(x)              (*((unsigned int *) (x)))
#define ramuint_hibyte(x)       (*(((unsigned char *)&x)+1))
#define ramuint_lobyte(x)       (*(((unsigned char *)&x)+0))
#define ramuchar(x)             (*((unsigned char *) (x)))

    SerialPutCharHex(ramuint_hibyte(c));
    SerialPutCharHex(ramuint_lobyte(c));

#undef ramuint(x)
#undef ramuint_hibyte(x)
#undef ramuint_lobyte(x)
#undef ramuchar(x)
}


/**
 * Write an 8 bit value to the serial port in decimal form
 *
 * @param c 8 bit value to be output.
 */
void SerialPutCharDec(unsigned char c) {
    unsigned char temp;

    temp = c;
    //hundreds
    if ((c / 100) > 0) putch((c / 100) + '0');
    c -= (c / 100)*100;

    //tens
    if (((temp / 10) > 0) || ((temp / 100) > 0)) putch((c / 10) + '0');
    c -= (c / 10)*10;

    //ones
    putch((c / 1) + '0');
}

/**
 * Write a string to the serial port.
 * @param str String to be written.  
 */
void SerialPutst(register const char *str) {
    while ((*str) != 0) {
        putch(*str);
        if (*str == 13) putch(10);
        if (*str == 10) putch(13);
        str++;
    }
}

/** @} */
