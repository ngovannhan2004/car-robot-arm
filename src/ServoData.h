#pragma once
#include "ESP32Servo.h"

class ServoData {
private:
    String name;
    int pin;
    int angle;
    int currentAngle;
    int standartAngle;
    Servo servo;

public:
    ServoData(String name, int pin, int angle, int standartAngle): name(name), pin(pin), angle(angle), standartAngle(standartAngle) {
        
        servo.attach(pin);
        servo.write(90);
        currentAngle = 90;
    }

    ServoData() {}

    String getName() {
        return name;
    }

    int getPin() {
        return pin;
    }

    int getAngle() {
        return angle;
    }

    void setAngle(int angle) {
        this->angle = angle;
        
    }

    int getCurrentAngle() {
        return currentAngle;
    }

    int getStandartAngle() {
        return standartAngle;
    }

    void setStandartAngle(int standartAngle) {
        if (standartAngle < 0) {
            standartAngle = 0;
        } else if (standartAngle > this->getAngle()) {
            standartAngle = this->getAngle();
        }
        this->standartAngle = standartAngle;
    }

    void setCurrentAngle(int currentAngle) {
        if (currentAngle < 0) {
            currentAngle = 0;
        } else if (currentAngle > this->getAngle()) {
            currentAngle = this->getAngle();
        }
        this->currentAngle = currentAngle;
        servo.write(currentAngle);
    }


    Servo& getServo() {
        return servo;
    }
};
