#ifndef _MUX_H
#define _MUX_H

#include <wirish/wirish.h>

#define MUX_ADDR1   20
#define MUX_ADDR2   18
#define MUX_ADDR3   7
#define MUX1    10
#define MUX2    6
#define MUX3    33

static void mux_set_addr(int addr) __attribute__((unused));
static void mux_init() __attribute__((unused));
static int mux_sample(int mux) __attribute__((unused));
static int mux_read_3() __attribute__((unused));

extern uint8_t addr_modif;

/* The sample status is used for secure sample mechanism,
 * error occurs if the system couldn't get a sample without
 * coder sampling interrupt within the maximum try number */
typedef enum {
  SSReady,
  SSBusy,
  SSError
} SampleStatus;
extern SampleStatus sample_status;
#define SECURE_SAMPLE_MAX_TRY_NB 10

/* secure sample try to avoid interrupt by coder samples.
 * (update the sample_status).
 * returns the voltage (V) */
float mux_secure_sample(int addr, int mux); 

static inline void mux_addr_modif_reset() {
  addr_modif = 0;
}

static inline int mux_addr_modified() {
  return addr_modif == 1;
}

static inline void mux_set_addr(int addr)
{
    gpio_write_bit(GPIOA, 15, (addr>>0)&1);
    gpio_write_bit(GPIOB, 4, (addr>>1)&1);
    gpio_write_bit(GPIOA, 4, (addr>>2)&1);
    addr_modif = 1;
}

static void mux_init()
{
    mux_set_addr(0);
    pinMode(MUX_ADDR1, OUTPUT);
    pinMode(MUX_ADDR2, OUTPUT);
    pinMode(MUX_ADDR3, OUTPUT);
    pinMode(MUX1, INPUT_FLOATING);
    pinMode(MUX2, INPUT_FLOATING);
    pinMode(MUX3, INPUT_FLOATING);
}

static int mux_sample(int mux)
{
    return analogRead(mux);
}

static inline int mux_read_1()
{
    return gpio_read_bit(GPIOA, 1) ? HIGH : LOW;
}
static inline int mux_read_2()
{
    return gpio_read_bit(GPIOA, 5) ? HIGH : LOW;
}
static inline int mux_read_3()
{
    return gpio_read_bit(GPIOB, 1) ? HIGH : LOW;
}

#endif
