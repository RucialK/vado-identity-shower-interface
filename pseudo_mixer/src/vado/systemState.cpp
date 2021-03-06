/*
  controllerState.h - Library for packaging controller state
  Created by Russell Keane, December 13, 2018.
  Released into the public domain.
*/

#include "Arduino.h"
#include "systemState.h"

ControllerState::ControllerState()
{
}

ControllerState::ControllerState(bool valid, uint8_t power, int8_t temp, uint8_t flow, uint8_t outlet)
{
    _isValid = valid;

    _power = power;
    _temperature = temp;
    _flow = flow;
    _outlet = outlet;
}

ControllerState::ControllerState(const ControllerState& toCopy)
{
    _isValid = toCopy._isValid;

    _power = toCopy._power;
    _temperature = toCopy._temperature;
    _flow = toCopy._flow;
    _outlet = toCopy._outlet;
}

const bool ControllerState::operator == (const ControllerState& rhs)
{
	return (_isValid == rhs._isValid &&
        _power == rhs._power &&
        _temperature == rhs._temperature &&
        _flow == rhs._flow &&
        _outlet == rhs._outlet);
}