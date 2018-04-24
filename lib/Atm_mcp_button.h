#pragma once

#include <Automaton.h>
#include <Adafruit_MCP23017.h>

class Atm_mcp_button: public Machine {

 public:
  enum { IDLE, PRESSED, RELEASE }; // STATES
  enum { EVT_PRESS, EVT_RELEASE, ELSE }; // EVENTS
  Atm_mcp_button( void ) : Machine() {};
  Atm_mcp_button& begin( Adafruit_MCP23017 pMCP, uint8_t pPin);
  Atm_mcp_button& trace( Stream & stream );
  Atm_mcp_button& trigger( int event );
  int state( void );
  Atm_mcp_button& onPress( Machine& machine, int event = 0 );
  Atm_mcp_button& onPress( atm_cb_push_t callback, int idx = 0 );
  Atm_mcp_button& onRelease( Machine& machine, int event = 0 );
  Atm_mcp_button& onRelease( atm_cb_push_t callback, int idx = 0 );

 private:
  enum { ENT_PRESSED, ENT_RELEASE }; // ACTIONS
  enum { ON_PRESS, ON_RELEASE, CONN_MAX }; // CONNECTORS
  atm_connector connectors[CONN_MAX];
  int event( int id );
  void action( int id );

  Adafruit_MCP23017 mcp;
  uint8_t pin;
};

/*
Automaton::ATML::begin - Automaton Markup Language

<?xml version="1.0" encoding="UTF-8"?>
<machines>
  <machine name="Atm_mcp_button">
    <states>
      <IDLE index="0">
        <EVT_PRESS>PRESSED</EVT_PRESS>
      </IDLE>
      <PRESSED index="1" on_enter="ENT_PRESSED">
        <EVT_RELEASE>RELEASE</EVT_RELEASE>
      </PRESSED>
      <RELEASE index="2" on_enter="ENT_RELEASE">
        <ELSE>IDLE</ELSE>
      </RELEASE>
    </states>
    <events>
      <EVT_PRESS index="0" access="PRIVATE"/>
      <EVT_RELEASE index="1" access="PRIVATE"/>
    </events>
    <connectors>
      <PRESS autostore="0" broadcast="0" dir="PUSH" slots="1"/>
      <RELEASE autostore="0" broadcast="0" dir="PUSH" slots="1"/>
    </connectors>
    <methods>
    </methods>
  </machine>
</machines>

Automaton::ATML::end
*/

