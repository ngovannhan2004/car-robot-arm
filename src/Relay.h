#pragma once
#include "Arduino.h"
class Relay
{
private:
    String name;
    int pin;
    bool state;

public:
    Relay(String name, int pin) : name(name), pin(pin)
    {
     
        pinMode(pin, OUTPUT);
        digitalWrite(pin, LOW);
        state = false;
    }

    Relay() {}

    String getName()
    {
        return name;
    }

    int getPin()
    {
        return pin;
    }

    bool getState()
    {
        return state;
    }

    void setState(bool state)
    {
        state ? digitalWrite(pin, HIGH) : digitalWrite(pin, LOW);
        this->state = state;
    }
};