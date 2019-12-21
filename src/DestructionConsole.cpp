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

#define W(idx, button) ((idx)*16 + (button))

byte button_led_mappings[][16] = {
        {W(0, 0), W(0, 1),  W(0, 2),  W(0, 3), W(0, 4),/*5*/W(0, 5),  W(0, 6), W(0, 7), W(0, 8),  W(0, 9),/*10*/W(0, 10), W(0, 11), W(0, 12), W(0, 13), W(0, 14), W(0, 15)},// 0
        {W(4, 6), W(0, 1),  W(0, 2),  W(4, 7), W(0, 4),     W(4, 2),  W(4, 3), W(0, 7), W(4, 12), W(0, 9),      W(4, 5),  W(4, 4),  W(6, 0),  W(0, 13), W(4, 0),  W(6, 0)}, // 1
        {W(0, 0), W(3, 0),  W(5, 13), W(0, 3), W(0, 4),     W(4, 7),  W(6, 1), W(0, 7), W(0, 8),  W(6, 3),      W(0, 10), W(3, 0),  W(0, 12), W(0, 13), W(0, 14), W(0, 15)},// 3
        {W(5, 9), W(5, 10), W(5, 0),  W(5, 8), W(5, 11),    W(5, 12), W(5, 3), W(5, 7), W(0, 8),  W(5, 2),      W(5, 1),  W(0, 11), W(0, 12), W(0, 13), W(5, 5),  W(5, 6)}, // 2

        // Я поменял местами 2 и 3, потому что я на плате реально поменял их местами
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


void process_button(int idx, int button, int up) {
    byte wMapping = button_led_mappings[idx][button];
    byte chip_number = (byte) wMapping / 16;
    byte chip_pin_number = wMapping % 16;
    Serial.print(F("Mapping: "));
    Serial.print(wMapping);
    Serial.print(F(" chip_number: "));
    Serial.print(chip_number);
    Serial.print(F(" chip_pin_number "));
    Serial.println(chip_pin_number);

    if (chip_number < 4 || chip_number > 6) {
        Serial.println(F("Bad chip number!!!"));
        return;
    }

    if (up) {
        uint8_t state = mcp_by_idx(chip_number)->digitalRead(chip_pin_number);
        mcp_by_idx(chip_number)->digitalWrite(chip_pin_number, (uint8_t) !state);
        led.on();
    }
    else
        led.off();
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
void setup() {
    Serial.begin(115200);
    Serial.println(freeMemory());

    modbus_setup();
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

        mcp4.digitalWrite(i, LOW);
        mcp5.digitalWrite(i, LOW);
        mcp6.digitalWrite(i, LOW);
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
