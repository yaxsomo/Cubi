#include "buzzer.h"
#include "voltage.h"
#include <terminal.h>

HardwareTimer           timer1(1);

#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978

// Partitions
struct buzzer_note {
    unsigned int freq;
    unsigned int duration;
};
static struct buzzer_note melody_mario1[] = {
    {NOTE_E7, 83}, {0, 83},
    {NOTE_E7, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_E7, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_C7, 83}, {0, 83},
    {NOTE_E7, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_G7, 83}, {0, 83},
    {0, 83}, {0, 83},
    {0, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_G6, 83}, {0, 83},
    {0, 83}, {0, 83},
    {0, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_C7, 83}, {0, 83},
    {0, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_G6, 83}, {0, 83},
    {0, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_E6, 83}, {0, 83},
    {0, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_A6, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_B6, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_AS6, 83}, {0, 83},
    {NOTE_A6, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_G6, 111}, {0, 111},
    {NOTE_E7, 111}, {0, 111},
    {NOTE_G7, 111}, {0, 111},
    {NOTE_A7, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_F7, 83}, {0, 83},
    {NOTE_G7, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_E7, 83}, {0, 83},
    {0, 83}, {0, 83},{0, 166}, {0, 166},
    {NOTE_C7, 83}, {0, 83},
    {NOTE_D7, 83}, {0, 83},
    {NOTE_B6, 83}, {0, 83},
    {0, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_C7, 83}, {0, 83},
    {0, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_G6, 83}, {0, 83},
    {0, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_E6, 83}, {0, 83},
    {0, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_A6, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_B6, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_AS6, 83}, {0, 83},
    {NOTE_A6, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_G6, 111}, {0, 111},
    {NOTE_E7, 111}, {0, 111},
    {NOTE_G7, 111}, {0, 111},
    {NOTE_A7, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_F7, 83}, {0, 83},
    {NOTE_G7, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_E7, 83}, {0, 83},
    {0, 83}, {0, 83},
    {NOTE_C7, 83}, {0, 83},
    {NOTE_D7, 83}, {0, 83},
    {NOTE_B6, 83}, {0, 83},
    {0, 83}, {0, 83},
    {0, 83}, {0, 83},
    {0, 0}
    };
    
static struct buzzer_note melody_mario2[] = {
    {NOTE_C4, 83}, {0, 83},
    {NOTE_C5, 83}, {0, 83},
    {NOTE_A3, 83}, {0, 83},
    {NOTE_A4, 83}, {0, 83},
    {NOTE_AS3, 83}, {0, 83},
    {NOTE_AS4, 83}, {0, 83},
    {0, 166}, {0, 166},
    {0, 333}, {0, 333},
    {NOTE_C4, 83}, {0, 83},
    {NOTE_C5, 83}, {0, 83},
    {NOTE_A3, 83}, {0, 83},
    {NOTE_A4, 83}, {0, 83},
    {NOTE_AS3, 83}, {0, 83},
    {NOTE_AS4, 83}, {0, 83},
    {0, 166}, {0, 166},
    {0, 333}, {0, 333},
    {NOTE_F3, 83}, {0, 83},
    {NOTE_F4, 83}, {0, 83},
    {NOTE_D3, 83}, {0, 83},
    {NOTE_D4, 83}, {0, 83},
    {NOTE_DS3, 83}, {0, 83},
    {NOTE_DS4, 83}, {0, 83},
    {0, 166}, {0, 166},
    {0, 333}, {0, 333},
    {NOTE_F3, 83}, {0, 83},
    {NOTE_F4, 83}, {0, 83},
    {NOTE_D3, 83}, {0, 83},
    {NOTE_D4, 83}, {0, 83},
    {NOTE_DS3, 83}, {0, 83},
    {NOTE_DS4, 83}, {0, 83},
    {0, 166}, {0, 166},
    {0, 166}, {0, 166},
    {NOTE_DS4, 55}, {0, 55},
    {NOTE_CS4, 55}, {0, 55},
    {NOTE_D4, 55}, {0, 55},
    {NOTE_CS4, 166}, {0, 166},
    {NOTE_DS4, 166}, {0, 166},
    {NOTE_DS4, 166}, {0, 166},
    {NOTE_GS3, 166}, {0, 166},
    {NOTE_G3, 166}, {0, 166},
    {NOTE_CS4, 166}, {0, 166},
    {NOTE_C4, 55}, {0, 55},
    {NOTE_FS4, 55}, {0, 55},
    {NOTE_F4, 55}, {0, 55},
    {NOTE_E3, 55}, {0, 55},
    {NOTE_AS4, 55}, {0, 55},
    {NOTE_A4, 55}, {0, 55},
    {NOTE_GS4, 100}, {0, 100},
    {NOTE_DS4, 100}, {0, 100},
    {NOTE_B3, 100}, {0, 100},
    {NOTE_AS3, 100}, {0, 100},
    {NOTE_A3, 100}, {0, 100},
    {NOTE_GS3, 100}, {0, 100},
    {0, 333}, {0, 333},
    {0, 333}, {0, 333},
    {0, 333}, {0, 333},
    {0, 0}
    };
        

