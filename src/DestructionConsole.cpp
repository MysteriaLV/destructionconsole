#include <Arduino.h>
#include "relay_box_modbus.h"
#include "Adafruit_MCP23017.h"

Adafruit_MCP23017 mcp0;
Adafruit_MCP23017 mcp1;

void setup() {
	Serial.begin(115200);
	modbus_setup();

	mcp0.begin(0);
	mcp1.begin(1);

	mcp1.pinMode(0, INPUT);
	mcp1.pullUp(0, HIGH);  // turn on a 100K pullup internally

	pinMode(LED_BUILTIN, OUTPUT);  // use the p13 LED as debugging
	mcp0.pinMode(0, OUTPUT);
}

void loop() {
	modbus_loop();
	automaton.run();

	// The LED will 'echo' the button
	digitalWrite(LED_BUILTIN, mcp1.digitalRead(0));
	mcp0.digitalWrite(0, mcp1.digitalRead(0));
}

