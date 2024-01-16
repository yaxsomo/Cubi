#ifndef __DISTANCE_H
#define __DISTANCE_H

extern float distances[];

void distance_init();
void distance_tick();
float distance_get(int index=0);

#endif
