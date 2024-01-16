#ifndef _OPTICALS_H
#define _OPTICALS_H

#define OPTICAL_NB 5

extern float opticals[];

int optical_get(int i);
void opticals_init();
void opticals_tick();

#endif
