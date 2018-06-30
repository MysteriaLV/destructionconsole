#pragma once

#include <Automaton.h>
#include <Adafruit_MCP23017.h>

class Atm_mcp_output: public Machine {

 public:
  enum { IDLE, UPDATED }; // STATES
  enum { EVT_UPDATE, ELSE }; // EVENTS
  Atm_mcp_output( void ) : Machine() {};
  Atm_mcp_output& begin( uint8_t i2c_addr );
  Atm_mcp_output& trace( Stream & stream );
  Atm_mcp_output& trigger( int event );
  int state( void );
  Atm_mcp_output& update( uint16_t newState );

 private:
  enum { ENT_UPDATED }; // ACTIONS
  int event( int id );
  void action( int id );

  Adafruit_MCP23017 mcp;
  uint16_t currentState;

  Atm_mcp_output &updateBit(uint8_t bit, bool value);
};

/*
Automaton::ATML::begin - Automaton Markup Language

<?xml version="1.0" encoding="UTF-8"?>
<machines>
  <machine name="Atm_mcp_output">
    <states>
      <IDLE index="0" sleep="1">
        <EVT_UPDATE>UPDATED</EVT_UPDATE>
      </IDLE>
      <UPDATED index="1" on_enter="ENT_UPDATED">
        <ELSE>IDLE</ELSE>
      </UPDATED>
    </states>
    <events>
      <EVT_UPDATE index="0" access="PUBLIC"/>
    </events>
    <connectors>
    </connectors>
    <methods>
    </methods>
  </machine>
</machines>

Automaton::ATML::end
*/

