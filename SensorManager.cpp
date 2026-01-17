#include "SensorManager.h"
#include "config.h"

SensorManager::SensorManager() {
}

bool SensorManager::begin() {
  Serial.println("[SensorManager] Initializing VL53L1X sensor...");
  
  // Initialize I2C
  Serial.printf("[SensorManager] I2C Pins: SDA=%d, SCL=%d, Frequency=%d Hz\n", I2C_SDA, I2C_SCL, I2C_FREQ);
  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.setClock(I2C_FREQ);
  
  // Give I2C time to stabilize
  delay(100);
  
  // Initialize sensor
  Serial.println("[SensorManager] Attempting to init sensor...");
  if (!sensor.init()) {
    Serial.println("[SensorManager] ERROR: Failed to detect and initialize sensor!");
    Serial.println("[SensorManager] Check I2C connections and sensor address (0x29)");
    return false;
  }
  
  Serial.println("[SensorManager] Sensor detected, configuring...");
  
  // Configure sensor for single-shot measurements
  sensor.setTimeout(SENSOR_TIMEOUT_MS);
  sensor.setDistanceMode(VL53L1X::Medium);  // Medium distance mode for better accuracy
  sensor.setMeasurementTimingBudget(140000);  // 140ms timing budget for better stability
  
  // Give sensor time to apply settings
  delay(100);
  
  initialized = true;
  Serial.println("[SensorManager] Sensor initialized successfully (single-shot mode)");
  return true;
}

uint16_t SensorManager::getDistance() {
  if (!initialized) {
    Serial.println("[SensorManager] WARNING: Sensor not initialized");
    return 0;
  }
  
  // Use single-shot measurement mode (works reliably)
  uint16_t distance = sensor.readRangeSingleMillimeters();
  lastMeasurementTime = millis();
  
  if (sensor.timeoutOccurred()) {
    Serial.println("[SensorManager] ERROR: Sensor timeout occurred!");
    return 0;
  }
  
  return distance;
}

uint16_t SensorManager::getAveragedDistance(uint8_t samples) {
  if (!initialized) {
    Serial.println("[SensorManager] WARNING: Sensor not initialized");
    return 0;
  }

  // Clamp samples to maximum supported to prevent buffer overflow
  const uint8_t MAX_SAMPLES = 10;
  if (samples > MAX_SAMPLES) {
    Serial.printf("[SensorManager] WARNING: samples clamped from %d to %d\n", samples, MAX_SAMPLES);
    samples = MAX_SAMPLES;
  }

  Serial.printf("[SensorManager] Taking %d measurements...\n", samples);
  uint16_t measurements[MAX_SAMPLES];
  uint8_t validSamples = 0;
  
  // Collect valid measurements
  for (uint8_t i = 0; i < samples; i++) {
    uint16_t distance = getDistance();
    
    if (distance > 0) {
      measurements[validSamples] = distance;
      validSamples++;
      Serial.printf("[SensorManager] Sample %d: %d mm\n", i + 1, distance);
    } else {
      Serial.printf("[SensorManager] Sample %d: FAILED\n", i + 1);
    }
    
    delay(100);  // Small delay between measurements
  }
  
  if (validSamples == 0) {
    Serial.println("[SensorManager] ERROR: All measurements failed!");
    return 0;
  }
  
  // Filter outliers using deviation from median
  // Calculate median
  for (uint8_t i = 0; i < validSamples - 1; i++) {
    for (uint8_t j = i + 1; j < validSamples; j++) {
      if (measurements[i] > measurements[j]) {
        uint16_t temp = measurements[i];
        measurements[i] = measurements[j];
        measurements[j] = temp;
      }
    }
  }
  
  uint16_t median = measurements[validSamples / 2];
  
  // Remove outliers: values that deviate more than 12% from median
  uint32_t sum = 0;
  uint8_t acceptedSamples = 0;
  const float deviationThreshold = 0.12;  // 12% deviation threshold
  
  for (uint8_t i = 0; i < validSamples; i++) {
    float deviation = abs((float)measurements[i] - (float)median) / (float)median;
    
    if (deviation <= deviationThreshold) {
      sum += measurements[i];
      acceptedSamples++;
    } else {
      Serial.printf("[SensorManager] Outlier detected: %d mm (deviation: %.1f%%) - REJECTED\n", 
                    measurements[i], deviation * 100);
    }
  }
  
  if (acceptedSamples == 0) {
    // If all samples are outliers, use the median
    Serial.println("[SensorManager] All samples rejected, using median value");
    return median;
  }
  
  uint16_t average = sum / acceptedSamples;
  Serial.printf("[SensorManager] Average distance: %d mm (%d accepted samples after outlier filtering)\n", 
                average, acceptedSamples);
  return average;
}

bool SensorManager::isInitialized() {
  return initialized;
}

unsigned long SensorManager::getLastMeasurementTime() {
  return lastMeasurementTime;
}
