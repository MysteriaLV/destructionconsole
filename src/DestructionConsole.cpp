#include <Arduino.h>
#include "destruction_console_modbus.h"
#include "Atm_mcp_input.h"
#include "freeMemory.h"

#define MCP_IN_COUNT 4

// IN: 5:12 - OUT: 2:5

Atm_mcp_input mcp_input[MCP_IN_COUNT];

Atm_led led, door, alarm_lights, backlight;
Adafruit_MCP23017 mcp4;
Adafruit_MCP23017 mcp5;
Adafruit_MCP23017 mcp6;
Atm_timer countdown;
uint8_t ledId = 0;

byte button_led_mappings[][16][2] = {
        {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4},/*6*/{0, 5}, {0, 6}, {0, 7}, {0, 8}, {0, 9},/*11*/{0, 10}, {0, 11}, {0, 12}, {0, 13}, {0, 14}, {0, 15}},// 0
        {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4},     {0, 5}, {0, 6}, {0, 7}, {0, 8}, {0, 9},      {0, 10}, {0, 11}, {0, 12}, {0, 13}, {0, 14}, {0, 15}},// 1
        {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {5, 12},    {0, 5}, {0, 6}, {0, 7}, {0, 8}, {0, 9},      {0, 10}, {0, 11}, {0, 12}, {0, 13}, {0, 14}, {0, 15}},// 2
    //    {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 4},     {0, 5}, {0, 6}, {0, 7}, {0, 8}, {0, 9},      {0, 10}, {0, 11}, {0, 12}, {0, 13}, {0, 14}, {0, 15}},// 3
};

Adafruit_MCP23017 mcp_by_idx(byte idx) {
    switch (idx) {
        case 4:
            return mcp4;
        case 5:
            return mcp5;
        case 6:
            return mcp6;
    }
}

void process_button(int idx, int button, int up) {
    byte mcp = button_led_mappings[idx][button][0];
    byte led = button_led_mappings[idx][button][0];
    Serial.println(idx);
    Serial.println(button);
    Serial.println(mcp);
    Serial.println(led);
    mcp_by_idx(mcp).digitalWrite(led, static_cast<uint8_t>(up));
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

//    for (uint8_t i = 0; i < MCP_IN_COUNT; i++) {
//        Serial.println(i);
//        mcp_input[i].begin(i)
//                .onPress(process_button, i)
//                .onRelease(process_button, i);
//    }


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
