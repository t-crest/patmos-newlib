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

#ifndef _MACHSPM_H
#define _MACHSPM_H

#include <stddef.h>

/**
 * Attribute for pointers into the scratchpad memory. Use as
 *
 * _SPM int *p = (_SPM int *) 0x1234;
 *
 * @see _IODEV in machine/patmos.h
 */
#define _SPM __attribute__((address_space(1)))


/**
 * Address ranges for SPM memory.
 * Default SPM size is 2k.
 */
#define SPM_BASE            0x00000000
#define SPM_HIGH            0x000007ff
#define SPM_SIZE            (1 + SPM_HIGH - SPM_BASE)
#define SPM_WORDS	    (SPM_SIZE/4)


/***********************************************************/
/* SPM initialization and control functions                */
/***********************************************************/

static inline void spm_init(void)
{
    // Does not do anything at the moment, maybe we need to initialize something later
}

static inline void spm_wait(void)
{
    // this is currently a NO-OP as long as we do not use DMA
}

static inline unsigned spm_is_busy(void)
{
    return 0;
}



/***********************************************************/
/* SPM alignment helper functions                          */
/***********************************************************/

extern unsigned spm_block_shift;

static inline void spm_set_block_size(unsigned bs)
{
    spm_block_shift = ((bs >> 16) & 0xf);
}

static inline int spm_is_aligned(_SPM const void * test)
{
    unsigned t = (unsigned) test;
    return !(t & ((4 << spm_block_shift) - 1));
}

static inline _SPM void * spm_align_ceil(_SPM void * test)
{
    unsigned t = (unsigned) test;
    t |= ((4 << spm_block_shift) - 1);
    t++;
    return (_SPM void *) t;
}

static inline _SPM void * spm_align_floor(_SPM void * test)
{
    unsigned t = (unsigned) test;
    t &= ~((4 << spm_block_shift) - 1);
    return (_SPM void *) t;
}

static inline int spm_data_is_aligned(const void * test)
{
    unsigned t = (unsigned) test;
    return !(t & ((4 << spm_block_shift) - 1));
}

static inline void * spm_data_align_ceil(void * test)
{
    unsigned t = (unsigned) test;
    t |= ((4 << spm_block_shift) - 1);
    t++;
    return (void *) t;
}

static inline void * spm_data_align_floor(void * test)
{
    unsigned t = (unsigned) test;
    t &= ~((4 << spm_block_shift) - 1);
    return (void *) t;
}


/***********************************************************/
/* SPM memory block transfer functions                     */
/***********************************************************/

static inline void spm_copy_from_ext(_SPM void * dst, const void * src, size_t size)
{
    _SPM  char *to   = (_SPM  char*) dst;
    const char *from = (const char*) src;

    // lets have some fun ;)
    size_t n = (size + 7) / 8;
    switch (size % 8) {
    case 0: do {  *(to++) = *(from++);
    case 7:       *(to++) = *(from++);
    case 6:       *(to++) = *(from++);
    case 5:       *(to++) = *(from++);
    case 4:       *(to++) = *(from++);
    case 3:       *(to++) = *(from++);
    case 2:       *(to++) = *(from++);
    case 1:       *(to++) = *(from++);
	       } while (--n > 0);
    }
}

static inline void spm_copy_to_ext(void * dst, _SPM const void * src, size_t size)
{
               char *to   = (     char*) dst;
    _SPM const char *from = (_SPM const char*) src;

    // lets have some fun ;)
    size_t n = (size + 7) / 8;
    switch (size % 8) {
    case 0: do {  *(to++) = *(from++);
    case 7:       *(to++) = *(from++);
    case 6:       *(to++) = *(from++);
    case 5:       *(to++) = *(from++);
    case 4:       *(to++) = *(from++);
    case 3:       *(to++) = *(from++);
    case 2:       *(to++) = *(from++);
    case 1:       *(to++) = *(from++);
	       } while (--n > 0);
    }
}


/**
 * Copy data of size size from src to dest by using the whole SPM as buffer.
 */
extern void spm_memmove(void * dest, const void * src, size_t size);


#endif /* _MACHSPM_H */
