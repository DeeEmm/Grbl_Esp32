#pragma once

#include "Pin.h"
#include "Configuration/Setting.h"

#include <Arduino.h>  // IRAM_ATTR

class ControlPin {
private:
    bool           _value;
    const char     _letter;
    volatile bool& _rtVariable;
    const char*    _legend;

    void IRAM_ATTR handleISR();

public:
    ControlPin(volatile bool& rtVariable, const char* legend, char letter) :
        _value(false), _letter(letter), _rtVariable(rtVariable), _legend(legend) {
        _rtVariable = _value;
    }

    Setting<Pin> _pin;

    void init();
    bool get() { return _value; }

    void report(char* status);

    ~ControlPin();
};
