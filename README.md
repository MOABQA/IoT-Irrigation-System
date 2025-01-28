# **ESP32 Automated Irrigation System** 😎

## Overview

This project utilizes an ESP32 microcontroller to create an IoT-based automated irrigation system, which can be controlled and monitored via a Blynk dashboard. The system measures soil moisture and controls watering based on predefined conditions or manual inputs.

## Blynk Dashboard Interface

- **Motor**: Toggle switch (V1) to turn the irrigation pump on/off.
- **Mode**: Toggle switch (V0) to switch between automatic and manual modes.
- **Soil Moisture**: Gauge (V2) displaying real-time soil moisture levels (0-100%).
- **Watered at**: Displays the last watering time or status (V3).

## Features

- **Real-time Soil Moisture Monitoring**: Updates soil moisture levels every second.
- **Automatic and Manual Control**: Switch between modes via the Blynk app or touch interface.
- **Alerts**: Buzzer for low water tank alerts.

## Setup

1. **Hardware Setup:**
   - ESP32 Development Board
   - Soil Moisture Sensor connected to pin 36
   - Relay Module for pump control on pin 22
   - Buzzer for alerts on pin 19
   - Touch sensors for mode and relay control

2. **Software Setup:**
   - Install ESP32 support in Arduino IDE.
   - Setup Blynk account and create a project.
   - Use the provided code in your Arduino environment.

3. **Blynk Configuration:**
   - Replace `BLYNK_AUTH_TOKEN` with your Blynk Auth Token from the dashboard setup.
   - Ensure your WiFi credentials (`ssid` and `pass`) are correctly entered in the code.

## Usage

- **Manual Mode**: Control the pump directly from the Blynk app or via touch sensors.
- **Automatic Mode**: The system waters based on soil moisture thresholds and scheduled times.

## Code Structure

- `setup()`: Initializes hardware, WiFi, and Blynk connection.
- `loop()`: Runs Blynk, checks WiFi status, manages mode switching, and controls watering.
- `BLYNK_WRITE(V0)` and `BLYNK_WRITE(V1)`: Handle mode and motor control from Blynk app.
- `sendMoistureValue()`: Sends soil moisture data to Blynk.
- `autoWaterPlant()`: Logic for automatic watering.
- Additional functions for touch input handling, time management, and alerts.

## Future Enhancements

- Integration with renewable energy sources.
- Advanced scheduling with predictive analytics for watering.

## License

This project is open-source under the MIT license. Feel free to modify and distribute.
