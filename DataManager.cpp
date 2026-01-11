#include "DataManager.h"
#include "config.h"
#include <time.h>

DataManager::DataManager() {
}

void DataManager::begin() {
  Serial.println("[DataManager] Initializing data manager...");
  count = 0;
  writeIndex = 0;
  firstMeasurementTime = 0;
  Serial.println("[DataManager] Data manager ready");
}

void DataManager::addMeasurement(uint16_t thickness, float risePercentage) {
  time_t currentTime = time(nullptr);
  
  Serial.printf("[DataManager] Adding measurement: %d mm, Rise: %.1f%%, Timestamp: %ld\n", 
                thickness, risePercentage, currentTime);
  
  // First measurement sets the baseline time
  if (count == 0) {
    firstMeasurementTime = currentTime;
  }
  
  // Add to circular buffer
  buffer[writeIndex].timestamp = currentTime;
  buffer[writeIndex].thickness = thickness;
  buffer[writeIndex].risePercentage = risePercentage;
  
  writeIndex = (writeIndex + 1) % MAX_POINTS;
  
  if (count < MAX_POINTS) {
    count++;
  }
  
  Serial.printf("[DataManager] Total measurements: %d\n", count);
}

uint16_t DataManager::getCount() {
  return count;
}

DataPoint DataManager::getMeasurement(uint16_t index) {
  if (index >= count) {
    DataPoint empty = {0, 0, 0.0};
    return empty;
  }
  
  // Calculate actual buffer index (circular)
  uint16_t bufferIndex = (writeIndex - count + index) % MAX_POINTS;
  return buffer[bufferIndex];
}

String DataManager::getAllMeasurementsJSON() {
  String json = "[";
  
  for (uint16_t i = 0; i < count; i++) {
    DataPoint dp = getMeasurement(i);
    
    if (i > 0) json += ",";
    
    // Convert timestamp to 24-hour format
    time_t t = dp.timestamp;
    struct tm *timeinfo = localtime(&t);
    
    char timeBuffer[20];
    strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", timeinfo);
    
    json += "{\"time\":\"";
    json += timeBuffer;
    json += "\",\"thickness\":";
    json += dp.thickness;
    json += ",\"rise\":";
    json += dp.risePercentage;
    json += "}";
  }
  
  json += "]";
  return json;
}

uint16_t DataManager::getInitialThickness() {
  if (count == 0) return 0;
  return getMeasurement(0).thickness;
}

uint16_t DataManager::getCurrentThickness() {
  if (count == 0) return 0;
  return getMeasurement(count - 1).thickness;
}

float DataManager::getCurrentRisePercentage() {
  if (count == 0) return 0.0;
  return getMeasurement(count - 1).risePercentage;
}

unsigned long DataManager::getElapsedTime() {
  if (count == 0) return 0;
  
  unsigned long now = time(nullptr);
  return now - firstMeasurementTime;
}

void DataManager::reset() {
  Serial.println("[DataManager] Resetting all measurement data...");
  count = 0;
  writeIndex = 0;
  firstMeasurementTime = 0;
  
  // Clear buffer
  for (uint16_t i = 0; i < MAX_POINTS; i++) {
    buffer[i] = {0, 0, 0.0};
  }
  
  Serial.println("[DataManager] Data reset complete");
}

bool DataManager::hasData() {
  return count > 0;
}

unsigned long DataManager::getLastMeasurementTime() {
  if (count == 0) return 0;
  return getMeasurement(count - 1).timestamp;
}

String DataManager::formatTime24H(unsigned long timestamp) {
  time_t t = timestamp;
  struct tm *timeinfo = localtime(&t);
  
  char buffer[20];
  strftime(buffer, sizeof(buffer), "%H:%M:%S", timeinfo);
  return String(buffer);
}
