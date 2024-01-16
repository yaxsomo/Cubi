#include <stdlib.h>
#include <wirish/wirish.h>
#include <terminal.h>
#include <libmaple/adc.h>
#include <libmaple/timer.h>
#include <main.h>
#include "dc.h"
#include "buzzer.h"
#include "voltage.h"
#include "bt.h"
#include "model.h"
#include "imu.h"
#include "helpers.h"

#define NOSOUND 0
#define BAUDRATE 115200

// Without weights :
#define CART_KP 1000
#define CART_KI 10000
#define CART_KD 40

// With weights
// #define CART_KP 2000
// #define CART_KI 25000
// #define CART_KD 60
#define X_KP 0 //2
#define X_KI 0 //10

/*
  Timer2 controls the pwms 8, 9, 10 and 11
 */

int my_atoi(char *p, bool *success);
void default_cart_pole();
void p_demo_cart_pole();
void i_demo_cart_pole();
void d_demo_cart_pole();

bool isUSB = false;
bool isUSBForEver = true;
long counter = 0;
unsigned int hardwareCounter = 0;
bool readyToUpdateHardware = false;
bool led_state = false;

static float cart_kp = CART_KP;
static float cart_ki = CART_KI;
static float cart_kd = CART_KD;
static float cart_i_acc = 0;
static float cart_err = 0;
static float cart_last_err = 0;
static float cart_extra_rotation = 0;
static float adapted_cartgoal = 0;

static float cart_x_kp = X_KP;
static float cart_x_ki = X_KI;
static float cart_x_i_acc = 0;
static float cart_x_err = 0;

// Timming
static uint32 dt = 0;
static uint32 dt_ms = 0;
static uint32 dt_cart = 0;
static uint32 last_dt_cart = 0;
static uint32 time_micros = 0;
static uint32 old_time_micros = 0;
static uint32 time_ms = 0;
static uint32 old_time_ms = 0;
static uint32 half_max = (1 << 31);

TERMINAL_COMMAND(freq, "Returns the last measure of dt (the period of the main loop in micros)")
{
    terminal_io()->print(micros());
    terminal_io()->print(" = micros(), ");
    terminal_io()->print(millis());
    terminal_io()->print(" = millis(), dt = ");
    terminal_io()->print(dt);
    terminal_io()->print(" us, dt = ");
    terminal_io()->print(dt_ms);
    terminal_io()->print(" ms, freq = ");
    terminal_io()->print(1000000.0 / dt);
    terminal_io()->print(" Hz, dt_cart = ");
    terminal_io()->print(last_dt_cart);
    terminal_io()->print(" us, cart_freq = ");
    if (last_dt_cart != 0)
    {
        terminal_io()->print(1000000.0 / last_dt_cart);
        terminal_io()->println(" Hz");
    }
    else
    {
        terminal_io()->println(" I ain't no dividing by no zero");
    }
}

TERMINAL_COMMAND(carterr, "Debug function")
{
    terminal_io()->print("cart_err = ");
    terminal_io()->print(cart_err);
    terminal_io()->print(", cart_last_err = ");
    terminal_io()->println(cart_last_err);
}

TERMINAL_COMMAND(pwm, "[usage] pwm pwm_value1 pwm_value2")
{
    if (argc == 2)
    {
        bool success = false;
        int pwm1 = my_atoi(argv[0], &success);
        int pwm2 = my_atoi(argv[1], &success);
        _limit(&pwm1, MAX_PWM);
        _limit(&pwm2, MAX_PWM);
        dc_motor *motors = get_motors();
        motors[0].command = pwm1;
        motors[1].command = pwm2;
        terminal_io()->print("Setting pwms ");
        terminal_io()->print(pwm1);
        terminal_io()->print(" and ");
        terminal_io()->println(pwm2);
    }
    else
    {
        terminal_io()->println("[usage] pwm pwm_value1 pwm_value2");
    }
}

TERMINAL_COMMAND(rc, "Go to bluetooth serial mode")
{
    // Terminal management
    Serial1.begin(BAUDRATE);
    terminal_init(&Serial1);
    isUSB = false;
    isUSBForEver = false;
}
TERMINAL_COMMAND(usb, "Go to USB serial mode")
{
    // Terminal management
    isUSB = true;
    terminal_init(&SerialUSB);
    isUSBForEver = true;
}

