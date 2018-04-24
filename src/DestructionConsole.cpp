#include <Arduino.h>
#include "relay_box_modbus.h"
#include "Atm_mcp_input.h"
#include "freeMemory.h"

#define MCP_COUNT 8

Atm_mcp_input mcp[MCP_COUNT];
Atm_led led;

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
}

void loop() {
	modbus_loop();
	automaton.run();
}
