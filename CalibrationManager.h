#ifndef CALIBRATION_MANAGER_H
#define CALIBRATION_MANAGER_H

#include <Arduino.h>

class CalibrationManager {
public:
  CalibrationManager();
  
  // Initialize - load calibration from NVS
  void begin();
  
  // Set zero point (distance to top of empty container)
  void setZeroPoint(uint16_t distanceToEmpty);
  
  // Get zero point
  uint16_t getZeroPoint();
  
  // Set dough height (distance to top of fresh dough surface)
  void setDoughHeight(uint16_t distanceToDough);
  
  // Get dough height
  uint16_t getDoughHeight();
  
  // Get initial dough thickness (zeroPoint - doughHeight)
  uint16_t getInitialDoughThickness();
  
  // Get calibration timestamp (when dough was calibrated)
  unsigned long getCalibrationTime();
  
  // Set offset (persistent adjustment)
  void setOffset(int16_t offsetMm);
  
  // Get offset
  int16_t getOffset();
  
  // Calculate actual dough thickness from sensor distance
  // doughThickness = zeroPoint - currentDistance + offset
  uint16_t calculateDoughThickness(uint16_t currentDistance);
  
  // Calculate rise percentage
  float calculateRisePercentage(uint16_t currentThickness, uint16_t initialThickness);
  
  // Check if calibrated
  bool isCalibrated();
  
  // Reset dough height only (preserves zero point and offset)
  // Used when resetting measurement data but keeping container calibration
  void resetDoughHeight();
  
  // Reset all calibration
  void reset();
  
private:
  uint16_t zeroPoint = 0;        // Distance to empty container
  uint16_t doughHeight = 0;      // Distance to top of fresh dough
  int16_t offsetMm = 0;          // User-set offset
  unsigned long calibrationTime = 0;  // Unix timestamp when dough was calibrated
  bool calibrated = false;
  
  // NVS persistent storage helper methods
  void loadFromNVS();
  void saveToNVS();
};

#endif
