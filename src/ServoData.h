#pragma once
#include "ESP32Servo.h"

class ServoData {
private:
    String name;
    int pin;
    int angle;
    int currentAngle;
    Servo servo;

public:
    ServoData(String name, int pin, int angle) : name(name), pin(pin), angle(angle), currentAngle(0) {
        
        servo.attach(pin);
        servo.write(0);
        currentAngle = 0;
    }

    ServoData() {}

    // Getter for name
    String getName() {
        return name;
    }

    // Getter for pin
    int getPin() {
        return pin;
    }

    // Getter for angle
    int getAngle() {
        return angle;
    }

    // Setter for angle
    void setAngle(int angle) {
        this->angle = angle;
        
    }

    // Getter for current angle
    int getCurrentAngle() {
        return currentAngle;
    }

    // Setter for current angle
    void setCurrentAngle(int currentAngle) {
        if (currentAngle < 0) {
            currentAngle = 0;
        } else if (currentAngle > this->getAngle()) {
            currentAngle = this->getAngle();
        }
        this->currentAngle = currentAngle;
        servo.write(currentAngle);
    }

    // Getter for servo object
    Servo& getServo() {
        return servo;
    }
};
