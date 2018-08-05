#ifndef DESTRUCTION_CONSOLE_MODBUS_H
#define DESTRUCTION_CONSOLE_MODBUS_H
// clang-format off

#define USE_HOLDING_REGISTERS_ONLY
#include <Arduino.h>
#include <Modbus.h>

extern void modbus_setup();
extern void modbus_loop();
extern void modbus_set(word event, word value);

//////////////// registers of DESTRUCTION_CONSOLE ///////////////////
enum
{
	// The first register starts at address 0
	ACTIONS,      // Always present, used for incoming actions

	// Any registered events, denoted by 'triggered_by_register' in rs485_node of Lua script, 1 and up
	ENTERED_CODE,

	TOTAL_ERRORS     // leave this one, error counter
};
#endif //DESTRUCTION_CONSOLE_MODBUS_H
