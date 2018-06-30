#include "Atm_mcp_output.h"

/* Add optional parameters for the state machine to begin()
 * Add extra initialization code
 */

#define NUM_BUTTONS 16

Atm_mcp_output &Atm_mcp_output::begin(uint8_t i2c_addr) {
    // clang-format off
    const static state_t state_table[] PROGMEM = {
            /*               ON_ENTER    ON_LOOP  ON_EXIT  EVT_UPDATE  ELSE */
            /*    IDLE */          -1, ATM_SLEEP, -1, UPDATED, -1,
            /* UPDATED */ ENT_UPDATED, -1, -1, -1, IDLE,
    };
    // clang-format on
    Machine::begin(state_table, ELSE);

    mcp.begin(i2c_addr);
    for (uint8_t i = 0; i < NUM_BUTTONS; i++) {
        mcp.pinMode(i, INPUT);
        mcp.pullUp(i, HIGH);  // turn on a 100K pullup internally
    }

    currentState = 0;
    return *this;
}

/* Add C++ code for each internally handled event (input) 
 * The code must return 1 to trigger the event
 */

int Atm_mcp_output::event(int id) {
    switch (id) {
    }
    return 0;
}

/* Add C++ code for each action
 * This generates the 'output' for the state machine
 */

void Atm_mcp_output::action(int id) {
    switch (id) {
        case ENT_UPDATED:
            mcp.writeGPIOAB(currentState);
            return;
    }
}

/* Optionally override the default trigger() method
 * Control how your machine processes triggers
 */

Atm_mcp_output &Atm_mcp_output::trigger(int event) {
    Machine::trigger(event);
    return *this;
}

/* Optionally override the default state() method
 * Control what the machine returns when another process requests its state
 */

int Atm_mcp_output::state(void) {
    return Machine::state();
}

/* Nothing customizable below this line                          
 ************************************************************************************************
*/

/* Public event methods
 *
 */

Atm_mcp_output &Atm_mcp_output::update(uint16_t newState) {
    if (currentState != newState) {
        currentState = newState;
        trigger(EVT_UPDATE);
    }
    return *this;
}

Atm_mcp_output &Atm_mcp_output::updateBit(uint8_t bit, bool value) {
    bitWrite(currentState, bit, value);
    trigger(EVT_UPDATE);
    return *this;
}

/* State trace method
 * Sets the symbol table and the default logging method for serial monitoring
 */

Atm_mcp_output &Atm_mcp_output::trace(Stream &stream) {
    Machine::setTrace(&stream, atm_serial_debug::trace,
                      "MCP_OUTPUT\0EVT_UPDATE\0ELSE\0IDLE\0UPDATED");
    return *this;
}



