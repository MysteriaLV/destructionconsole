#include "Atm_mcp_input.h"

/* Add optional parameters for the state machine to begin()
 * Add extra initialization code
 */

#define w_bit_is_set(sfr, bit) (_SFR_WORD(sfr) & _BV(bit))
#define w_bit_is_clear(sfr, bit) (!(_SFR_WORD(sfr) & _BV(bit)))

#define NUM_BUTTONS 16

Atm_mcp_input &Atm_mcp_input::begin(uint8_t i2c_addr) {
	// clang-format off
  const static state_t state_table[] PROGMEM = {
    /*             ON_ENTER  ON_LOOP  ON_EXIT  EVT_UPDATED  ELSE */
    /*   IDLE */         -1,      -1,      -1,      UPDATE,   -1,
    /* UPDATE */ ENT_UPDATE,      -1,      -1,          -1, IDLE,
  };
  // clang-format on
	Machine::begin(state_table, ELSE);

	mcp.begin(i2c_addr);
	for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
		mcp.pinMode(i, INPUT);
		mcp.pullUp(i, HIGH);  // turn on a 100K pullup internally
	}

	currentState = mcp.readGPIOAB();
	return *this;
}

/* Add C++ code for each internally handled event (input)
 * The code must return 1 to trigger the event
 */

int Atm_mcp_input::event(int id) {
	switch (id) {
		case EVT_UPDATED:
			return currentState != mcp.readGPIOAB();
	}
	return 0;
}

/* Add C++ code for each action
 * This generates the 'output' for the state machine
 *
 * Available connectors:
 *   push( connectors, ON_PRESS, <sub>, <v>, <up> );
 *   push( connectors, ON_RELEASE, <sub>, <v>, <up> );
 */

void Atm_mcp_input::action(int id) {
	switch (id) {
		case ENT_UPDATE: {
			uint16_t lastState = currentState;
			currentState = mcp.readGPIOAB();

			for (byte i = 0; i < NUM_BUTTONS; i++) {
				if (w_bit_is_clear(lastState, i) && w_bit_is_set(currentState, i)) {
					push(connectors, ON_RELEASE, 0, i, 0);
				}
				if (w_bit_is_set(lastState, i) && w_bit_is_clear(currentState, i))
				{
					push( connectors, ON_PRESS, 0, i, 1);
				}
			}
			return;
		}
	}
}

/* Optionally override the default trigger() method
 * Control how your machine processes triggers
 */

Atm_mcp_input &Atm_mcp_input::trigger(int event) {
	Machine::trigger(event);
	return *this;
}

/* Optionally override the default state() method
 * Control what the machine returns when another process requests its state
 */

int Atm_mcp_input::state(void) {
	return Machine::state();
}

/* Nothing customizable below this line
 ************************************************************************************************
*/

/* Public event methods
 *
 */

/*
 * onPress() push connector variants ( slots 8, autostore 0, broadcast 0 )
 */

Atm_mcp_input &Atm_mcp_input::onPress(Machine &machine, int event) {
	onPush(connectors, ON_PRESS, 0, 8, 1, machine, event);
	return *this;
}

Atm_mcp_input &Atm_mcp_input::onPress(atm_cb_push_t callback, int idx) {
	onPush(connectors, ON_PRESS, 0, 8, 1, callback, idx);
	return *this;
}

Atm_mcp_input &Atm_mcp_input::onPress(int sub, Machine &machine, int event) {
	onPush(connectors, ON_PRESS, sub, 8, 0, machine, event);
	return *this;
}

Atm_mcp_input &Atm_mcp_input::onPress(int sub, atm_cb_push_t callback, int idx) {
	onPush(connectors, ON_PRESS, sub, 8, 0, callback, idx);
	return *this;
}

/*
 * onRelease() push connector variants ( slots 8, autostore 0, broadcast 0 )
 */

Atm_mcp_input &Atm_mcp_input::onRelease(Machine &machine, int event) {
	onPush(connectors, ON_RELEASE, 0, 8, 1, machine, event);
	return *this;
}

Atm_mcp_input &Atm_mcp_input::onRelease(atm_cb_push_t callback, int idx) {
	onPush(connectors, ON_RELEASE, 0, 8, 1, callback, idx);
	return *this;
}

Atm_mcp_input &Atm_mcp_input::onRelease(int sub, Machine &machine, int event) {
	onPush(connectors, ON_RELEASE, sub, 8, 0, machine, event);
	return *this;
}

Atm_mcp_input &Atm_mcp_input::onRelease(int sub, atm_cb_push_t callback, int idx) {
	onPush(connectors, ON_RELEASE, sub, 8, 0, callback, idx);
	return *this;
}

/* State trace method
 * Sets the symbol table and the default logging method for serial monitoring
 */

//Atm_mcp_input &Atm_mcp_input::trace(Stream &stream) {
//	Machine::setTrace(&stream, atm_serial_debug::trace,
//	                  "MCP_INPUT\0EVT_UPDATED\0ELSE\0IDLE\0UPDATE");
//	return *this;
//}
