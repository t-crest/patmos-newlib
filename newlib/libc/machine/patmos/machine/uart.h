// Copyright 2013 Stefan Hepp
// 
// This file is part of the newlib C library for the Patmos processor.
// 
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// (COPYING3.LIB). If not, see <http://www.gnu.org/licenses/>.
//
// This header provides simple functions to send strings, characters and integers
// over the UART.
//


#ifndef _MACHUART_H
#define _MACHUART_H

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * Read up to len characters from UART into buf.
 * Returns the number of characters read.
 */
static inline int uart_read(char *buf, int len)
{
    return _read(STDIN_FILENO, buf, len);
}

/**
 * Send some raw data of len bytes over UART.
 */
static inline void uart_write(const void* data, size_t len)
{
    _write(STDOUT_FILENO, data, len);
}

/**
 * Print a zero-terminated string to UART.
 */
static inline void uart_print(const char* str)
{
    size_t len = strlen(str);
    if (len > 0) {
	_write(STDOUT_FILENO, str, len);
    }
}

/**
 * Print a single character to UART.
 */
static inline void uart_printc(char c)
{
    _write(STDOUT_FILENO, &c, 1);
}

/**
 * Print a zero-terminated string to UART and append a newline.
 */
static inline void uart_println(const char* str)
{
    uart_print(str);
    uart_printc('\n');
}

/**
 * Print a signed integer using a decimal base to UART.
 */
static inline void uart_int(long long int i) 
{
    char s[19];
    unsigned len = 0;
    char *p = s + 19;

    if (i < 0) {
	char c = '-';
	_write(STDOUT_FILENO, &c, 1);
    }

    long long unsigned rem = llabs(i);
    do {
	*(--p) = '0' + (rem % 10);
	rem = rem / 10;
	len++;
    } while (rem > 0 && len < 19);
    
    _write(STDOUT_FILENO, p, len);
}

/**
 * Print an unsigned integer as hexadecimal value with a fixed 
 * number of digits to UART.
 */
static inline void uart_hex(long long unsigned i, unsigned digits)
{
    char s[16];
    unsigned len = 0;
    char *p = s + 16;

    do {
	char c = (i & 0xf);
	*(--p) = c < 10 ? '0' + c : 'a' + c - 10;
	i = i / 16;
	len++;
	if (len == 16) break;
    } while ((digits > 0 && len < digits) || (digits == 0 && i > 0));
    
    _write(STDOUT_FILENO, p, len);
}

/**
 * Print a string followed by an integer to UART.
 */
static inline void uart_printd(const char* str, long long int i)
{
    uart_print(str);
    uart_int(i);
}

/**
 * Print a string followed by a hexadecimal value using a fixed number of digits to UART.
 */
static inline void uart_printh(const char* str, long long unsigned i, unsigned digits) 
{
    uart_print(str);
    uart_hex(i, digits);
}

/**
 * Print a string followed by a signed integer value and a newline character  to UART.
 */
static inline void uart_printdln(const char* str, long long int i)
{
    uart_print(str);
    uart_int(i);
    uart_printc('\n');
}

/**
 * Print a string followed by a hexadecimal value and a newline character to UART.
 */
static inline void uart_printhln(const char* str, long long unsigned i, unsigned digits) 
{
    uart_print(str);
    uart_hex(i, digits);
    uart_printc('\n');
}


#endif /* _MACHUART_H */
