# 🍞 Dough Tracker

An IoT sourdough starter monitoring system that tracks dough rise over time using a distance sensor. Built for the Seeed Studio XIAO ESP32C6 with VL53L1X Time-of-Flight sensor.



## Overview

Dough Tracker automatically measures your sourdough starter's height every 15 minutes and visualizes the rise percentage through a web interface. Perfect for monitoring fermentation progress and understanding your starter's behavior.

<img width="740" height="853" alt="interface" src="https://github.com/user-attachments/assets/75c7c197-57d4-4ed7-9337-59a0280b5ee8" />
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

- [Seeed Studio XIAO ESP32C6] (https://www.aliexpress.com/item/1005006946131911.html?spm=a2g0o.order_list.order_list_main.16.5aa71802xvDHiz)
- [VL53L1X Time-of-Flight (ToF) Distance Sensor] *important: get the model with lid (https://www.aliexpress.com/item/1005006760524608.html?spm=a2g0o.order_list.order_list_main.23.5aa71802xvDHiz)
- Straight walled container for the dough - The sensor measures rise by height. If the container is not straight walled, a doubling in size will not correspond to a 100% increase if the walls taper, as the program does not measure volume. 
- [3D-printed lid for your container] (https://makerworld.com/en/models/2230938-dough-tracker-track-your-dough-fermentation#profileId-2427211)

## Quick Start

1. Upload the code to your XIAO ESP32C6
2. Connect to "DoughTracker" WiFi network (password: 12345678) using your phone. Click connect anyway if you are prompted that the network may not provide internet connection. 
3. Open a web browser and type 192.168.4.1 - You'll be taken to the device page. Scroll down untill you find WiFi configuration. Scan for networks and connect to your home network. You'll lose connection to the device, but can now access it at http://dough.local, or the new device IP. 
4. Calibrate empty container*
5. Add your starter and calibrate fresh dough
6. Monitor rise progress!
* Important note on calibrating empty container: The reflective index of your container will affect the measurement taken by the sensor. My see-through container gets about a 10mm difference when measuring without a dough. 
  I recommend coating the bottom of the container in a thin layer of dough before calibrating. This way, you calibrate against a surface with the same reflective index.
  
<img width="740" height="853" alt="interface" src="https://github.com/user-attachments/assets/b637c37f-160a-4263-a73c-5a87f77ad0ba" />

## Wiring diagram
The wiring of the device should look like this, and I am sorry, all I had was paint:

In the order of ESP to VL53L1X sensor:
- 3V3->VIN
- GND->GND
- SDA(D4,22)->SDA
- SCL(D5,23)->SCL

<img width="740" height="853" alt="interface" src="https://github.com/user-attachments/assets/b11284e5-dbb7-47ca-ae82-e1fd766c3697" />

I would recommend wiring the ESP first, then placing it in the 3D-printed enclosure and cut the wires to an appropriate length before soldering to the VL53L1X sensor.
After soldering, fit the lens cap over the sensor and press the components into place. It should then look like this, with the lens pointing down into the container: 

<img width="740" height="853" alt="interface" src="https://github.com/user-attachments/assets/3acffe53-0eb2-4f71-a56c-fd0dbd8f24a0" />

###DISCLAIMER###
This program has been coded, in full, using Claude in Cline. I have no experience in coding, but wanted a program to monitor the progress of the fermentation of my dough
