# Hatchling Robot — HURC-Controlled Drive + Conveyor

A PlatformIO / Arduino project for the TURTLE Hatchling program. Drives a
two-wheeled robot with the left/right joysticks on the HURC controller, and
toggles a conveyor motor with the A button.

## Hardware

- uPesy ESP32 Wroom DevKit (on the robot)
- HURC dev2 controller (separate device, flashed with your robot's MAC)
- 2× L298N motor drivers
- 3× TT-motors (2 drive wheels + 1 conveyor)
- 7.2V NiMH battery pack

### Wiring

Power (per the Hatchling Hardware slide):

- 7.2V battery → L298N #1 `12V` + `GND`
- ESP32 `5V` pin ← L298N #1 `5V` pin
- All grounds (battery, both L298Ns, ESP32) tied together — **common ground**
- ⚠️ Never power the ESP32 from USB and `5V` pin at the same time

Pin mapping (see `src/main.cpp`):

| Function | ESP32 pin | L298N pin |
| --- | --- | --- |
| Left motor IN1 / IN2 / ENA | 27 / 26 / 14 | L298N #1 IN1 / IN2 / ENA |
| Right motor IN3 / IN4 / ENB | 25 / 33 / 32 | L298N #1 IN3 / IN4 / ENB |
| Conveyor IN1 / IN2 / ENA | 18 / 19 / 5 | L298N #2 IN1 / IN2 / ENA |

## Controls

| Input | Action |
| --- | --- |
| Left joystick Y | Forward / reverse |
| Right joystick X | Turn left / right |
| A button | Toggle conveyor on/off (press to flip) |

The drive mixing is arcade-style: `left = fwd + turn`, `right = fwd - turn`.
A 15% deadzone ignores tiny stick drift.

## Setup

### 1. Build and flash

Open this folder in VS Code with the **PlatformIO IDE** extension installed,
then:

1. Click the **✓ Build** button (bottom toolbar). Red squiggles in the editor
   are often "ghost errors" from IntelliSense — only the terminal output from
   Build is authoritative.
2. Plug the ESP32 into USB.
3. Click **→ Upload**.
4. Click the **🔌 Serial Monitor** button.

### 2. Pair the controller with this robot

#### What's actually happening

Every ESP32 has a unique 6-byte **MAC address** baked in at the factory — it
looks like `A4:CF:12:AB:CD:EF`. Think of it as a serial number for networking.
Your laptop has one, your phone has one, every Wi-Fi device has one.

The HURC controller and your robot both contain an ESP32, and they talk using
a protocol called **ESP-NOW** — Espressif's peer-to-peer thing. It's fast and
doesn't need a router or a Wi-Fi network, but it has one rule: **the sender
must know the receiver's MAC address before it can send anything**. Like you
can't mail a package without writing the address on it.

So the pairing goes:

1. You upload this code to the robot's ESP32. `printMacAddress()` reads that
   chip's MAC and prints it to the serial monitor.
2. You copy that string (or screenshot it) and hand it to a Hatchling director.
3. The director has the controller's source code. They paste your MAC into a
   line that looks roughly like
   `uint8_t robotMac[] = {0x24, 0x6F, 0x28, 0xAB, 0xCD, 0xEF};` and flash that
   to your specific HURC.
4. Now the controller has your robot's address hardcoded. Joystick input
   beams packets to exactly your MAC. Other robots in the room ignore them;
   yours picks them up.

The `TurtleReceiver` library on the robot side is just listening — it
doesn't care who the controller is, it accepts whatever arrives. That's why
pairing only has to happen on the controller side.

#### Steps

1. With the Serial Monitor open, press the ESP32's **EN** button to reset.
   You'll see a line like:
   ```
   Mac Address: 24:6F:28:AB:CD:EF
   ```
2. Send that MAC to a Hatchling director. They flash it into your HURC
   controller's firmware.
3. Once paired, the MAC doesn't change — the ESP32 keeps the same MAC for
   life unless the chip itself gets swapped. Comment out or delete the
   `printMacAddress()` line in `src/main.cpp` and re-upload.

## Troubleshooting

- **Build fails on `#include <TurtleReceiver.h>`** → move the library files
  from `lib/TurtleReceiver/` to `src/` and rebuild.
- **Upload fails / "failed to connect"** → hold the ESP32's **Boot** button
  while the upload starts, release once it begins writing.
- **Serial Monitor shows garbage** → `monitor_speed` in `platformio.ini` must
  match `Serial.begin()` in code. Both are `115200` here; don't change one
  without the other.
- **Motors spin the wrong way** → swap IN1 and IN2 for that motor (either in
  the wiring or in `src/main.cpp`).
- **Nothing moves but controller is paired** → check that all grounds are
  common, and that the L298N jumpers over the ENA/ENB pins are **removed**
  (the code drives those with PWM).

## Project layout

```
Hatchling-Robot/
├── platformio.ini              PlatformIO board + monitor config
├── src/main.cpp                Drive + conveyor logic
├── lib/TurtleReceiver/         HURC receiver library (vendored)
├── include/                    (empty — for your own headers)
└── test/                       (empty — for unit tests)
```

## References

- [HURC Receiver Documentation](https://www.turtlerobotics.org/hatchling) — all `NetController` getter functions
- [ESP32-WROOM getting started](https://randomnerdtutorials.com/getting-started-with-esp32/)
- [L298N tutorial](https://lastminuteengineers.com/l298n-dc-stepper-driver-arduino-tutorial/)
