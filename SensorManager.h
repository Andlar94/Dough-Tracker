#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <VL53L1X.h>

class SensorManager {
public:
  SensorManager();
  
  // Initialize sensor
  bool begin();
  
  // Take a single measurement
  uint16_t getDistance();
  
  // Take multiple measurements and return average
  uint16_t getAveragedDistance(uint8_t samples = 5);
  
  // Check if sensor is initialized
  bool isInitialized();
  
  // Get last measurement time
  unsigned long getLastMeasurementTime();
  
private:
  VL53L1X sensor;
  bool initialized = false;
  unsigned long lastMeasurementTime = 0;
};

#endif
