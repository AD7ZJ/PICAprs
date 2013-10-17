//*****************
//function prototypes
#ifndef SERIAL_H
#define    SERIAL_H

#include "main.h"

uint8_t UsartTimeout(void);
uint8_t getch(void);
void SerialInit(void);
void putch(unsigned char c);
void putst(register const char * str);
void putchdec(unsigned char c);
void putchhex(unsigned char c);
void putinthex(unsigned int c);

#define putlf putst("\n") //put line feed

#endif /* SERIAL_H */

