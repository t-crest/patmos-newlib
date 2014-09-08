#include <stdio.h>
#include <stdlib.h>
#include <machine/patmos.h>
#include <machine/spm.h>
 
extern int _addr_base_spm;
extern int _addr_base_ext;
extern int _spm_ext_diff;
extern int SWSC_SIZE;
 
 
int main(int argc, char **argv) {
 
  printf("0x%x\n", _addr_base_spm);
  printf("0x%x\n", _addr_base_ext);
 
  // test if we can write to these
  int *foo = (int*) _addr_base_ext;
  *foo = 42;
  _SPM int *bar = (_SPM int*) _addr_base_spm;
  *bar = 42;
 
 _SPM unsigned int *sc_top;
  _UNCACHED unsigned int *m_top;
  int i, n, n_spill;
  unsigned spilled_word;
  asm("mov %0 = $r1;" // copy argument to n
      "mov %1 = $r27;" // copy st to sc_top
      "mov %2 = $r28;" // copy ss to m_top
     // : "=r" (n) 
      : "=r" (n), "=r"(sc_top), "=r"(m_top));

  sc_top -= n;
  n_spill = sc_top + _spm_ext_diff - m_top - SWSC_SIZE;
 
  for (i = 0; i < n_spill; i++){
        
	m_top -= 0x01;
	spilled_word = *sc_top;         
	*m_top = spilled_word;
  }

  asm(
      "mov $r27 = %0;" // copy sc_top to st
      "mov $r28 = %1;" // copy m_top to ss
     // : "=r" (n) 
      :"=r"(sc_top), "=r"(m_top));

  puts("Hello world.\n");
 
  return 0;
}
