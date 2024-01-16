#include <terminal.h>

#define BTCONF_PIN              22

void bt_init()
{
    pinMode(BTCONF_PIN, OUTPUT);
    digitalWrite(BTCONF_PIN, LOW);

    /*
    Serial1.begin(38400);
    for (int k=0; k<5; k++) {
        Serial1.write("AT+RESET\r\n\r\n");
        Serial1.write("AT+RESET\r\n");
    }
    Serial1.begin(115200);
    */
}

// This doesn't work well at all. I'm missing something, works once in while.
static void bt_conf(char *name, char *pin)
{
    digitalWrite(BTCONF_PIN, HIGH);

    Serial1.write("AT\r\n");
    delay(10);
    if (Serial1.available()) {
        terminal_io()->println("Reading :");
        while(Serial1.available()) {
            terminal_io()->print((char)Serial1.read());
        }
        terminal_io()->println();
    } else {
        terminal_io()->println("Nothing to read");
    }
    Serial1.write("AT+UART=115200,0,0\r\n");
    delay(10);
    Serial1.write("AT+NAME=");
    Serial1.write(name);
    Serial1.write("\r\n");
    delay(10);
    Serial1.write("AT+PSWD=");
    Serial1.write(pin);
    Serial1.write("\r\n");
    delay(10);
    Serial1.write("AT+RESET\r\n");
    delay(10);
    digitalWrite(BTCONF_PIN, LOW);
}

TERMINAL_COMMAND(btconf, "Bluetooth config")
{
    if (argc != 2) {
        terminal_io()->println("Usage: btconf <name> <pin>");
    } else {
        char *name = argv[0];
        char *pin = argv[1];
        terminal_io()->println("Configuring bluetooth to name:");
        terminal_io()->println(name);
        terminal_io()->println("And pin:");
        terminal_io()->println(pin);

        for (int k=0; k<3; k++) {
            terminal_io()->println("9600");
            Serial1.begin(9600);
            for (int n=0; n<3; n++) bt_conf(name, pin);
            terminal_io()->println("38400");
            
            Serial1.begin(38400);
            for (int n=0; n<3; n++) bt_conf(name, pin);
            terminal_io()->println("57600");
            
            Serial1.begin(57600);
            for (int n=0; n<3; n++) bt_conf(name, pin);
            terminal_io()->println("115200");
            
            Serial1.begin(115200);
            for (int n=0; n<3; n++) bt_conf(name, pin);
            terminal_io()->println("921600");
            
            Serial1.begin(921600);
            for (int n=0; n<3; n++) bt_conf(name, pin);
        }
    }
}
/*

static void goToConf()
{
    digitalWrite(BTCONF_PIN, HIGH);
    delay(100);
    digitalWrite(BTCONF_PIN, LOW);
}

static void bt_conf(char *name, char *pin)
{
    goToConf();
    Serial1.write("AT\r\n");
    delay(10);
    Serial1.write("AT+UART=115200,0,0\r\n");
    delay(10);
    Serial1.write("AT+NAME=");
    Serial1.write(name);
    Serial1.write("\r\n");
    delay(10);
    Serial1.write("AT+PSWD=");
    Serial1.write(pin);
    delay(10);
    Serial1.write("\r\n");
    Serial1.write("AT+RESET\r\n");
    delay(10);
}

TERMINAL_COMMAND(btconf, "Bluetooth config")
{
    if (argc != 2) {
        terminal_io()->println("Usage: btconf <name> <pin>");
    } else {
        char *name = argv[0];
        char *pin = argv[1];
        terminal_io()->println("Configuring bluetooth to name:");
        terminal_io()->println(name);
        terminal_io()->println("And pin:");
        terminal_io()->println(pin);

        for (int k=0; k<3; k++) {
            Serial1.begin(9600);
            for (int n=0; n<3; n++) bt_conf(name, pin);
            Serial1.begin(38400);
            for (int n=0; n<3; n++) bt_conf(name, pin);
            Serial1.begin(57600);
            for (int n=0; n<3; n++) bt_conf(name, pin);
            Serial1.begin(115200);
            for (int n=0; n<3; n++) bt_conf(name, pin);
            Serial1.begin(921600);
            for (int n=0; n<3; n++) bt_conf(name, pin);
        }
    }
}

TERMINAL_COMMAND(btpulse, "BT conf pulse")
{
    goToConf();
}
*/
