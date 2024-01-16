#include <terminal.h>
#include "mux.h"

#define DELAY 100

#define OPTICAL_EN1     13
#define OPTICAL_EN2     12

#define OPTICAL_NB 5

float opticals[OPTICAL_NB];

int opt_addr[OPTICAL_NB][2] = { 
  { 4, MUX1 },
  { 5, MUX2 },
  { 0, MUX2 },
  { 4, MUX2 },
  { 3, MUX2 }
}; 

int optical_get(int i) {
  if (i<0 || i >= OPTICAL_NB) return 0;
  return opticals[i];
}

void opticals_init()
{
  digitalWrite(OPTICAL_EN1, LOW);
  digitalWrite(OPTICAL_EN2, LOW);
  pinMode(OPTICAL_EN1, OUTPUT);
  pinMode(OPTICAL_EN2, OUTPUT);
  digitalWrite(OPTICAL_EN1, HIGH);
  digitalWrite(OPTICAL_EN2, HIGH);
}

void opticals_tick() {
  for (int i=0; i<OPTICAL_NB; i++)
    opticals[i] = mux_secure_sample(opt_addr[i][0], opt_addr[i][1]);
}

TERMINAL_COMMAND(opt, "Test opticals")
{
  digitalWrite(OPTICAL_EN1, HIGH);
  digitalWrite(OPTICAL_EN2, HIGH);
  
  while (!SerialUSB.available()) {
    int n = sizeof(opt_addr) / (2*sizeof(int));
    for (int i=0; i<n; i++) {
      terminal_io()->print(mux_secure_sample(opt_addr[i][0], opt_addr[i][1]));
      terminal_io()->print('\t');
    }
    terminal_io()->println();
  }
}
