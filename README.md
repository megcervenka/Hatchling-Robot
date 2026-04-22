# Turtle Receiver

A lightweight ESP32 “network controller” receiver for using the HURC.  
Provides easy-to-use functions to receive controller input (joystick axes, buttons) over ESP-NOW.

---

## Table of Contents

- [Features](#features)  
- [Hardware Requirements](#hardware-requirements)  
- [Software Requirements](#software-requirements)  
- [Installation](#installation)  
- [Usage](#usage)  
  - [Initialization](#initialization)  
  - [Reading Inputs](#reading-inputs)  
- [API Reference](#api-reference)  
- [Example Sketch](#example-sketch)  

---

## Features

- Receive analog joystick X/Y axes 
- Detect joystick press (Z click)  
- Read digital button states (A, B, X, Y, L, R)  
- Optional debug‑message printing via Serial  
- Uses ESP-NOW for low‑latency wireless input  

---

## Hardware Requirements

- ESP32 development board (any variant)  
- Controller transmitter running matching ESP-NOW code  
- USB cable for programming and power  

---

## Software Requirements

- Arduino IDE 1.8.13+ (or PlatformIO)  
- ESP32 board support installed  
- Libraries:
  - `esp_now`
  - `esp_wifi`
  - `WiFi`

---

## Installation

1. Clone or download this repository into your PlatformIO project's lib folder. 
2. Include `TurtleReceiver.h` in your sketch:

    ```cpp
    #include <TurtleReceiver.h>
    ```

---

## Usage

### Initialization

1. In your `setup()` function, initialize Serial (if using debug) and the receiver:

    ```cpp
    #include <TurtleReceiver.h>

    // Enable debug messages? true = on, false = off  NOTE: this will flood your monitor with messages. Only enable if truly necessary.
    NetController controller(/* displayDebugMessage = */ true);

    void setup() {
      Serial.begin(115200);
      controller.controllerSetup();
    }
    ```

2. Optionally, print the ESP32’s MAC address for pairing:

    ```cpp
    printMacAddress();
    ```

### Reading Inputs

In your `loop()`, poll the controller for input states:

```cpp
void loop() {
  float x = controller.getJoy1X();
  float y = controller.getJoy1Y();
  bool click = controller.getJoy1Z();

  if (controller.getA()) {
    // A button pressed
  }
  // ... similarly for getB(), getX(), getY(), getL(), getR()
}

```

---

## API Reference

### Class: `NetController`

| Method                                    | Description                                                                                  |
|-------------------------------------------|----------------------------------------------------------------------------------------------|
| `NetController(bool displayDebug=false)`  | Constructor. Pass `true` to enable serial debug messages (requires `Serial.begin()` first). Will flood the monitor with messages, use only if necessary. |
| `void controllerSetup()`                  | Initialize ESP‑NOW and Wi‑Fi for receiving controller data. Call once in `setup()`.         |
| `float getJoy1X()`                        | Returns left joystick X-axis in range **[-1.0, 1.0]**.                                       |
| `float getJoy1Y()`                        | Returns left joystick Y-axis in range **[-1.0, 1.0]**.                                       |
| `bool getJoy1Z()`                         | Returns `true` if left joystick is clicked (pressed down), otherwise `false`.               |
| `bool getA()`                             | Returns `true` if the A button (Xbox layout) is pressed.                                |
| `bool getB()`                             | Returns `true` if the B button (Xbox layout) is pressed.                                |
| `bool getX()`                             | Returns `true` if the X button (Xbox layout) is pressed.                             |
| `bool getY()`                             | Returns `true` if the Y button (Xbox layout) is pressed.                                |
| `bool getR()`                             | Returns `true` if the right shoulder button is pressed.                                |
| `bool getL()`                             | Returns `true` if the left shoulder button is pressed.                             |

### Free Functions

| Function                                 | Description                                                           |
|------------------------------------------|-----------------------------------------------------------------------|
| `void printMacAddress()`                 | Prints the ESP32’s station MAC address to the serial monitor.         |

---

## Example Sketch
```cpp
#include <Arduino.h>
#include <TurtleReceiver.h>

NetController controller(); // debug messages off by default

void setup() {
  Serial.begin(115200);
  controller.controllerSetup();
  printMacAddress();
}

void loop() {
  // Read joystick
  float joyX = controller.getJoy1X();
  float joyY = controller.getJoy1Y();
  bool joyClick = controller.getJoy1Z();

  // Print values
  if (joyX != 0 || joyY != 0 || joyClick) {
    Serial.printf("Joy: X=%.2f  Y=%.2f  Click=%d\n", joyX, joyY, joyClick);
  }

  // Buttons
  if (controller.getA()) Serial.println("Button A pressed");
  if (controller.getB()) Serial.println("Button B pressed");
  if (controller.getX()) Serial.println("Button X pressed");
  if (controller.getY()) Serial.println("Button Y pressed");
  if (controller.getL()) Serial.println("Button L pressed");
  if (controller.getR()) Serial.println("Button R pressed");

  delay(50);
}
```

---


