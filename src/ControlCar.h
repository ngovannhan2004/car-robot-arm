#pragma once
#include "Arduino.h"

class ControlCar
{

private:
   int pinLeftIN1;
    int pinLeftIN2;
    int pinRightIN1;
    int pinRightIN2;
    int pinLeftEN;
    int pinRightEN;
    int speedMove = 255;
    int speedTurn = 150;
    void setMotorSpeed(int speedLeft, int speedRight)
    {
        analogWrite(pinLeftEN, abs(speedLeft));
        analogWrite(pinRightEN, abs(speedRight));
        digitalWrite(pinLeftIN1, speedLeft >= 0 ? HIGH : LOW);
        digitalWrite(pinLeftIN2, speedLeft <= 0 ? HIGH : LOW);
        digitalWrite(pinRightIN1, speedRight >= 0 ? HIGH : LOW);
        digitalWrite(pinRightIN2, speedRight <= 0 ? HIGH : LOW);
    }

public:
    ControlCar(int pinLeftIN1, int pinLeftIN2, int pinRightIN1, int pinRightIN2, int pinLeftEN, int pinRightEN)
    : pinLeftIN1(pinLeftIN1), pinLeftIN2(pinLeftIN2), pinRightIN1(pinRightIN1), pinRightIN2(pinRightIN2), pinLeftEN(pinLeftEN), pinRightEN(pinRightEN)
    {
        pinMode(pinLeftIN1, OUTPUT);
        pinMode(pinLeftIN2, OUTPUT);
        pinMode(pinRightIN1, OUTPUT);
        pinMode(pinRightIN2, OUTPUT);
        pinMode(pinLeftEN, OUTPUT);
        pinMode(pinRightEN, OUTPUT);
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
