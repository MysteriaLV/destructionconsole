#include <Arduino.h>
#include "destruction_console_modbus.h"
#include "Atm_mcp_input.h"
#include "freeMemory.h"

#define MCP_IN_COUNT 4

Atm_mcp_input mcp_input[MCP_IN_COUNT];

Atm_led led, door, alarm_lights, backlight;
Adafruit_MCP23017 mcp4;
Adafruit_MCP23017 mcp5;
Adafruit_MCP23017 mcp6;
Atm_timer countdown;
Atm_step puzzle_controller;

#define W(idx, button) ((idx)*16 + (button))
#define FX 33
#define FX_CTRL W(2, 6)
#define SENHANCE W(2, 8)
#define SHIELD_CONTROL W(2, 5)
#define ENGAGE 47
#define BLASTERS 24
#define BOOMSTICK 26
#define PHASERS 20
#define NUKES 27
#define BTN_SET 42
#define BTN_LEVEL 35
#define PANIC W(1, 0)

byte correct_button_seq[] = {FX, BLASTERS, FX_CTRL, BOOMSTICK, NUKES, PHASERS, SHIELD_CONTROL, ENGAGE, BTN_SET, BTN_LEVEL, PANIC};
byte button_seq_position = 0;

byte button_led_mappings[][16] = {
        {W(0, 0), W(0, 1),  W(0, 2),  W(0, 3), W(0, 4),/*5*/W(0, 5),  W(0, 6), W(0, 7), W(0, 8),  W(0, 9),/*10*/W(0, 10), W(0, 11), W(0, 12), W(0, 13), W(0, 14), W(0, 15)},// 0
        {W(4, 6), W(0, 1),  W(0, 2),  W(4, 7), W(4, 13),    W(4, 2),  W(4, 3), W(0, 7), W(4, 12), W(0, 9),      W(4, 5),  W(4, 4),  W(6, 0),  W(4, 0),  W(0, 14),  W(6, 0)}, // 1
        {W(5, 9), W(5, 10), W(5, 0),  W(5, 8), W(5, 11),    W(5, 12), W(5, 3), W(5, 7), W(0, 8),  W(5, 2),      W(5, 1),  W(0, 11), W(5, 11), W(0, 13), W(5, 5),  W(5, 6)}, // 2
        {W(0, 0), W(3, 0),  W(5, 13), W(0, 3), W(0, 4),     W(4, 7),  W(6, 1), W(0, 7), W(0, 8),  W(6, 3),      W(0, 10), W(3, 0),  W(0, 12), W(0, 13), W(0, 14), W(0, 15)},// 3
};

Adafruit_MCP23017 *mcp_by_idx(byte idx) {
    switch (idx) {
        case 4:
            return &mcp4;
        case 5:
            return &mcp5;
        case 6:
            return &mcp6;
        default:
            return (Adafruit_MCP23017 *) nullptr;
    }
}

void puzzle_init(int idx, int v, int up) {
    modbus_set(ENTERED_CODE, 0);
    door.off();
    alarm_lights.off();
    backlight.off();
    button_seq_position = 0;

    for (uint8_t i = 0; i < 16; i++) {
        mcp4.digitalWrite(i, LOW);
        mcp5.digitalWrite(i, LOW);
        mcp6.digitalWrite(i, LOW);
    }
}

void puzzle_activated(int idx, int v, int up) {
    door.on();
    alarm_lights.off();
    backlight.on();
    button_seq_position = 0;
}

void puzzle_completed(int idx, int v, int up) {
    modbus_set(ENTERED_CODE, 1);
    alarm_lights.on();
    for (uint8_t i = 0; i < 16; i++) {
        mcp4.digitalWrite(i, HIGH);
        mcp5.digitalWrite(i, HIGH);
        mcp6.digitalWrite(i, HIGH);
    }
}

