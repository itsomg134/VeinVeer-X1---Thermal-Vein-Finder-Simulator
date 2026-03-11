# VeinVeer X1 - Thermal Vein Finder Simulator

[![Arduino Compatible](https://img.shields.io/badge/Arduino-Compatible-brightgreen.svg)](https://www.arduino.cc/)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Status: Prototype](https://img.shields.io/badge/Status-Prototype%20%2F%20Educational-blue.svg)](README.md)

**Find the pulse. Find the path. Find the vein.**

The **VeinVeer X1** is an Arduino-based educational prototype that simulates the core functionality of a medical vein-finding device. It uses a temperature sensor to detect "blood heat"—the slight temperature difference between blood-filled veins and surrounding tissue—and visualizes the results using an LED bar graph, LCD display, and audible feedback.

This project is perfect for learning about sensor integration, data visualization, and simulating real-world medical technology on a budget.

![Concept Image](link-to-your-image-here.jpg) *(Replace with a photo of your built project)*


## Table of Contents

- [The Concept](#-the-concept)
- [How It Works](#-how-it-works)
- [Features](#-features)
- [Hardware Requirements](#-hardware-requirements)
- [Circuit Diagram](#-circuit-diagram)
- [Installation & Setup](#-installation--setup)
- [Usage Guide](#-usage-guide)
- [Code Structure](#-code-structure)
- [Customization](#-customization)
- [Limitations](#-limitations)
- [Future Enhancements](#-future-enhancements)
- [Contributing](#-contributing)
- [License](#-license)


## The Concept

Inspired by advanced medical imaging devices, the VeinVeer X1 demonstrates the principle of **thermal vein detection**. Real vein finders use infrared to visualize the thermal signature of blood. This simulator mimics that by:

1.  Using a temperature sensor as a single-pixel "thermal scanner."
2.  Detecting temperature increases that simulate a warm vein passing under the sensor.
3.  Providing immediate visual and audible feedback to the user.

It is designed for **educational purposes**, hobbyist learning, and as a proof-of-concept for more complex medical technology projects.


## How It Works

The system operates on a continuous loop of sensing and feedback:

1.  **Sensing:** An LM35 or TMP36 temperature sensor reads the surface temperature of an object (e.g., skin) once per second. The code includes a simulation function that periodically adds a slight temperature increase to mimic the "blood heat" of a passing vein.
2.  **Processing:** The Arduino calculates the current temperature and compares it to a pre-set baseline and threshold (`BASELINE_TEMP + BLOOD_HEAT_DELTA`). If the temperature is above the threshold, it is classified as a "vein."
3.  **Visualization (LED Bar):** The temperature is mapped to a 10-LED bar graph. Cooler temperatures light fewer LEDs, while warmer temperatures (simulating a vein) light up more, creating a thermal gradient effect.
4.  **Feedback (LCD):** A 16x2 LCD screen displays the real-time temperature in Celsius and the current system status (e.g., "Scanning...", "VEIN FOUND!").
5.  **Feedback (Audio):** A buzzer provides audible confirmation. A rising double-beep signals a vein detection, while a short low beep signals the loss of a vein.
6.  **User Control:** A button toggles between "Standard" and "Deep Tissue" modes, which amplifies the sensor reading to simulate finding deeper veins.


## Features

- **Real-time Temperature Monitoring:** Continuously reads and displays temperature from an analog sensor.
- **"Blood Heat" Simulation:** Software-based simulation creates a dynamic, scanning-like experience even with a static heat source.
- **10-Segment LED Thermal Bar Graph:** Provides an intuitive, at-a-glance visualization of temperature gradients (cool-to-warm).
- **16x2 LCD Status Display:** Shows precise temperature readings (°C) and operational mode.
- **Audible Vein Confirmation:** Distinctive buzzer sounds indicate when a "vein" (temperature threshold) is detected or lost.
- **Dual Operating Modes:**
    - **Standard Mode:** Normal sensitivity.
    - **Deep Tissue Mode:** Amplifies the signal to simulate detection of veins deeper below the surface.
- **Onboard Calibration Routine:** A simple startup routine averages sensor readings to establish a baseline temperature.
- **Modular Code:** Well-commented C++ code that is easy to understand and modify.


## Hardware Requirements

To build this project, you will need the following components:

| Component | Quantity | Notes |
| :--- | :--- | :--- |
| **Arduino Board** | 1 | Uno, Nano, Mega, or any compatible board. |
| **Temperature Sensor** | 1 | LM35 (simpler) **or** TMP36 (more common). The code supports both. |
| **Standard LCD 1602** | 1 | 16x2 character LCD (with or without I2C). *Note: The provided code uses a direct connection.* |
| **LEDs (5mm)** | 10 | Any colors. A gradient from blue (cool) to red (hot) is recommended for effect. |
| **Resistor (220Ω)** | 10 | For current-limiting the LEDs. |
| **Piezo Buzzer** | 1 | Small, passive or active. |
| **Tactile Push Button** | 1 | For toggling modes. |
| **Breadboard** | 1 | Full-size or half-size for prototyping. |
| **Jumper Wires**| Many | Male-to-male and male-to-female. |
| **Power Supply** | 1 | USB cable for the Arduino or a 9V battery. |

---

## Circuit Diagram

Connect the components as described in the table below. **Ensure you have common ground (GND) for all components.**

### Pin Connections

| Arduino Pin | Connects To | Component Pin / Note |
| :--- | :--- | :--- |
| **5V** | VCC (Power) | Temperature Sensor VCC, LCD VCC (pin 2), LCD LED+ (pin 15 via resistor) |
| **GND** | Ground | Temperature Sensor GND, LCD GND (pin 1), LCD LED- (pin 16), Buzzer GND, Button (other leg) |
| **A0** | Output | Temperature Sensor OUT (LM35/TMP36) |
| **Pin 2** | Signal | Buzzer Positive (Piezo +) |
| **Pin 13** | Signal | Push Button (one leg) - *Uses INPUT_PULLUP* |
| **Pins 3-12** | Anode | 10 x LEDs (Anode) via 220Ω resistors |
| **A1 (LCD RS)** | Control | LCD Pin 4 |
| **A2 (LCD E)** | Control | LCD Pin 6 |
| **A3 (LCD D4)** | Data | LCD Pin 11 |
| **A4 (LCD D5)** | Data | LCD Pin 12 |
| **A5 (LCD D6)** | Data | LCD Pin 13 |
| **A0 (LCD D7)** | Data | LCD Pin 14 |