static struct buzzer_note melody_pirate[] = {
    {NOTE_E4, 125}, {0, 125},
    {NOTE_G4, 125}, {0, 125},
    {NOTE_A4, 250}, {0, 250},
    {NOTE_A4, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_A4, 125}, {0, 125},
    {NOTE_B4, 125}, {0, 125},
    {NOTE_C5, 250}, {0, 250},
    {NOTE_C5, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_C5, 125}, {0, 125},
    {NOTE_D5, 125}, {0, 125},
    {NOTE_B4, 250}, {0, 250},
    {NOTE_B4, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_A4, 125}, {0, 125},
    {NOTE_G4, 125}, {0, 125},
    {NOTE_A4, 375}, {0, 375},
    {0, 125}, {0, 125},
    {NOTE_E4, 125}, {0, 125},
    {NOTE_G4, 125}, {0, 125},
    {NOTE_A4, 250}, {0, 250},
    {NOTE_A4, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_A4, 125}, {0, 125},
    {NOTE_B4, 125}, {0, 125},
    {NOTE_C5, 250}, {0, 250},
    {NOTE_C5, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_C5, 125}, {0, 125},
    {NOTE_D5, 125}, {0, 125},
    {NOTE_B4, 250}, {0, 250},
    {NOTE_B4, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_A4, 125}, {0, 125},
    {NOTE_G4, 125}, {0, 125},
    {NOTE_A4, 375}, {0, 375},
    {0, 125}, {0, 125},
    {NOTE_E4, 125}, {0, 125},
    {NOTE_G4, 125}, {0, 125},
    {NOTE_A4, 250}, {0, 250},
    {NOTE_A4, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_A4, 125}, {0, 125},
    {NOTE_C5, 125}, {0, 125},
    {NOTE_D5, 250}, {0, 250},
    {NOTE_D5, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_D5, 125}, {0, 125},
    {NOTE_E5, 125}, {0, 125},
    {NOTE_F5, 250}, {0, 250},
    {NOTE_F5, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_E5, 125}, {0, 125},
    {NOTE_D5, 125}, {0, 125},
    {NOTE_E5, 125}, {0, 125},
    {NOTE_A4, 250}, {0, 250},
    {0, 125}, {0, 125},
    {NOTE_A4, 125}, {0, 125},
    {NOTE_B4, 125}, {0, 125},
    {NOTE_C5, 250}, {0, 250},
    {NOTE_C5, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_D5, 250}, {0, 250},
    {NOTE_E5, 125}, {0, 125},
    {NOTE_A4, 250}, {0, 250},
    {0, 125}, {0, 125},
    {NOTE_A4, 125}, {0, 125},
    {NOTE_C5, 125}, {0, 125},
    {NOTE_B4, 250}, {0, 250},
    {NOTE_B4, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_C5, 125}, {0, 125},
    {NOTE_A4, 125}, {0, 125},
    {NOTE_B4, 375}, {0, 375},
    {0, 375}, {0, 375},
    {NOTE_A4, 250}, {0, 250},
    {NOTE_A4, 125}, {0, 125},
    {NOTE_A4, 125}, {0, 125},
    {NOTE_B4, 125}, {0, 125},
    {NOTE_C5, 250}, {0, 250},
    {NOTE_C5, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_C5, 125}, {0, 125},
    {NOTE_D5, 125}, {0, 125},
    {NOTE_B4, 250}, {0, 250},
    {NOTE_B4, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_A4, 125}, {0, 125},
    {NOTE_G4, 125}, {0, 125},
    {NOTE_A4, 375}, {0, 375},
    {0, 125}, {0, 125},
    {NOTE_E4, 125}, {0, 125},
    {NOTE_G4, 125}, {0, 125},
    {NOTE_A4, 250}, {0, 250},
    {NOTE_A4, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_A4, 125}, {0, 125},
    {NOTE_B4, 125}, {0, 125},
    {NOTE_C5, 250}, {0, 250},
    {NOTE_C5, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_C5, 125}, {0, 125},
    {NOTE_D5, 125}, {0, 125},
    {NOTE_B4, 250}, {0, 250},
    {NOTE_B4, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_A4, 125}, {0, 125},
    {NOTE_G4, 125}, {0, 125},
    {NOTE_A4, 375}, {0, 375},
    {0, 125}, {0, 125},
    {NOTE_E4, 125}, {0, 125},
    {NOTE_G4, 125}, {0, 125},
    {NOTE_A4, 250}, {0, 250},
    {NOTE_A4, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_A4, 125}, {0, 125},
    {NOTE_C5, 125}, {0, 125},
    {NOTE_D5, 250}, {0, 250},
    {NOTE_D5, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_D5, 125}, {0, 125},
    {NOTE_E5, 125}, {0, 125},
    {NOTE_F5, 250}, {0, 250},
    {NOTE_F5, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_E5, 125}, {0, 125},
    {NOTE_D5, 125}, {0, 125},
    {NOTE_E5, 125}, {0, 125},
    {NOTE_A4, 250}, {0, 250},
    {0, 125}, {0, 125},
    {NOTE_A4, 125}, {0, 125},
    {NOTE_B4, 125}, {0, 125},
    {NOTE_C5, 250}, {0, 250},
    {NOTE_C5, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_D5, 250}, {0, 250},
    {NOTE_E5, 125}, {0, 125},
    {NOTE_A4, 250}, {0, 250},
    {0, 125}, {0, 125},
    {NOTE_A4, 125}, {0, 125},
    {NOTE_C5, 125}, {0, 125},
    {NOTE_B4, 250}, {0, 250},
    {NOTE_B4, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_C5, 125}, {0, 125},
    {NOTE_A4, 125}, {0, 125},
    {NOTE_B4, 375}, {0, 375},
    {0, 375}, {0, 375},
    {NOTE_E5, 250}, {0, 250},
    {0, 125}, {0, 125},
    {0, 375}, {0, 375},
    {NOTE_F5, 250}, {0, 250},
    {0, 125}, {0, 125},
    {0, 375}, {0, 375},
    {NOTE_E5, 125}, {0, 125},
    {NOTE_E5, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_G5, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_E5, 125}, {0, 125},
    {NOTE_D5, 125}, {0, 125},
    {0, 125}, {0, 125},
    {0, 375}, {0, 375},
    {NOTE_D5, 250}, {0, 250},
    {0, 125}, {0, 125},
    {0, 375}, {0, 375},
    {NOTE_C5, 250}, {0, 250},
    {0, 125}, {0, 125},
    {0, 375}, {0, 375},
    {NOTE_B4, 125}, {0, 125},
    {NOTE_C5, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_B4, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_A4, 500}, {0, 500},
    {NOTE_E5, 250}, {0, 250},
    {0, 125}, {0, 125},
    {0, 375}, {0, 375},
    {NOTE_F5, 250}, {0, 250},
    {0, 125}, {0, 125},
    {0, 375}, {0, 375},
    {NOTE_E5, 125}, {0, 125},
    {NOTE_E5, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_G5, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_E5, 125}, {0, 125},
    {NOTE_D5, 125}, {0, 125},
    {0, 125}, {0, 125},
    {0, 375}, {0, 375},
    {NOTE_D5, 250}, {0, 250},
    {0, 125}, {0, 125},
    {0, 375}, {0, 375},
    {NOTE_C5, 250}, {0, 250},
    {0, 125}, {0, 125},
    {0, 375}, {0, 375},
    {NOTE_B4, 125}, {0, 125},
    {NOTE_C5, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_B4, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_A4, 500}, {0, 500},
    {0, 0}
    };    
    
