#include <Arduino.h>
#include <SoftwareSerial.h>

#include "editline.h"
#include "cmdproc.h"

#include "pm1006.h"

#define PIN_PM1006_RX  D2
#define PIN_PM1006_TX  D1
#define PIN_LDR        A0
#define PIN_FAN        D0
#define PIN_LED_G      D5
#define PIN_LED_RO     D3

#define printf Serial.printf

static SoftwareSerial pmSerial(PIN_PM1006_RX, PIN_PM1006_TX);
static PM1006 pm1006(&pmSerial);
static char editline[80];

static void show_help(const cmd_t * cmds)
{
    for (const cmd_t * cmd = cmds; cmd->cmd != NULL; cmd++) {
        printf("%10s: %s\n", cmd->name, cmd->help);
    }
}

static int do_help(int argc, char *argv[]);

static int do_fan(int argc, char *argv[])
{
    if (argc < 2) {
        return -1;
    }
    bool on = (atoi(argv[1]) != 0);
    printf("Turning fan %s\n", on ? "on" : "off");
    digitalWrite(PIN_FAN, on);
    return 0;
}

static int do_ldr(int argc, char *argv[])
{
    int v = analogRead(PIN_LDR);
    printf("LDR value = %d\n", v);
    return 0;
}

static int do_ledg(int argc, char *argv[])
{
    if (argc < 2) {
        return -1;
    }
    bool on = (atoi(argv[1]) != 0);
    printf("Turning green LED %s\n", on ? "on" : "off");
    digitalWrite(PIN_LED_G, !on);
    return 0;
}

static int do_ledr(int argc, char *argv[])
{
    if (argc < 2) {
        return -1;
    }
    int v = atoi(argv[1]);
    printf("Turning red/orange LED %d\n", v);
    switch (v) {
    case 0:
        pinMode(PIN_LED_RO, OUTPUT);
        digitalWrite(PIN_LED_RO, 0);
        break;
    case 1:
        pinMode(PIN_LED_RO, OUTPUT);
        digitalWrite(PIN_LED_RO, 1);
        break;
    default:
        pinMode(PIN_LED_RO, INPUT);
        break;
    }
    return 0;
}


const cmd_t commands[] = {
    { "help", do_help, "Show help" },
    { "fan", do_fan, "<0|1> Turn fan on or off" },
    { "ldr", do_ldr, "Read LDR" },
    { "g", do_ledg, "<0|1> Control green LED" },
    { "r", do_ledr, "<0|1> Control red/orange LED" },
    { NULL, NULL, NULL }
};

static int do_help(int argc, char *argv[])
{
    show_help(commands);
    return CMD_OK;
}

void setup(void)
{
    Serial.begin(115200);
    printf("Hello this is PM600!\n");

    pinMode(PIN_FAN, OUTPUT);
    digitalWrite(PIN_FAN, 0);

    pinMode(PIN_LED_G, OUTPUT);
    digitalWrite(PIN_LED_G, 1);

    EditInit(editline, sizeof(editline));

    pmSerial.begin(PM1006::BIT_RATE);
}

void loop(void)
{
    static int last_tick = -1;

    // try to perform a measurement every 3 seconds
    int tick = millis() / 3000;
    if (tick != last_tick) {
        last_tick = tick;

        printf("Attempting measurement:\n");
        uint16_t pm2_5;
        if (pm1006.read_pm25(&pm2_5)) {
            printf("PM2.5 = %u\n", pm2_5);
        } else {
            printf("Measurement failed!\n");
        }
    }
    // parse command line
    bool haveLine = false;
    if (Serial.available()) {
        char c;
        haveLine = EditLine(Serial.read(), &c);
        Serial.write(c);
    }
    if (haveLine) {
        int result = cmd_process(commands, editline);
        switch (result) {
        case CMD_OK:
            printf("OK\n");
            break;
        case CMD_NO_CMD:
            break;
        case CMD_UNKNOWN:
            printf("Unknown command, available commands:\n");
            show_help(commands);
            break;
        default:
            printf("%d\n", result);
            break;
        }
        printf(">");
    }
}