TERMINAL_PARAMETER_BOOL(cartpole, "activates the cartpole mode", false);
TERMINAL_PARAMETER_FLOAT(cartgoal, "Angle goal for the inverted pendulum", 1.607);
TERMINAL_PARAMETER_FLOAT(criticangle, "Max angle we're allowed to aim around the vertical point to control the robot in rad", 0.005);

TERMINAL_COMMAND(cart, "goal(rads) [kp] [ki] [kd] [rotation]")
{
    if (argc >= 1)
    {
        cartpole = true;
        cartgoal = atof(argv[0]);
        // Resetting the accumulator
        cart_i_acc = 0;
    }
    if (argc >= 2)
    {
        cart_kp = atof(argv[1]);
        cart_ki = 0;
        cart_kd = 0;
    }
    if (argc >= 3)
    {
        cart_ki = atof(argv[2]);
        cart_kd = 0;
    }
    if (argc >= 4)
    {
        cart_kd = atof(argv[3]);
    }
    if (argc >= 5)
    {
        cart_extra_rotation = atof(argv[3]);
    }
}

TERMINAL_COMMAND(a, "debug")
{
    terminal_io()->println(adapted_cartgoal);
}

TERMINAL_COMMAND(c, "cartpole pid values: kp ki kd [rotation]")
{
    if (argc >= 3)
    {
        // Resetting the accumulator
        cart_i_acc = 0;
        cart_kp = atof(argv[0]);
        cart_ki = atof(argv[1]);
        cart_kd = atof(argv[2]);
    }
    if (argc >= 4)
    {
        cart_extra_rotation = atof(argv[3]);
    }
}

TERMINAL_COMMAND(d, "cartpole X pi values: kp ki")
{
    if (argc >= 2)
    {
        // Resetting the accumulator
        cart_x_i_acc = 0;
        cart_x_kp = atof(argv[0]);
        cart_x_ki = atof(argv[1]);
    }
}

TERMINAL_COMMAND(cd, "op command that sets the robot in inverted pendulum mode with default values")
{
    default_cart_pole();
}

TERMINAL_COMMAND(cdp, "Proportional control demo (needs an IMU)")
{
    p_demo_cart_pole();
}
TERMINAL_COMMAND(cdi, "Integral control demo (needs an IMU)")
{
    i_demo_cart_pole();
}
TERMINAL_COMMAND(cdd, "Derivate control demo (needs an IMU)")
{
    d_demo_cart_pole();
}
void default_cart_pole() {
    // Resetting the accumulator
    cart_i_acc = 0;
    cartpole = 1;
    model_reset();
    cart_kp = CART_KP;
    cart_ki = CART_KI;
    cart_kd = CART_KD;
    cart_x_kp = X_KP;
    cart_x_ki = X_KI;

    get_model()->mode = MODE_SPEED_CONTROL;
}

void p_demo_cart_pole() {
    // Resetting the accumulator
    cart_i_acc = 0;
    cartpole = 1;
    model_reset();
    cart_kp = CART_KP;
    cart_ki = 0;
    cart_kd = 0;
    cart_x_kp = 0;
    cart_x_ki = 0;

    get_model()->mode = MODE_SPEED_CONTROL;
}

void i_demo_cart_pole() {
    // Resetting the accumulator
    cart_i_acc = 0;
    cartpole = 1;
    model_reset();
    cart_kp = 0;
    cart_ki = CART_KI;
    cart_kd = 0;
    cart_x_kp = 0;
    cart_x_ki = 0;

    get_model()->mode = MODE_SPEED_CONTROL;
}

void d_demo_cart_pole() {
    // Resetting the accumulator
    cart_i_acc = 0;
    cartpole = 1;
    model_reset();
    cart_kp = 0;
    cart_ki = 0;
    cart_kd = CART_KD;
    cart_x_kp = 0;
    cart_x_ki = 0;

    get_model()->mode = MODE_SPEED_CONTROL;
}

