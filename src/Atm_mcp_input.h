#pragma once

#include <Automaton.h>
#include <Adafruit_MCP23017.h>

class Atm_mcp_input: public Machine {

 public:
  enum { IDLE, UPDATE }; // STATES
  enum { EVT_UPDATED, ELSE }; // EVENTS
  Atm_mcp_input( void ) : Machine() {};
  Atm_mcp_input& begin( uint8_t i2c_addr );
//  Atm_mcp_input& trace( Stream & stream );
  Atm_mcp_input& trigger( int event );
  int state( void );
  Atm_mcp_input& onPress( Machine& machine, int event = 0 );
  Atm_mcp_input& onPress( atm_cb_push_t callback, int idx = 0 );
  Atm_mcp_input& onPress( int sub, Machine& machine, int event = 0 );
  Atm_mcp_input& onPress( int sub, atm_cb_push_t callback, int idx = 0 );
  Atm_mcp_input& onRelease( Machine& machine, int event = 0 );
  Atm_mcp_input& onRelease( atm_cb_push_t callback, int idx = 0 );
  Atm_mcp_input& onRelease( int sub, Machine& machine, int event = 0 );
  Atm_mcp_input& onRelease( int sub, atm_cb_push_t callback, int idx = 0 );

 private:
  enum { ENT_UPDATE }; // ACTIONS
  enum { ON_PRESS, ON_RELEASE = 8, CONN_MAX = 16 }; // CONNECTORS
  atm_connector connectors[CONN_MAX];
  int event( int id ); 
  void action( int id );

  Adafruit_MCP23017 mcp;
  uint16_t currentState;
};

/*
Automaton::ATML::begin - Automaton Markup Language

<?xml version="1.0" encoding="UTF-8"?>
<machines>
  <machine name="Atm_mcp_input">
    <states>
      <IDLE index="0">
        <EVT_UPDATED>UPDATE</EVT_UPDATED>
      </IDLE>
      <UPDATE index="1" on_enter="ENT_UPDATE">
        <ELSE>IDLE</ELSE>
      </UPDATE>
    </states>
    <events>
      <EVT_UPDATED index="0" access="PRIVATE"/>
    </events>
    <connectors>
      <PRESS autostore="0" broadcast="0" dir="PUSH" slots="8"/>
      <RELEASE autostore="0" broadcast="0" dir="PUSH" slots="8"/>
    </connectors>
    <methods>
    </methods>
  </machine>
</machines>

Automaton::ATML::end
*/

