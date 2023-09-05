#include "stdio.h"
#include "arch/i686/io.h"
#include <stdarg.h>
#include <stdbool.h>

const unsigned SCREEN_WIDTH = 80;
const unsigned SCREEN_HEIGHT = 25;
uint8_t* ScreenBuffer = (uint8_t*)0xB8000;
int ScreenX = 0, ScreenY = 0;

void putChr(char c, int x, int y) {
    ScreenBuffer[2 * (y*SCREEN_WIDTH + x)] = c;
}

void putCol(uint8_t c, int x, int y) {
    ScreenBuffer[2 * (y*SCREEN_WIDTH + x) + 1] = c;
}

char getChar(int x, int y) {
    return ScreenBuffer[2 * (y*SCREEN_WIDTH + x)];
}

uint8_t getColor(int x, int y) {
    return ScreenBuffer[2 * (y*SCREEN_WIDTH + x) + 1];
}

void setCursor(int x, int y) {
    uint16_t pos = y*SCREEN_WIDTH+x;
    x86_outb(0x03D4, 0x0F);
    x86_outb(0x03D5, (uint8_t)(pos & 0xFF));
    x86_outb(0x03D4, 0x0E);
    x86_outb(0x03D5, (uint8_t)((pos >> 8) & 0xFF));
}

void clrScr() {
    for (int i = 0; i < SCREEN_HEIGHT * SCREEN_WIDTH; i++) {
        putChr('\0', i%SCREEN_WIDTH, i/SCREEN_WIDTH);
        putCol(0x7, i%SCREEN_WIDTH, i/SCREEN_WIDTH);
    }
    ScreenY = 0;
    ScreenX = 0;
    setCursor(ScreenX, ScreenY);
}

void scrollBack(int lines) {
    for (int y = lines; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            putChr(getChar(x, y), x, y - lines);
            putCol(getColor(x, y), x, y - lines);
        }
    }

    for (int y = SCREEN_HEIGHT - lines; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            putChr('\0', x, y);
            putCol(0x7, x, y);
        }
    }
    ScreenY -= lines;    
}

void putChar(char c) {
    // x86_outb(0xE9, c);
    switch (c) {
        case '\n':
            ScreenX = 0;
            ScreenY++;
            break;
        case '\r':
            ScreenX = 0;
            break;
        case '\t':
            for(int i = 0; i < 4 - (ScreenX%4); i++) putChar(' ');
            break;
        default:
            putChr(c, ScreenX, ScreenY);
            ScreenX++;
            break;
    }
    if(ScreenX >= SCREEN_WIDTH) {
        ScreenY++;
        ScreenX = 0;
    }
    if(ScreenY >= SCREEN_HEIGHT) scrollBack(1);
    setCursor(ScreenX, ScreenY);
}

void putString(const char* str) {
    while(*str) {
        putChar(*str);
        str++;
    }
}


const char cgHexChars[] = "0123456789abcdef";

void printun(unsigned long long num, int radix) {
    char buff[32];
    int pos = 0;

    do {
        unsigned long long rem = num % radix;
        num /= radix;
        buff[pos++] = cgHexChars[rem];
    } while (num > 0);
    
    while(--pos >= 0) putChar(buff[pos]);
}

void printn(long long number, int radix) {
    if(number < 0) {
        putChar('-');
        printun(-number, radix);
    } else printun(number, radix);
}

void printf(const char* str, ...) {
    
    va_list args;
    va_start(args, str);

    int state = 0;
    int argLength = 0;
    int radix = 10;
    bool sign = false;
    bool number = false;
    while (*str) {
        switch (state) {
        case 0:
            switch (*str) {
            case '%':
                state = 1;
                break;
            default:
                putChar(*str);
                break;
            }
            break;
        case 1:
            switch (*str) {
            case 'h':
                argLength = 2;
                state = 3;
                break;
            case 'l':
                argLength = 3;
                state = 2;
                break;
            default:
                goto specifierState;
            }
            break;
        case 2:
            if (*str == 'h') {
                argLength = 1;
                state = 4;
            }
            else goto specifierState;
            break;
        case 3:
            if (*str == 'l') {
                argLength = 4;
                state = 4;
            }
            else goto specifierState;
            break;
        case 4:
        specifierState:
            switch (*str) {
            case 'c':
                putChar((char)va_arg(args, int));
                break;
            case 's':
                putString(va_arg(args, const char*));
                break;
            case 'd':
            case 'i':
                radix = 10;
                sign = true;
                number = true;
                break;
            case 'u':
                radix = 10;
                sign = false;
                number = true;
                break;
            case 'X':
            case 'x':
            case 'p':
                radix = 16;
                sign = false;
                number = true;
                break;
            case 'o':
                radix = 8;
                sign = false;
                number = true;
                break;
            case 'b':
                radix = 2;
                sign = false;
                number = true;
                break;
            default:
                break;
            }
            if(number) {
                if(sign){
                    switch (argLength) {
                        case 0:
                        case 1:
                        case 2:
                            printn(va_arg(args, int), radix);
                            break;
                        case 3:
                            printn(va_arg(args, long), radix);
                            break;
                        case 4:
                            printn(va_arg(args, long long), radix);
                            break;
                    }
                } else {
                    switch (argLength) {
                        case 0:
                        case 1:
                        case 2:
                            printun(va_arg(args, unsigned int), radix);
                            break;
                        case 3:
                            printun(va_arg(args, unsigned long), radix);
                            break;
                        case 4:
                            printun(va_arg(args, unsigned long long), radix);
                            break;
                    }
                }
            }
                

            state = 0;
            argLength = 0;
            radix = 10;
            sign = false;
            number = false;
            break;
        default:
            break;
        }
        str++;
    }
    va_end(args);
    setCursor(ScreenX, ScreenY);
}

void print_buffer(const char* msg, const void* buffer, uint32_t count)
{
    const uint8_t* u8Buffer = (const uint8_t*)buffer;
    
    putString(msg);
    for (uint16_t i = 0; i < count; i++)
    {
        putChar(cgHexChars[u8Buffer[i] >> 4]);
        putChar(cgHexChars[u8Buffer[i] & 0xF]);
    }
    putChar('\n');
}
