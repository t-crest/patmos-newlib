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
// This code is partially based on Jack Whitham's spmsort package.
//


#ifndef _MACHUART_H
#define _MACHUART_H

#include <unistd.h>


/**
 * Print a zero-terminated string to UART.
 */
static inline void uart_print(const char* str)
{
    _write(STDOUT_FILENO, str, strlen(str));
}

/**
 * Print a zero-terminated string to UART and append a newline.
 */
static inline void uart_println(const char* str)
{
    _write(STDOUT_FILENO, str, strlen(str));
    _write(STDOUT_FILENO, "\n", 1);
}


// TODO define a few more helpers to print out numbers without resorting to printf


#endif /* _MACHUART_H */
