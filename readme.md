# ESP32 Controlled Car Project

## Introduction
This project demonstrates how to control a car using an ESP32 microcontroller interfaced with an L298N H-Bridge motor driver to control two DC motors. This setup allows for basic maneuvers such as moving forward, backward, and turning.

## Components Required
- ESP32 Dev Module
- L298N H-Bridge Motor Driver
- 2x DC Motors
- Power Supply (appropriate for your motors and ESP32, typically 12V for the L298N and 3.3V or 5V for the ESP32)
- Jumper Wires
- Breadboard (optional for prototyping)

## Wiring Diagram
**Please insert the path to your wiring diagram image here, such as:**
<!--  -->

### Wiring Instructions

1. **L298N to ESP32 Connections:**
   - `pinLeftEN` (ESP32) → `ENB` (L298N)
   - `pinRightEN` (ESP32) → `ENA` (L298N)
   - `pinLeftIN1` (ESP32) → `IN3` (L298N)
   - `pinLeftIN2` (ESP32) → `IN4` (L298N)
   - `pinRightIN1` (ESP32) → `IN1` (L298N)
   - `pinRightIN2` (ESP32) → `IN2` (L298N)

2. **L298N to DC Motors:**
   - Connect `OUT1` and `OUT2` to the right motor.
   - Connect `OUT3` and `OUT4` to the left motor.

3. **Power Supply:**
   - Connect the power supply to `12V` and `GND` on the L298N.
   - Ensure that the ESP32 is powered safely either via USB or an appropriate external power source.

4. **Common Ground:**
   - Connect the `GND` of the ESP32 to the `GND` of the L298N to complete the common ground circuit.

## Setup and Installation
1. Install the Arduino IDE and configure it for the ESP32 by installing the ESP32 board definitions.
2. Upload the `ControlCar` sketch provided in the `src` directory of this repository.

## Usage
- **Move Forward:** `car.moveForward();`
- **Move Backward:** `car.moveBackward();`
- **Turn Left:** `car.turnLeft();`
- **Turn Right:** `car.turnRight();`
- **Stop:** `car.stop();`

## Customizations
You can adjust the motor speeds by modifying the `setSpeedMove` and `setSpeedTurn` methods. Acceptable values range from 0 (stop) to 255 (maximum speed).

## Troubleshooting
- Check all connections for security and correctness.
- Verify that the power supply is capable of providing sufficient voltage and current without causing damage to the components.
- Ensure that the ESP32 and L298N are not subjected to voltages higher than their specifications.

## Authors
- Nam077

## License
This project is licensed under the MIT License - see the LICENSE.md file for details.
