#include "voltage.h"
#include <terminal.h>
#include "buzzer.h"

bool voltage_is_error;
int voltage_now;

bool voltage_error()
{
    return voltage_is_error;
}

const static float stepPerVolt = (4096/3.3)*VOLTAGE_R2/(float)(VOLTAGE_R1+VOLTAGE_R2);
const static int voltage_limit_l = VOLTAGE_LIMIT*stepPerVolt;
const static int voltage_USB = VOLTAGE_USB*stepPerVolt;

void voltage_init()
{
    pinMode(VOLTAGE_PIN, INPUT_ANALOG);

    voltage_now = analogRead(VOLTAGE_PIN);
    voltage_is_error = false;
}

void voltage_tick()
{
    static int divider = 0;
    divider++;

    if (divider > 1) {
        divider = 0;
        int newSample = analogRead(VOLTAGE_PIN);

        if (newSample < voltage_now) voltage_now--;
        if (newSample > voltage_now) voltage_now++;

        // if the voltage goes below VOLTAGE_USB, then we're probably only connected to the USB (or the battery is completely dead)
        if (voltage_is_error) {
            if (voltage_now > (voltage_limit_l + VOLTAGE_HYST) || (voltage_now <= voltage_USB)) {
                voltage_is_error = false;
                buzzer_stop();
            }
        } else {
            if (voltage_now < voltage_limit_l && (voltage_now > voltage_USB)) {
                voltage_is_error = true;
                buzzer_play(MELODY_ALERT, true);
            }
        }
    }
}

float voltage_current()
{
    return voltage_now / stepPerVolt;
}
TERMINAL_COMMAND(voltage, "Get the voltage (x10)")
{
    terminal_io()->println((int)(10*voltage_current()));
}