void setup()
{
    // Wirish board specific init
    disableDebugPorts();

    pinMode(BOARD_LED_PIN, OUTPUT);
    pinMode(BOARD_BUTTON_PIN, INPUT);

    buzzer_init();
    if (!NOSOUND)
    {
        buzzer_play(MELODY_BOOT);
    }
    // Initializing IMU
    imu_init();

    model_init();
    motor_init();

    voltage_init();
    bt_init();

    // Terminal management
    Serial1.begin(BAUDRATE);
    // The HC05 is connected to Serial1
    terminal_init(&Serial1);
    ////terminal_init(&SerialUSB);

    digitalWrite(BOARD_LED_PIN, HIGH);

    digitalWrite(PWM12_ENABLE, HIGH);

    old_time_micros = micros();
    old_time_ms = millis();
    /*
    while(true) {
      delay(500);
      digitalWrite(BOARD_LED_PIN, LOW);
      delay(500);
      digitalWrite(BOARD_LED_PIN, HIGH);
    }*/
}

void loop()
{
    counter++;
    //delay(1);
    time_micros = micros();
    time_ms = millis();
    dt = time_micros - old_time_micros;
    dt_ms = time_ms - old_time_ms;

    if (dt > half_max)
    {
        // It probably means that the value reached its maximum and reseted
        uint32 max_value = 0;
        max_value = ~(max_value);
        dt = time_micros + (max_value - old_time_micros);
    }
    old_time_micros = time_micros;
    old_time_ms = time_ms;

    if (counter < 500)
    {
        digitalWrite(BOARD_LED_PIN, LOW);
    }
    else if (counter >= 500 && counter < 1000)
    {
        digitalWrite(BOARD_LED_PIN, HIGH);
    }
    else
    {
        counter = 0;
    }

    if (counter % 100 == 0)
    {
        // By default, the Serial used is the Bluetooth, except if a the SerialUSB is being written.
        // The isUSBForEver flag is used to persist in USB or in bluetooth, when asked through the terminal commands (rc/usb)
        if (SerialUSB.available() && !isUSB && isUSBForEver)
        {
            isUSB = true;
            terminal_init(&SerialUSB);
        }
    }

    if (digitalRead(BOARD_BUTTON_PIN))
    {
        default_cart_pole();
    }
    // Ticking IMU
    imu_tick();

    dt_cart = dt_cart + dt;

    if (cartpole && imu_is_checked)
    {
        imu_is_checked = false;
        cart_last_err = cart_err;
        // The angle goal is very slighlty modified with a PI controller so the X position of the robot (integrated through odometry) stays at what we want it to be
        cart_x_err = get_model()->x;
        cart_x_i_acc = cart_x_i_acc + cart_x_ki * cart_x_err * dt_cart / 1000000.0;
        _limit(&cart_x_i_acc, criticangle);
        float temp_sum = (cart_x_kp * cart_x_err + cart_x_i_acc);
        _limit(&temp_sum, criticangle);
        adapted_cartgoal = cartgoal + temp_sum;

        if (cart_x_kp == 0 && cart_x_ki == 0)
        {
            adapted_cartgoal = cartgoal;
        }
        cart_err = smooth_pitch - adapted_cartgoal;
        cart_i_acc = cart_i_acc + cart_ki * cart_err * dt_cart / 1000000.0;
        _limit(&cart_i_acc, MAX_PWM);
        // Putting minus signs was too exhausting.
        int speed = (int)(cart_kp * (cart_err) + cart_i_acc + cart_kd * (cart_err - cart_last_err) * 1000000 / dt_cart) * (-1);
        _limit(&speed, MAX_PWM);

        dc_motor *motors = get_motors();
        motors[0].speed_target = speed + cart_extra_rotation;
        motors[1].speed_target = speed - cart_extra_rotation;
        last_dt_cart = dt_cart;
        dt_cart = 0;
    }

    buzzer_tick();

    voltage_tick();
    terminal_tick();
    model_tick();
    dc_tick();

    return;
}

int my_atoi(char *p, bool *success)
{
    int k = 0;
    int sign = 1;
    if (*p == '-')
    {
        sign = -1;
        p++;
    }
    while (*p != '\0')
    {
        int value = *p - '0';
        if (value >= 0 && value <= 9)
        {
            k = k * 10 + value;
            p++;
        }
        else
        {
            *success = false;
            return 0;
        }
    }
    *success = true;
    return k * sign;
}