void process_button(int idx, int button, int up) {
    if (puzzle_controller.state() != 1)
        return;

    byte wMapping = button_led_mappings[idx][button];
    byte chip_number = (byte) wMapping / 16;
    byte chip_pin_number = wMapping % 16;
   Serial.print(F("Mapping: "));
   Serial.print(wMapping);
   Serial.print(F(" chip_number: "));
   Serial.print(chip_number);
   Serial.print(F(" chip_pin_number "));
   Serial.println(chip_pin_number);


    if (up) {
        led.on();
        if (chip_number >= 4 && chip_number <= 6) {
            uint8_t state = mcp_by_idx(chip_number)->digitalRead(chip_pin_number);
            mcp_by_idx(chip_number)->digitalWrite(chip_pin_number, (uint8_t) !state);
        } else {
//            Serial.println(F("Bad chip number!!!"));
        }

        int btn_code = W(idx, button);
       Serial.print(F(" btn_code "));
       Serial.println(btn_code);

        if (btn_code == correct_button_seq[button_seq_position]) {
            Serial.print(F("Correctly guessed N"));
            Serial.println(button_seq_position++);
        } else {
            Serial.print(F("Wrong button "));
            Serial.println(btn_code);
            button_seq_position = 0;

            // Verify if it is correct button #0
            if (btn_code == correct_button_seq[button_seq_position]) {
                Serial.print(F("...but guessed N"));
                Serial.println(button_seq_position++);
            }

            if (btn_code == PANIC){
                // Turn off all leds
                for (uint8_t i = 0; i < 16; i++) {
                    mcp4.digitalWrite(i, LOW);
                    mcp5.digitalWrite(i, LOW);
                    mcp6.digitalWrite(i, LOW);
                }
            }
        }

        if (button_seq_position == sizeof(correct_button_seq))
            // advance to completed
            puzzle_controller.trigger(puzzle_controller.EVT_STEP);

    } else
        led.off();
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

void setup() {
    Serial.begin(115200);
    Serial.println(freeMemory());

    led.begin(LED_BUILTIN);

    door.begin(12, false).off();
    alarm_lights.begin(4).off();
    backlight.begin(11).off();

    Serial.println(4);
    mcp4.begin(4);

    Serial.println(5);
    mcp5.begin(5);

    Serial.println(6);
    mcp6.begin(6);

    for (uint8_t i = 0; i < 16; i++) {
        mcp4.pinMode(i, OUTPUT);
        mcp5.pinMode(i, OUTPUT);
        mcp6.pinMode(i, OUTPUT);
    }

    for (uint8_t i = 0; i < MCP_IN_COUNT; i++) {
        Serial.println(i);
        mcp_input[i].begin(i)
                .onPress(process_button, i)
                .onRelease(process_button, i);
    }

#ifdef MY_TEST_MODE
    door.blink(1000, 10000).start();
    alarm_lights.blink(10000, 3000).start();
    backlight.blink(1000, 1000).start();


    for (uint8_t i = 0; i < MCP_IN_COUNT; i++) {
        Serial.println(i);
        mcp_input[i].begin(i)
                .onPress(led, Atm_led::EVT_ON)
                .onRelease(led, Atm_led::EVT_OFF);
    }

    countdown.begin(200) // Each step takes 1 second
            .repeat(ATM_COUNTER_OFF) // Set to 70 seconds
            .onTimer([](int idx, int v, int up) {
                uint8_t newLed = (ledId + 1) % 16;

                mcp4.digitalWrite(newLed, HIGH);
                mcp5.digitalWrite(newLed, HIGH);
                mcp6.digitalWrite(newLed, HIGH);

                mcp4.digitalWrite(ledId, LOW);
                mcp5.digitalWrite(ledId, LOW);
                mcp6.digitalWrite(ledId, LOW);

                ledId = newLed;
//                led.toggle();
            })
            .start();
#endif

    puzzle_controller.begin()
            .onStep(0, puzzle_init)
            .onStep(1, puzzle_activated)
            .onStep(2, puzzle_completed);

    // Start puzzle
    puzzle_controller.trigger(puzzle_controller.EVT_STEP);

    // [debug] advance to activated
    puzzle_controller.trigger(puzzle_controller.EVT_STEP);

    modbus_setup();
    Serial.println(F("...setup done"));

    for (int i = 1; i < 20; i++) {
        led.toggle();
        delay(50);
    }
}

void loop() {
    modbus_loop();
    automaton.run();
}

#pragma clang diagnostic pop
