#include <stdlib.h>
#include <wirish/wirish.h>
#include <terminal.h>
#include "mux.h"

uint8_t addr_modif = 0;
SampleStatus sample_status = SSReady;

float mux_secure_sample(int addr, int mux) {
  sample_status = SSBusy;
  int try_nb = SECURE_SAMPLE_MAX_TRY_NB; 
  int v,ok;
  do {
    mux_set_addr(addr);
    mux_addr_modif_reset(); /* one detects an interrupt by coders' sample */
    delay_us(2); /* from the multiplexer spec, propagation takes less that 1 us */ 
    v = mux_sample(mux);
    ok = !mux_addr_modified();
  } while (try_nb-- > 0 && !ok);
    
  if (ok) { 
    sample_status = SSReady;
    return v*3.3/4096;
  }
  else { 
    sample_status = SSError;
    return 0.0;
  }
}

TERMINAL_COMMAND(mux, "Debug multiplexer")
{
    int v1,v2,v3;
    for (int k=0; k<8; k++) {
        mux_set_addr(k);
	mux_addr_modif_reset();
        delay_us(20);
	v1 = mux_sample(MUX1);
	v2 = mux_sample(MUX2);
	v3 = mux_sample(MUX3);
	if (mux_addr_modified())
	  v1 = v2 = v3 = 0;
        terminal_io()->print(k);
        terminal_io()->print(" ");
        terminal_io()->print(mux_sample(MUX1));
        terminal_io()->print(" ");
        terminal_io()->print(mux_sample(MUX2));
        terminal_io()->print(" ");
        terminal_io()->println(mux_sample(MUX3));
    }
}
