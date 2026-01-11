#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <Arduino.h>
#include "config.h"

class CalibrationManager;  // Forward declaration

struct DataPoint {
  unsigned long timestamp;    // Unix timestamp in seconds
  uint16_t thickness;         // Dough thickness in mm
  float risePercentage;       // Rise percentage from initial thickness
};

class DataManager {
public:
  DataManager();
  
  // Initialize data manager
  void begin();
  
  // Set calibration manager pointer (for getting calibration time)
  void setCalibrationManager(CalibrationManager* calibMgr);
  
  // Add a new measurement
  void addMeasurement(uint16_t thickness, float risePercentage);
  
  // Get measurement count
  uint16_t getCount();
  
  // Get measurement at index (0 = oldest)
  DataPoint getMeasurement(uint16_t index);
  
  // Get all measurements as JSON string
  String getAllMeasurementsJSON();
  
  // Get initial thickness
  uint16_t getInitialThickness();
  
  // Get current thickness
  uint16_t getCurrentThickness();
  
  // Get current rise percentage
  float getCurrentRisePercentage();
  
  // Get time since first measurement (seconds)
  unsigned long getElapsedTime();
  
  // Reset all data
  void reset();
  
  // Check if has data
  bool hasData();
  
  // Get last measurement time
  unsigned long getLastMeasurementTime();
  
private:
  static const uint16_t MAX_POINTS = MAX_DATA_POINTS;
  DataPoint buffer[MAX_DATA_POINTS];
  uint16_t count = 0;
  uint16_t writeIndex = 0;  // Circular buffer index
  unsigned long firstMeasurementTime = 0;
  CalibrationManager* calibrationMgr = nullptr;
  
  // Helper to format timestamp
  String formatTime24H(unsigned long timestamp);
};

#endif
