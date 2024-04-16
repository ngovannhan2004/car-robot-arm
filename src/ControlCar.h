#pragma once
#include "Arduino.h"

class ControlCar
{

private:
    int pinIN1;
    int pinIN2;
    int pinIN3;
    int pinIN4;
    int pinENA;
    int pinENB;
    int speedMove = 255;
    int speedTurn = 150;
    void setMotorSpeed(int speedLeft, int speedRight)
    {
        analogWrite(pinENA, abs(speedLeft));
        analogWrite(pinENB, abs(speedRight));
        digitalWrite(pinIN1, speedLeft >= 0 ? HIGH : LOW);
        digitalWrite(pinIN2, speedLeft <= 0 ? HIGH : LOW);
        digitalWrite(pinIN3, speedRight >= 0 ? HIGH : LOW);
        digitalWrite(pinIN4, speedRight <= 0 ? HIGH : LOW);
    }

public:
    ControlCar(int pinIN1, int pinIN2, int pinIN3, int pinIN4, int pinENA, int pinENB) : pinIN1(pinIN1), pinIN2(pinIN2), pinIN3(pinIN3), pinIN4(pinIN4), pinENA(pinENA), pinENB(pinENB)
    {
        pinMode(pinIN1, OUTPUT);
        pinMode(pinIN2, OUTPUT);
        pinMode(pinIN3, OUTPUT);
        pinMode(pinIN4, OUTPUT);
        pinMode(pinENA, OUTPUT);
        pinMode(pinENB, OUTPUT);
    }

    ControlCar() {}

    void moveForward()
    {
        this->setMotorSpeed(this->speedMove, this->speedMove);
    }

    void moveBackward()
    {
        this->setMotorSpeed(-this->speedMove, -this->speedMove);
    }

    void stop()
    {
        this->setMotorSpeed(0, 0);
    }

    void turnLeft()
    {
        this->setMotorSpeed(-this->speedTurn, this->speedTurn);
    }

    void turnRight()
    {
        this->setMotorSpeed(this->speedTurn, -this->speedTurn);
    }

     void turnLeftReverse()
    {
        this->setMotorSpeed(-this->speedTurn, this->speedTurn);
    }

    void turnRightReverse()
    {
        this->setMotorSpeed(this->speedTurn, -this->speedTurn);
    }
    void moveLeft()
    {
        this->setMotorSpeed(0, this->speedTurn);
    }

    void moveRight()
    {
        this->setMotorSpeed(this->speedTurn, 0);
    }

    void setSpeedMove(int speedMove)
    {
        if (speedMove > 255)
        {
            speedMove = 255;
        }
        else if (speedMove < 0)
        {
            speedMove = 0;
        }

        this->speedMove = speedMove;
        Serial.println(speedMove);
    }

    void setSpeedTurn(int speedTurn)
    {
        if (speedTurn > 255)
        {
            speedTurn = 255;
        }
        else if (speedTurn < 0)
        {
            speedTurn = 0;
        }
        Serial.println(speedTurn);
        this->speedTurn = speedTurn;
    }
    int getSpeedMove()
    {
        return this->speedMove;
    }
    int getSpeedTurn()
    {
        return this->speedTurn;
    }
};
