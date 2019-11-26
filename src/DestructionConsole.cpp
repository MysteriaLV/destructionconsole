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
uint8_t ledId = 0;

#define W(idx, button) ((idx)*16 + (button))

byte button_led_mappings[][16] = {
        {W(0, 0), W(0, 1), W(0, 2), W(0, 3), W(0, 4),/*5*/W(0, 5), W(0, 6), W(0, 7), W(0, 8), W(0, 9),/*10*/W(0, 10), W(0, 11), W(0, 12), W(0, 13), W(0, 14), W(0, 15)},// 0
        {W(0, 0), W(0, 1), W(0, 2), W(0, 3), W(0, 4),     W(0, 5), W(0, 6), W(0, 7), W(0, 8), W(0, 9),      W(0, 10), W(0, 11), W(0, 12), W(0, 13), W(0, 14), W(0, 15)},// 1
        {W(0, 0), W(0, 1), W(0, 2), W(0, 3), W(0, 4),     W(0, 5), W(0, 6), W(0, 7), W(0, 8), W(0, 9),      W(0, 10), W(0, 11), W(0, 12), W(0, 13), W(0, 14), W(0, 15)},// 2
        {W(0, 0), W(0, 1), W(0, 2), W(0, 3), W(0, 4),     W(5, 12), W(0, 6), W(0, 7), W(0, 8), W(0, 9),      W(0, 10), W(0, 11), W(0, 12), W(0, 13), W(0, 14), W(0, 15)},// 2
};

Adafruit_MCP23017 mcp_by_idx(byte idx) {
    switch (idx) {
        case 4:
            return mcp4;
        case 5:
            return mcp5;
        case 6:
            return mcp6;
        default:
            return mcp4;
    }
}

void process_button(int idx, int button, int up) {
    byte wMapping = button_led_mappings[idx][button];
//    Serial.println(idx);
//    Serial.println(button);
//    Serial.println((byte) wMapping / 16);
//    Serial.println(wMapping % 16);
    mcp_by_idx((byte) wMapping / 16).digitalWrite(wMapping % 16, static_cast<uint8_t>(up));
}


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

        mcp4.digitalWrite(i, HIGH);
        mcp5.digitalWrite(i, HIGH);
        mcp6.digitalWrite(i, HIGH);
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