static struct buzzer_note melody_boot[] = {
    {NOTE_E4, 125}, {0, 125},
    {NOTE_G4, 125}, {0, 125},
    {NOTE_A4, 250}, {0, 250},
    {NOTE_A4, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_A4, 125}, {0, 125},
    {NOTE_B4, 125}, {0, 125},
    {NOTE_C5, 250}, {0, 250},
    {NOTE_C5, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_C5, 125}, {0, 125},
    {NOTE_D5, 125}, {0, 125},
    {NOTE_B4, 250}, {0, 250},
    {NOTE_B4, 125}, {0, 125},
    {0, 125}, {0, 125},
    {NOTE_A4, 125}, {0, 125},
    {NOTE_G4, 125}, {0, 125},
    {NOTE_A4, 375}, {0, 375},
    {0, 125}, {0, 125},
    {0, 0}
};

static struct buzzer_note melody_boot_old[] = {
    {523, 200/2},
    {659, 350/2},
    {523, 200/2},
    {698, 300/2},
    {659, 160/2},
    {0, 0}
};

static struct buzzer_note melody_alert[] = {
    {2000, 200},
    {200, 200},
    {2000, 200},
    {200, 200},
    {0, 0}
};

static struct buzzer_note melody_alert_fast[] = {
    {2000, 100},
    {200, 100},
    {2000, 100},
    {200, 100},
    {0, 0}
};

