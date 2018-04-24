#include "Atm_mcp_button.h"

/* Add optional parameters for the state machine to begin()
 * Add extra initialization code
 */

Atm_mcp_button &Atm_mcp_button::begin(Adafruit_MCP23017 pMCP, uint8_t pPin) {
	// clang-format off
	const static state_t state_table[] PROGMEM = {
	/*               ON_ENTER  ON_LOOP  ON_EXIT  EVT_PRESS  EVT_RELEASE  ELSE */
	/*    IDLE */          -1,      -1,      -1,   PRESSED,          -1,   -1,
	/* PRESSED */ ENT_PRESSED,      -1,      -1,        -1,     RELEASE,   -1,
	/* RELEASE */ ENT_RELEASE,      -1,      -1,        -1,          -1, IDLE,
	};
    // clang-format on
	mcp = pMCP;
	pin = pPin;

	Machine::begin(state_table, ELSE);
	mcp.pinMode(pPin, INPUT);
	mcp.pullUp(pPin, HIGH);  // turn on a 100K pullup internally
	return *this;
}

/* Add C++ code for each internally handled event (input) 
 * The code must return 1 to trigger the event
 */

int Atm_mcp_button::event(int id) {
	switch (id) {
		case EVT_PRESS:
			return !mcp.digitalRead(pin);
		case EVT_RELEASE:
			return mcp.digitalRead(pin);
	}
	return 0;
}

/* Add C++ code for each action
 * This generates the 'output' for the state machine
 *
 * Available connectors:
 *   push( connectors, ON_PRESS, 0, <v>, <up> );
 *   push( connectors, ON_RELEASE, 0, <v>, <up> );
 */

void Atm_mcp_button::action(int id) {
	switch (id) {
		case ENT_PRESSED:
			push( connectors, ON_PRESS, 0, 0, 0 );
			return;
		case ENT_RELEASE:
			push( connectors, ON_RELEASE, 0, 0, 0 );
			return;
	}
}

/* Optionally override the default trigger() method
 * Control how your machine processes triggers
 */

Atm_mcp_button &Atm_mcp_button::trigger(int event) {
	Machine::trigger(event);
	return *this;
}

/* Optionally override the default state() method
 * Control what the machine returns when another process requests its state
 */

int Atm_mcp_button::state(void) {
	return Machine::state();
}

/* Nothing customizable below this line                          
 ************************************************************************************************
*/

/* Public event methods
 *
 */

/*
 * onPress() push connector variants ( slots 1, autostore 0, broadcast 0 )
 */

Atm_mcp_button &Atm_mcp_button::onPress(Machine &machine, int event) {
	onPush(connectors, ON_PRESS, 0, 1, 1, machine, event);
	return *this;
}

Atm_mcp_button &Atm_mcp_button::onPress(atm_cb_push_t callback, int idx) {
	onPush(connectors, ON_PRESS, 0, 1, 1, callback, idx);
	return *this;
}

/*
 * onRelease() push connector variants ( slots 1, autostore 0, broadcast 0 )
 */

Atm_mcp_button &Atm_mcp_button::onRelease(Machine &machine, int event) {
	onPush(connectors, ON_RELEASE, 0, 1, 1, machine, event);
	return *this;
}

Atm_mcp_button &Atm_mcp_button::onRelease(atm_cb_push_t callback, int idx) {
	onPush(connectors, ON_RELEASE, 0, 1, 1, callback, idx);
	return *this;
}

/* State trace method
 * Sets the symbol table and the default logging method for serial monitoring
 */

Atm_mcp_button &Atm_mcp_button::trace(Stream &stream) {
	Machine::setTrace(&stream, atm_serial_debug::trace,
	                  "MCP_BUTTON\0EVT_PRESS\0EVT_RELEASE\0ELSE\0IDLE\0PRESSED\0RELEASE");
	return *this;
}



