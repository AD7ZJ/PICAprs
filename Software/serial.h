//*****************
//function prototypes
#ifndef SERIAL_H
#define	SERIAL_H

void serial_setup(void);
void putch(unsigned char c);
unsigned char getch(void);
void putst(register const char * str);
unsigned char usart_timeout(void);
void putchdec(unsigned char c);
void putchhex(unsigned char c);
void putinthex(unsigned int c);

#define putlf putst("\n") //put line feed

#endif /* SERIAL_H */
