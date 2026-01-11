# 🍞 Dough Tracker

An IoT sourdough starter monitoring system that tracks dough rise over time using a distance sensor. Built for the Seeed Studio XIAO ESP32C6 with VL53L1X Time-of-Flight sensor.

## Overview

Dough Tracker automatically measures your sourdough starter's height every 15 minutes and visualizes the rise percentage through a web interface. Perfect for monitoring fermentation progress and understanding your starter's behavior.

## Features

- __Automatic Monitoring__: Measurements taken every 15 minutes
- __Web Interface__: Modern, responsive dashboard accessible via WiFi
- __Real-time Visualization__: Chart.js graphs showing rise percentage over time
- __Smart Calibration__: Persistent container calibration that survives power-offs
- __Outlier Filtering__: Median-based filtering removes sensor anomalies (12% deviation threshold)
- __Dual Time Display__: Shows both clock time and elapsed time on charts
- __WiFi Setup__: Easy network configuration through web interface
- __mDNS Support__: Access via `http://dough.local`
- __Data Persistence__: Measurements and calibration stored in non-volatile memory

## Hardware Requirements

- Seeed Studio XIAO ESP32C6
- VL53L1X Time-of-Flight (ToF) Distance Sensor
- I2C connection (SDA: GPIO 22, SCL: GPIO 23)

## Quick Start

1. Upload the code to your XIAO ESP32C6
2. Connect to "DoughTracker" WiFi network (password: 12345678)
3. Configure your home WiFi through the web interface
4. Calibrate empty container
5. Add your starter and calibrate fresh dough
6. Monitor rise progress!


###DISCLAIMER###
This program has been coded, in full, using Claude in Cline. I have no experience in coding, but wanted a program to monitor the progress of the fermentation of my dough
