// Copyright 2014 Sahar Abbaspour
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

#include <stdio.h>
#include <stdlib.h>
#include <machine/patmos.h>
#include <machine/spm.h>
 
extern unsigned _addr_base_spm;
extern unsigned _addr_base_ext;
extern unsigned _spm_ext_diff;
extern unsigned SWSC_EXT_SIZE;
extern unsigned SWSC_SPM_SIZE;

#define MASK (SWSC_SPM_SIZE - 1)

// define to poison SPM when spilling word
//#define POISON
// define to disable filling (check if poisoned value comes back)
// undefine to test whether spill/fill works
//#define NOENSURE

int test;
#if 1
void _sc_reserve() __attribute__((naked,used,patmos_sc_func));
void _sc_reserve()
{

  int n, m_top, sc_top;
  unsigned spilled_word;
  int   i;
  int  n_spill;
  asm volatile("mov %0 = $r1;" // copy argument to n
      "mov %1 = $r19;" // copy st to sc_top
      "mov %2 = $r20;" // copy ss to m_top
      : "=r" (n), "=r"(sc_top), "=r"(m_top) 
      ::
      );



  sc_top -= n * 4;
  n_spill = (m_top - sc_top - (int) SWSC_SPM_SIZE) / 4;


  for (i = 0; i < n_spill; i++){
    m_top -= 4;
    _SPM unsigned *spm = (_SPM unsigned *) (m_top & MASK);
    _UNCACHED unsigned *ext_mem = (_UNCACHED unsigned *) (m_top);
    #ifdef POISON
      spilled_word = *spm;
      *spm = -1;
    #else
      spilled_word = *spm;
    #endif
    *ext_mem = spilled_word;
  }

  asm volatile(
      "mov $r19 = %0;" // copy sc_top to st
      "mov $r20 = %1;" // copy m_top to ss
      : 
      : "r"(sc_top), "r"(m_top) 
      : "$r19", "$r20" 
      );
}
#endif

#if 1
void _sc_ensure() __attribute__((naked,used,patmos_sc_func,patmos_preserve_ret));
void _sc_ensure()
{
#if 1
  int  n, m_top, sc_top;
  unsigned filled_word;
  int i, n_fill;
  asm (
      "mov %0 = $r8;" // copy argument to n
      "mov %1 = $r19;" // copy st to sc_top
      "mov %2 = $r20;" // copy ss to m_top
      : "=r" (n), "=r"(sc_top), "=r"(m_top) 
      ::
      );

  n_fill = (n*4 - (m_top - sc_top)) / 4;

  for (i = 0; i < n_fill; i++){
    _SPM unsigned *spm = (_SPM unsigned *) (m_top & MASK);
    _UNCACHED unsigned *ext_mem = (_UNCACHED unsigned *) (m_top);

    filled_word = *ext_mem;
    #ifndef NOENSURE
      *spm = filled_word;
    #endif
    m_top += 4;
  }


  asm volatile(
      "mov $r20 = %0;" // m_top
      :
      : "r"(m_top)
      : "$r20"
      );

#endif
}
#endif


#if 1
void _sc_free() __attribute__((naked,used,patmos_sc_func,patmos_preserve_ret));
void _sc_free()
{
#if 1
  int sc_top, m_top, n;

  asm volatile(
      "mov %0 = $r8;" // copy argument to n
      "mov %1 = $r19;" // copy st to sc_top
      "mov %2 = $r20;" // copy ss to m_top
      : "=r" (n), "=r"(sc_top), "=r"(m_top) /* output regs */
      ::
      );

  sc_top += n*4;
   
  if (sc_top > m_top) {
  m_top = sc_top ;
  }

  asm volatile(
      "mov $r19 = %0;" // copy sc_top to st
      "mov $r20 = %1;" // copy m_top to ss
      : /* no output regs */
      : "r"(sc_top), "r"(m_top) /* input regs */
      : "$r19", "$r20" /* clobbered */
      );
#endif
}
#endif
