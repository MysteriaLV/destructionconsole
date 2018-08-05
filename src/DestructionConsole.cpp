#include <Arduino.h>
#include "destruction_console_modbus.h"
#include "Atm_mcp_input.h"
#include "Atm_mcp_output.h"
#include "freeMemory.h"

#define MCP_COUNT 3

Atm_mcp_input mcp[MCP_COUNT];
Atm_mcp_output mcp3;
Atm_mcp_output mcp6;

Atm_led led;
Adafruit_MCP23017 mcp7;
Atm_timer countdown;
bool isEven = false;

void setup() {
    Serial.begin(115200);
    Serial.println(freeMemory());

    modbus_setup();
    led.begin(LED_BUILTIN);

    for (uint8_t i = 0; i < MCP_COUNT; i++) {
        Serial.println(i);
        mcp[i].begin(i)
                .onPress(led, Atm_led::EVT_ON)
                .onRelease(led, Atm_led::EVT_OFF);
    }

    mcp7.begin(7);
    for (uint8_t i = 0; i < 16; i++) {
        mcp7.pinMode(i, OUTPUT);
    }

    mcp3.begin(3);
//            .trace(Serial);
    mcp6.begin(6);

    countdown.begin(1000) // Each step takes 1 second
            .repeat(ATM_COUNTER_OFF) // Set to 70 seconds
            .onTimer([](int idx, int v, int up) {
                if (isEven) {
                    mcp3.update(0xff);
                    mcp6.update(0x0);
                } else {
                    mcp3.update(0x0);
                    mcp6.update(0xff);
                }

                for (uint8_t i = 0; i < 16; i = i + 2) {
                    mcp7.digitalWrite(i, isEven);
                }
                for (uint8_t i = 1; i < 16; i = i + 2) {
                    mcp7.digitalWrite(i, !isEven);
                }
                isEven = !isEven;
                led.toggle();
            })
//            .trace(Serial)
            .start();
}

void loop() {
    modbus_loop();
    automaton.run();
}
