#ifndef __DC_H
#define __DC_H


// Motors pins mapping

// Bread-board connections
/*
#define M1A 6
#define M1B 7
#define M2A 4
#define M2B 5
#define PWM12_ENABLE 29
#define PWM34_ENABLE 28
*/
// PCB board connections
#define M1A 5
#define M1B 4
#define M2A 13
#define M2B 12
#define PWM12_ENABLE 6
#define PWM34_ENABLE 14

#define PWM1A 8 // Timer2
#define PWM1B 9 // Timer2
#define PWM2A 10 // Timer2
#define PWM2B 11 // Timer2

// 7 impulsions per encoder rotation. Up and down on A and B => 7*4 changes per motor rotation
// => 7*4*ReductionRatio = CPR
// At 20kHz and a 2800 CPR, the maximum speed we can track is 7 wheel rotations per second
// Cycles per revolution 
#define CPR  2800

// Board specific
#define CRISTAL_FREQ    72000000

// PWM frequency (all values are not possible)
#define PWM_FREQ        20000

// Interrupt 1 frequency (all values are not possible)
#define INT_FREQ        20000

// Prescale for the PWM timer
#define PWM_PRESCALE    1

// Maximum value that we can use when we write a PWM
#define MAX_PWM         (CRISTAL_FREQ/PWM_PRESCALE)/PWM_FREQ

// Maximum average voltage that will be send to a motor
#define MAX_PWM_VOLTAGE 9

// Period of time between each speed estimation
#define SERVO_DT        20

// Delta time of the speed estimation (ms), should be a multiple of above
#define SPEED_DT        60

// Size of the ring speed estimation ring buffer
#define SPEED_RB        (((SPEED_DT)/SERVO_DT)+1)

// Maximum PWM that can be sent by the I (PID) component alone
#define MAX_I           MAX_PWM

// Servo divider overflow
#define SERVO_OVF       (INT_FREQ*SERVO_DT/1000)

// Minimum PWM value that must be sent to start moving the robot
#define STATIC_FRICTION_PWM 1300

extern int nbDc;
extern int prevNbDc;

struct dc_motor {
    int a, b;
    int pwm1, pwm2;
    // Storing positions in a circular buffer
    int encoder_rb[SPEED_RB];
    // Current id in the circular buffer
    int rb_pos;
    int encoder_value;
    int encoder_phase;
    int dir;
    int speed;
    int prev_speed;
    int speed_target;
    // Accumulator for the I component
    int acc;
    int command;
    int nb_errors;
};

dc_motor * get_motors();

void motor_init();

static void encoder_update(dc_motor * motor, int phase);

static void encoders_update();

void servo_update();

static void _dc_ovf();

int _min(int a, int b);

void dc_command();

// Converts rad/s to firmware unit (step per speed_dt ms)
int SI2firmware(float w);

// Converts firmware unit (step per speed_dt ms) to rad/s 
float firmware2SI(int enc_speed);

static int s(int m);


void _limit(int *i, int max_value);
void _limit(float *i, float max_value);

void dc_tick();


#endif
