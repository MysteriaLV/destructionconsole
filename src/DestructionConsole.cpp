#include <Arduino.h>
#include "destruction_console_modbus.h"
#include "Atm_mcp_input.h"
#include "Atm_mcp_output.h"
#include "freeMemory.h"

#define MCP_IN_COUNT 4

Atm_mcp_input mcp_input[MCP_IN_COUNT];
//Atm_mcp_output mcp4;
//Atm_mcp_output mcp5;

Atm_led led, door, alarm_lights, backlight;
Adafruit_MCP23017 mcp4;
Adafruit_MCP23017 mcp5;
Adafruit_MCP23017 mcp6;
Atm_timer countdown;
uint8_t ledId = 0;

void setup() {
    Serial.begin(115200);
    Serial.println(freeMemory());

    modbus_setup();
    led.begin(LED_BUILTIN);

    door.begin(12)
            .blink(1000, 10000).start();
    alarm_lights.begin(4)
            .blink(10000, 3000).start();
    backlight.begin(11)
            .blink(1000, 1000).start();

    for (uint8_t i = 0; i < MCP_IN_COUNT; i++) {
        Serial.println(i);
        mcp_input[i].begin(i)
                .onPress(led, Atm_led::EVT_ON)
                .onRelease(led, Atm_led::EVT_OFF);
    }

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

    Serial.println("...setup done");

    for (int i = 1; i < 20; i++) {
        led.toggle();
        delay(50);
    }
}

void loop() {
    modbus_loop();
    automaton.run();
}
