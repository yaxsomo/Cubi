#include <stdio.h>
#include <terminal.h>
#include <wirish.h>
#include "dc.h"
#include "model.h"
#include "helpers.h"
#include "voltage.h"


// Dividers
static int divider = 0;
bool servoFlag = false;
static dc_motor motors[2];
HardwareTimer timer4(4);
HardwareTimer timer2(2);
int nbSpeeds = 0;
int nbDc = 0;
int prevNbDc = 0;

// PID parameters found with the Ziegler-Nichols method
TERMINAL_PARAMETER_INT(kp, "PID P", 20);
TERMINAL_PARAMETER_INT(ki, "PID I", 3); // Found 5, went lower because extreme changes in command would still create instabilities
TERMINAL_PARAMETER_INT(kd, "PID D", 2);
TERMINAL_PARAMETER_INT(enableodo, "enables/disables the odometry calculations", 1);


TERMINAL_COMMAND(nbspeeds, "Number of times the speed has been estimated since start (and the number of timer ovf)")
{
    terminal_io()->println(nbSpeeds);
    terminal_io()->println(nbDc);
}

// Result : 5us. Note that at 20KHz, we have 50us in between interrupts.
TERMINAL_COMMAND(eb, "Encoders benchmark")
{
    int avg = 0;
    for (int k=0; k<10000; k++) {
        int start = micros();
        encoders_update();
        int elapsed = micros()-start;
        avg += elapsed;
    }
    terminal_io()->println(avg/10000);
}

// Result (back when some of the calculation wasn't float): when speeds are not 0, ~340 us
// Result (full float) : when speeds are not 0, ~320 us...
TERMINAL_COMMAND(odobench, "Odometry benchmark")
{
    int avg = 0;
    for (int k=0; k<10000; k++) {
        int start = micros();
        increment_odometry();        
        int elapsed = micros()-start;
        avg += elapsed;
    }
    terminal_io()->println(avg/10000);
}
// Result (back when some of the calculation wasn't float) : when speeds are not 0, measure_ik ~90us, dx_dy ~90us.
// Result (full float) : when speeds are not 0, measure_ik ~73us, dx_dy ~108us.
TERMINAL_COMMAND(mikbench, "Measure IK benchmark")
{
    int avg = 0;
    int avg2 = 0;
    for (int k=0; k<10000; k++) {
        int start = micros();

        float linearSpeed;
        float rotationSpeed;
        measure_ik(&linearSpeed, &rotationSpeed);
        int elapsed = micros()-start;
        avg += elapsed;

        start = micros();
        // Constant speed over a duration -> distance
        float dt = SERVO_DT/1000.0;
        float dx = 0;
        float dy = 0;
        float dtheta = 0;
        
        dx_dy(linearSpeed, rotationSpeed, dt, &dx, &dy, &dtheta);       
        elapsed = micros()-start;
        avg2 += elapsed;
    }
    terminal_io()->print("measure_ik (us) : ");
    terminal_io()->println(avg/10000);
    terminal_io()->print("dx_dy (us) : ");    
    terminal_io()->println(avg2/10000);
}

TERMINAL_COMMAND(dct, "DC test")
{
    if (argc == 2) {
        int command1 = atoi(argv[0]);
        int command2 = atoi(argv[1]);
        _limit(&command1, MAX_PWM);
        _limit(&command2, MAX_PWM);
        
        motors[0].command = command1;
        motors[1].command = command2;

        terminal_io()->print("Sending to motor 1 : ");        
        terminal_io()->println(motors[0].command);
        terminal_io()->print("Sending to motor 2 : ");        
        terminal_io()->println(motors[1].command);
        
        dc_command();
    }
}


TERMINAL_COMMAND(em, "em")
{
    get_model()->mode = 0;
    motors[0].command = 0;
    motors[1].command = 0;
    dc_command();
    for (int k=0; k<2; k++) {
        motors[k].speed_target = 0;
    }
}

TERMINAL_COMMAND(enc, "Read encoders")
{
    while (!SerialUSB.available()) {
        for (int k=0; k<2; k++) {
            terminal_io()->print(motors[k].encoder_value);
            terminal_io()->print(" ");
        }
        terminal_io()->println();
        delay(10);
    }
}

TERMINAL_COMMAND(mode, "Sets how the motors are controlled (0 is speed control, 1 is PWM control, 2 is ikasserv)")
{
    if (argc == 1) {
        get_model()->mode = atoi(argv[0]);
    }
}

TERMINAL_COMMAND(er, "Encoders reset")
{
    for (int k=0; k<2; k++) {
        motors[k].encoder_value = 0;
        motors[k].nb_errors = 0;
    }
}

TERMINAL_COMMAND(es, "Encoders speed")
{
    //while (!SerialUSB.available()) {
        for (int k=0; k<2; k++) {
            terminal_io()->print(motors[k].speed);
            terminal_io()->print(" ");
        }
        terminal_io()->println(" [imp/SPEED_DT]");

        for (int k=0; k<2; k++) {
            terminal_io()->print(firmware2SI(motors[k].speed)*RAD2DEG);
            terminal_io()->print(" ");
        }
        terminal_io()->println(" [deg/s]");
        //delay(10);
    //}
}

