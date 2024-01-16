#ifndef _VOLTAGE_H
#define _VOLTAGE_H

#define VOLTAGE_R1      62
#define VOLTAGE_R2      30
#define VOLTAGE_LIMIT   8.9
#define VOLTAGE_HYST    0.2
#define VOLTAGE_USB     5.3
#define VOLTAGE_PIN     3

bool voltage_error();
void voltage_init();
void voltage_tick();
float voltage_current();

#endif
