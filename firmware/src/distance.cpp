#include <stdlib.h>
#include <terminal.h>
#include <wirish/wirish.h>
#include "mux.h"
#include "distance.h"
#include <function.h>

#define DISTANCE_EN_PIN     28

float distances[3];

Function volt_to_cm;

void distance_init()
{
    // Transistor pins
    digitalWrite(DISTANCE_EN_PIN, LOW);
    pinMode(DISTANCE_EN_PIN, OUTPUT);

    // From the datasheet 
    // http://www.sharp-world.com/products/device/lineup/data/pdf/datasheet/gp2y0a41sk_e.pdf
    volt_to_cm.addPoint(0.0, 40);
    volt_to_cm.addPoint(0.4, 32.5);
    volt_to_cm.addPoint(0.6, 22);
    volt_to_cm.addPoint(1, 13);
    volt_to_cm.addPoint(1.4, 9);
    volt_to_cm.addPoint(1.76, 7);
    volt_to_cm.addPoint(2.35, 5);
    volt_to_cm.addPoint(3.0, 3.5);
    volt_to_cm.addPoint(3.3, 2.0);
}

void distance_en(bool en)
{
    digitalWrite(DISTANCE_EN_PIN, en ? HIGH : LOW);
}

float distance_get(int index)
{
    distance_en(true);
    int mux;
    if (index == 0) mux = MUX1;
    if (index == 1) mux = MUX2;
    if (index == 2) mux = MUX3;

    return volt_to_cm.get(mux_secure_sample(6, mux));
}

void distance_tick() 
{
    for(int i=0; i<3; i++) {
        // XXX: This could be fatest, using only one set addr & 3 reads
        distances[i] = distance_get(i);
    }
}


TERMINAL_COMMAND(de, "Enable distances")
{
    /*
       distance_en(0, true);
       distance_en(1, true);
       distance_en(2, true);
       */
}

TERMINAL_COMMAND(dist, "Monitor distances")
{
    while (!SerialUSB.available()) {
        for (int index=0; index<3; index++) {
            terminal_io()->print(distance_get(index));
            terminal_io()->print("\t");
        }
        terminal_io()->println();
        delay(10);
    }
}