static struct buzzer_note melody_warning[] = {
    {800, 200},
    {400, 200},
    {200, 0},
    {200, 400},
    {0, 0}
};

static struct buzzer_note melody_begin[] = {
    {800, 200},
    {0, 200},
    {800, 200},
    {0, 0},
};

static struct buzzer_note melody_custom[] = {
    {0, 0},
    {0, 0}
};

// Status
static struct buzzer_note *melody;
static struct buzzer_note *melody_repeat;
static int melody_st;


void buzzer_init()
{
    melody = NULL;
    pinMode(BUZZER_PIN, PWM);
    pwmWrite(BUZZER_PIN, 0);
}

void buzzer_play_note(int note)
{

    timer1.pause();
    timer1.setPrescaleFactor(72000000 / (note * 100));
    timer1.setOverflow(100);
    
    if (note == 0) {
        pinMode(BUZZER_PIN, OUTPUT);
        digitalWrite(BUZZER_PIN, LOW);
    } else {
        timer1.refresh();
        timer1.resume();
        pinMode(BUZZER_PIN, PWM);
        pwmWrite(BUZZER_PIN, 50);
    }
}

static void buzzer_enter(struct buzzer_note *note)
{
    buzzer_play_note(note->freq);
    melody = note;
    melody_st = millis();

    if (note->freq == 0 && note->duration == 0) {
        if (melody_repeat != NULL) {
            buzzer_enter(melody_repeat);
        } else {
            melody = NULL;
        }
    }
}

void buzzer_play(unsigned int melody_num, bool repeat)
{
    // Avoid playing another melody when there is a battery alert
    if (voltage_error() && melody_num != MELODY_ALERT && melody_num != MELODY_ALERT_FAST) {
        return;
    }
    struct buzzer_note *to_play = NULL;

    if (melody_num == MELODY_BOOT) {
        to_play = &melody_boot[0];
    } else if (melody_num == MELODY_ALERT) {
        to_play = &melody_alert[0];
    } else if (melody_num == MELODY_ALERT_FAST) {
        to_play = &melody_alert_fast[0];
    } else if (melody_num == MELODY_WARNING) {
        to_play = &melody_warning[0];
    } else if (melody_num == MELODY_BEGIN) {
        to_play = &melody_begin[0];
    } else if (melody_num == MELODY_CUSTOM) {
        to_play = &melody_custom[0];
    } else if (melody_num == MELODY_MARIO1) {
        to_play = &melody_mario1[0];
    } else if (melody_num == MELODY_MARIO2) {
        to_play = &melody_mario2[0];
    } else if (melody_num == MELODY_PIRATE) {
        to_play = &melody_pirate[0];
    } else {
        melody = NULL;
    }

    if (to_play) {
        melody_repeat = repeat ? to_play : NULL;
        buzzer_enter(to_play);
    }
}

void buzzer_tick()
{
    if (melody != NULL) {
        if (millis()-melody_st > melody->duration) {
            buzzer_enter(melody+1);
        } 
    }
}

void buzzer_stop()
{
    buzzer_play_note(0);
    melody = NULL;
    melody_repeat = NULL;
}

bool buzzer_is_playing()
{
    return melody != NULL;
}

void buzzer_wait_play()
{
    while (buzzer_is_playing()) {
        buzzer_tick();
    }
}


void buzzer_beep(unsigned int freq, unsigned int duration)
{
    melody_custom[0].freq = freq;
    melody_custom[0].duration = duration;
    buzzer_play(MELODY_CUSTOM);
}

#ifdef HAS_TERMINAL
TERMINAL_COMMAND(play, "Play a melody")
{
    int melnum = atoi(argv[0]);
    terminal_io()->print("Playing melody ");
    terminal_io()->print(melnum);
    terminal_io()->println();
    buzzer_play(melnum);
}

TERMINAL_COMMAND(beep, "Plays a beep")
{
    if (argc == 2) {
        buzzer_beep(atoi(argv[0]), atoi(argv[1]));
    } else if (argc == 1) {
        buzzer_beep(atoi(argv[0]), 1000);
    } else {
        terminal_io()->println("Usage: beep freq [duration]");
    }
}

#endif