TERMINAL_COMMAND(nberrors, "Number of errors read")
{
    while (!SerialUSB.available()) {
        for (int k=0; k<2; k++) {
            terminal_io()->print(motors[k].nb_errors);
            terminal_io()->print(" ");
        }
        terminal_io()->println();
        delay(10);
    }
}

TERMINAL_COMMAND(st, "Speed target")
{
    if (argc == 2) {
        motors[0].speed_target = atoi(argv[0]);
        motors[1].speed_target = atoi(argv[1]);
    }
}

TERMINAL_COMMAND(stdeg, "Speed target in deg/s")
{
    if (argc == 2) {
        motors[0].speed_target = SI2firmware(DEG2RAD*atoi(argv[0]));
        motors[1].speed_target = SI2firmware(DEG2RAD*atoi(argv[1]));
    }
}

TERMINAL_COMMAND(printmotor, "Prints the current motor info")
{
    for (int k=0; k<2; k++) {
        terminal_io()->print("Motor ");
        terminal_io()->println(k);
        
        terminal_io()->print("rb_pos ");
        terminal_io()->println(motors[k].rb_pos);
        
        terminal_io()->print("encoder_value ");
        terminal_io()->println(motors[k].encoder_value);

        terminal_io()->print("encoder_phase ");
        terminal_io()->println(motors[k].encoder_phase);

        terminal_io()->print("dir ");
        terminal_io()->println(motors[k].dir);

        terminal_io()->print("speed ");
        
        terminal_io()->println(motors[k].speed);

        terminal_io()->print("prev_speed ");
        terminal_io()->println(motors[k].prev_speed);

        terminal_io()->print("speed_target ");
        terminal_io()->println(motors[k].speed_target);

        terminal_io()->print("acc ");
        terminal_io()->println(motors[k].acc);

        terminal_io()->print("command ");
        terminal_io()->println(motors[k].command);
        terminal_io()->println();

        terminal_io()->print("divider ");
        terminal_io()->println(divider);

        terminal_io()->println("enc_positions ");
        for (int i = 0; i < SPEED_RB; i ++) {
            terminal_io()->println(motors[k].encoder_rb[(motors[k].rb_pos + i)%SPEED_RB]); 
        }
        terminal_io()->println();   
    }
}

dc_motor * get_motors() {
    return motors;
}

void motor_init() {
    // Timer configuration
    timer4.pause();
    // Trying 20kHz for now
    //timer4.setChannel1Mode(TIMER_OUTPUT_COMPARE);    
    timer4.setPrescaleFactor(1);
    timer4.setOverflow(CRISTAL_FREQ/INT_FREQ);
    timer4.setCompare(TIMER_CH1, 1);
    timer4.attachCompare1Interrupt(_dc_ovf);
    timer4.refresh();
    timer4.resume();

    timer2.pause();
    // The timer overflows at 20kHz. Used for the PWM
    timer2.setChannel1Mode(TIMER_PWM);
    timer2.setPrescaleFactor(PWM_PRESCALE);
    timer2.setOverflow(CRISTAL_FREQ/PWM_FREQ);
    timer2.refresh();
    timer2.resume();

    // Pin setup
    pinMode(PWM12_ENABLE, OUTPUT);
    pinMode(PWM34_ENABLE, OUTPUT);

    digitalWrite(PWM12_ENABLE, HIGH);
    digitalWrite(PWM34_ENABLE, HIGH);    

    motors[0].a = M1A;
    motors[0].b = M1B;
    motors[1].a = M2A;
    motors[1].b = M2B;

    motors[0].pwm1 = PWM1A;
    motors[0].pwm2 = PWM1B;
    motors[1].pwm1 = PWM2A;
    motors[1].pwm2 = PWM2B;

    for (int k = 0; k < 2; k++) {
        pinMode(motors[k].a, INPUT);
        pinMode(motors[k].b, INPUT);
        pinMode(motors[k].pwm1, PWM);
        pinMode(motors[k].pwm2, PWM);

        pwmWrite(motors[k].pwm1, 0);  
        pwmWrite(motors[k].pwm2, 0);  
        
        motors[k].rb_pos = 0;
        motors[k].encoder_value = 0;
        motors[k].encoder_phase = 0;
        motors[k].dir = 0;
        motors[k].speed = 0;
        motors[k].prev_speed = 0;
        motors[k].speed_target = 0;
        motors[k].command = 0;
        motors[k].nb_errors = 0;

        for (int i = 0; i < SPEED_RB;i++) {
            motors[k].encoder_rb[i] = 0;
        }
    }
}

