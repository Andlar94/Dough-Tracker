#include "CalibrationManager.h"
#include <Preferences.h>
#include <time.h>

static Preferences preferences;

CalibrationManager::CalibrationManager() {
}

void CalibrationManager::begin() {
  Serial.println("[CalibrationManager] Initializing calibration manager...");
  loadFromNVS();
  
  if (isCalibrated()) {
    Serial.printf("[CalibrationManager] Calibration loaded: ZeroPoint=%d mm, Offset=%d mm\n", zeroPoint, offsetMm);
  } else {
    Serial.println("[CalibrationManager] No calibration found - needs calibration");
  }
}

void CalibrationManager::setZeroPoint(uint16_t distanceToEmpty) {
  // Apply offset to the raw distance before setting as zero point
  int16_t adjustedDistance = distanceToEmpty + offsetMm;
  
  // Ensure adjusted distance doesn't go negative
  if (adjustedDistance < 0) {
    adjustedDistance = 0;
  }
  
  Serial.printf("[CalibrationManager] Setting zero point: rawDistance=%d, offset=%d, adjustedZeroPoint=%d mm\n", 
                distanceToEmpty, offsetMm, adjustedDistance);
  
  zeroPoint = adjustedDistance;
  calibrated = true;
  saveToNVS();
}

uint16_t CalibrationManager::getZeroPoint() {
  return zeroPoint;
}

void CalibrationManager::setDoughHeight(uint16_t distanceToDough) {
  // Apply offset to the raw distance before setting as dough height
  int16_t adjustedDistance = distanceToDough + offsetMm;
  
  // Ensure adjusted distance doesn't go negative
  if (adjustedDistance < 0) {
    adjustedDistance = 0;
  }
  
  // Capture the calibration timestamp (when fresh dough is placed)
  calibrationTime = time(nullptr);
  
  Serial.printf("[CalibrationManager] Setting dough height: rawDistance=%d, offset=%d, adjustedHeight=%d mm\n", 
                distanceToDough, offsetMm, adjustedDistance);
  Serial.printf("[CalibrationManager] Calibration timestamp set: %ld\n", calibrationTime);
  
  doughHeight = adjustedDistance;
  saveToNVS();
}

uint16_t CalibrationManager::getDoughHeight() {
  return doughHeight;
}

uint16_t CalibrationManager::getInitialDoughThickness() {
  if (zeroPoint == 0 || doughHeight == 0) {
    return 0;
  }
  uint16_t thickness = zeroPoint - doughHeight;
  Serial.printf("[CalibrationManager] Initial dough thickness: zeroPoint=%d - doughHeight=%d = %d mm\n", 
                zeroPoint, doughHeight, thickness);
  return thickness;
}

unsigned long CalibrationManager::getCalibrationTime() {
  return calibrationTime;
}

void CalibrationManager::setOffset(int16_t offsetMm) {
  Serial.printf("[CalibrationManager] Setting offset to %d mm\n", offsetMm);
  this->offsetMm = offsetMm;
  saveToNVS();
}

int16_t CalibrationManager::getOffset() {
  return offsetMm;
}

uint16_t CalibrationManager::calculateDoughThickness(uint16_t currentDistance) {
  if (!isCalibrated()) {
    Serial.println("[CalibrationManager] WARNING: Not calibrated!");
    return 0;
  }
  
  // First, apply offset to the sensor reading to calibrate it
  // adjustedDistance = currentDistance + offset
  int16_t adjustedDistance = currentDistance + offsetMm;
  
  // Ensure adjusted distance doesn't go negative
  if (adjustedDistance < 0) {
    adjustedDistance = 0;
  }
  
  // Then calculate dough thickness = zero point distance - adjusted distance
  // (if sensor is closer, distance is smaller, so dough is thicker)
  int16_t thickness = zeroPoint - adjustedDistance;
  
  // Ensure we don't return negative values
  if (thickness < 0) {
    thickness = 0;
  }
  
  Serial.printf("[CalibrationManager] Calculating thickness: zeroPoint=%d, rawDistance=%d, offset=%d, adjustedDistance=%d, result=%d mm\n", 
                zeroPoint, currentDistance, offsetMm, adjustedDistance, thickness);
  
  return (uint16_t)thickness;
}

float CalibrationManager::calculateRisePercentage(uint16_t currentThickness, uint16_t initialThickness) {
  if (initialThickness == 0) {
    return 0.0;
  }
  
  return ((float)(currentThickness - initialThickness) / initialThickness) * 100.0;
}

bool CalibrationManager::isCalibrated() {
  return calibrated && zeroPoint > 0;
}

void CalibrationManager::resetDoughHeight() {
  Serial.println("[CalibrationManager] Resetting dough height only (preserving zero point and offset)...");
  doughHeight = 0;
  calibrationTime = 0;
  saveToNVS();
  Serial.println("[CalibrationManager] Dough height reset complete - zero point and offset preserved");
}

void CalibrationManager::reset() {
  Serial.println("[CalibrationManager] Resetting all calibration...");
  zeroPoint = 0;
  doughHeight = 0;
  offsetMm = 0;
  calibrationTime = 0;
  calibrated = false;

  preferences.begin("dough", false);
  preferences.clear();
  preferences.end();

  Serial.println("[CalibrationManager] Calibration reset complete");
}

void CalibrationManager::loadFromNVS() {
  Serial.println("[CalibrationManager] Loading calibration from NVS...");
  preferences.begin("dough", true);  // Read-only

  zeroPoint = preferences.getUShort("zeroPoint", 0);
  doughHeight = preferences.getUShort("doughHeight", 0);
  offsetMm = preferences.getShort("offsetMm", 0);
  calibrationTime = preferences.getULong("calibTime", 0);
  calibrated = (zeroPoint > 0);

  preferences.end();

  Serial.printf("[CalibrationManager] Loaded from NVS: zeroPoint=%d, doughHeight=%d, offset=%d, calibTime=%lu\n",
                zeroPoint, doughHeight, offsetMm, calibrationTime);
}

void CalibrationManager::saveToNVS() {
  Serial.printf("[CalibrationManager] Saving to NVS: zeroPoint=%d, doughHeight=%d, offset=%d, calibTime=%lu\n",
                zeroPoint, doughHeight, offsetMm, calibrationTime);
  preferences.begin("dough", false);  // Read-write

  preferences.putUShort("zeroPoint", zeroPoint);
  preferences.putUShort("doughHeight", doughHeight);
  preferences.putShort("offsetMm", offsetMm);
  preferences.putULong("calibTime", calibrationTime);

  preferences.end();
  Serial.println("[CalibrationManager] Calibration saved to NVS successfully");
}
