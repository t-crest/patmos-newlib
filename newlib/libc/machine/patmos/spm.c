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
// This code is partially based on Jack Witham's spmsort package.
//

#include <assert.h>
    
#include <machine/spm.h>

unsigned spm_block_shift = 0;

void spm_memcpy(void * dest, const void * source, size_t size)
{
    unsigned char * source_aligned = spm_align_floor((void *) source);
    unsigned char * dest_aligned = spm_align_floor((void *) dest);
    unsigned source_offset = (unsigned char *) source - source_aligned;
    unsigned dest_offset = (unsigned char *) dest - dest_aligned;
    unsigned num_blocks = ~0;
    unsigned spm_block_size = (4 << spm_block_shift);

    assert (source_offset == dest_offset);
    if (size == 0) return;

    /* Effortless copy when the alignments match */
    do {
        spm_copy_from_ext(SPM_BASE, source_aligned, SPM_SIZE);
        source_aligned += spm_block_size;
        if (num_blocks == ~0) {
            /* Compute total transfer size during first transfer */
            num_blocks = (size + spm_block_size - 1) / spm_block_size;
        }

        spm_copy_to_ext(dest_aligned, SPM_BASE, SPM_SIZE);
        dest_aligned += spm_block_size;
        num_blocks--;
    } while (num_blocks != 0);

    spm_wait();
}