static void encoder_update(dc_motor * motor, int phase)
{
    if (motor->encoder_phase != phase) {
        // Compute the phase diff, could be -1, 1 or 2
        int diff = phase-motor->encoder_phase;
        if (diff == -3) diff = 1;
        else if (diff == 3) diff = -1;
        else if (diff == -2) diff = 2;

        if (diff == 1) {
            motor->dir = 2;
            motor->encoder_value++;
        } else if (diff == -1) {
            motor->dir= -2;
            motor->encoder_value--;
        } else if (diff == 2) {
            // We missed a step, we assume the rotation didn't change its direction
            motor->encoder_value += motor->dir;
            motor->nb_errors++;
        }
        // Store the phase
        motor->encoder_phase = phase;
    }
}

static void encoders_update()
{
    int phases[2];
    for (int k=0; k<2; k++) {
        phases[k] = digitalRead(motors[k].a)|(digitalRead(motors[k].b)<<1);
    }

    for (int k=0; k<2; k++) {
        int phase = phases[k];

        // Astuce ! Renaming '11' to '2' and '10' to 3 makes the phase move in the natural order
        switch (phase) {
            case 0b00:
                encoder_update(&(motors[k]), 0);
                break;
            case 0b01:
                encoder_update(&(motors[k]), 1);
                break;
            case 0b11:
                encoder_update(&(motors[k]), 2);
                break;
            case 0b10:
                encoder_update(&(motors[k]), 3);
                break;
        }
    }
}

void servo_update()
{
    // Reducing the chance that a motor's enc value was updated inbetween.
    int encs[2];
    encs[0] = motors[0].encoder_value;
    encs[1] = motors[1].encoder_value;
    for (int k=0; k<2; k++) {
        int old_pos = motors[k].rb_pos;

        // Updating encoder pos in the circular buffer
        motors[k].rb_pos++;
        if (motors[k].rb_pos >= SPEED_RB) {
            motors[k].rb_pos = 0;
        }

        // Updating speed estimations
        motors[k].encoder_rb[old_pos] = encs[k];
        motors[k].prev_speed = motors[k].speed;
        motors[k].speed = motors[k].encoder_rb[old_pos] - motors[k].encoder_rb[motors[k].rb_pos];
    }
   if (enableodo) {
        increment_odometry();
    }
}

static void _dc_ovf()
{
    // Updating encoders
    encoders_update();

    // Updating servoing (TODO handle the overflow, it could happend if the robot worked at full speed for about a day)
    divider++;
    nbDc++;
    if (divider >= SERVO_OVF) {
        nbSpeeds++;
        divider = 0;
        servoFlag = true;
    }
}

int _min(int a, int b)
{
    return (a < b) ? a : b;
}

void dc_command()
{
    // Trying to make the PWM commands battery voltage independent
    float voltage = voltage_current();
    for (int k = 0; k < 2; k++) {           
        int command = motors[k].command* MAX_PWM_VOLTAGE/voltage;
        _limit(&command, MAX_PWM);
        if (command > 0) {
            if (command < STATIC_FRICTION_PWM) {
                command = STATIC_FRICTION_PWM;
            }
            pwmWrite(motors[k].pwm1, abs(command));      
            pwmWrite(motors[k].pwm2, 0);      
        } else {
            if (command > STATIC_FRICTION_PWM) {
                command = -STATIC_FRICTION_PWM;
            }
            pwmWrite(motors[k].pwm1, 0);      
            pwmWrite(motors[k].pwm2, abs(command));  
        }
    }
}

// Converts rad/s to firmware unit (step per speed_dt ms)
int SI2firmware(float w)
{
    return w*(SPEED_DT/1000.0)*CPR/(2*M_PI);
}

// Converts firmware unit (step per speed_dt ms) to rad/s 
float firmware2SI(int enc_speed) {
  return ((float) enc_speed) / (SPEED_DT/1000.0) / CPR * (2*M_PI);
}

static int s(int m)
{
    return (m > 0) ? 1 : -1;
}

void _limit(int *i, int max_value)
{
    if (*i < -max_value) *i = -max_value;
    if (*i > max_value) *i = max_value;
}

void _limit(float *i, float max_value)
{
    if (*i < -max_value) *i = -max_value;
    if (*i > max_value) *i = max_value;
}

void dc_tick()
{
    if (servoFlag) {
        servoFlag = 0;

        // Updating the speed calculations
        servo_update();

        if (get_model()->mode != MODE_PWM_CONTROL) {
            if (get_model()->mode == MODE_IK_CONTROL) {
                ik_asserv_tick();
            }
            // Control calculations
            for (int k=0; k<2; k++) {

                int err = (motors[k].speed_target - motors[k].speed);
                motors[k].acc += ki * err;
                _limit(&motors[k].acc, MAX_I);

                float p_contribution = kp * err;
                /* Bad idea actually :D
                if ((p_contribution < 0 && motors[k].acc >= 0) || (p_contribution > 0 && motors[k].acc <= 0)) {
                    // Astuce !
                    motors[k].acc = 0;
                }*/

                int command = p_contribution + motors[k].acc + kd * (motors[k].speed - motors[k].prev_speed);
                motors[k].command = command;
            }
        }

        dc_command();
    }
}

