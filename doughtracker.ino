/*
 * Sourdough Rise Tracker
 * 
 * Hardware: Seeed Studio XIAO ESP32C6 with VL53L1X ToF Sensor
 * 
 * Features:
 * - WiFi connectivity with mDNS (dough.local)
 * - VL53L1X sensor for dough height tracking
 * - Web interface with Chart.js visualization
 * - Automatic measurements every 15 minutes
 * - Persistent calibration and offset storage
 * - Serial debugging
 */

#include "config.h"
#include "SensorManager.h"
#include "CalibrationManager.h"
#include "DataManager.h"
#include "WifiManager.h"
#include "MyWebServer.h"
#include "WebhookManager.h"
#include <time.h>

// Global instances
SensorManager sensorMgr;
CalibrationManager calibMgr;
DataManager dataMgr;
WifiManager wifiMgr;
WebhookManager webhookMgr;
MyWebServer webServer(&sensorMgr, &calibMgr, &dataMgr, &wifiMgr, &webhookMgr);

// Timing variables
unsigned long lastMeasurementTime = 0;
unsigned long measurementInterval = MEASUREMENT_INTERVAL;
bool calibrated = false;

// Function prototypes
void setup();
void loop();
void performMeasurement();
void handleWiFiSetup();
void printStatus();
void resetMeasurementTimer();

void setup() {
  // Initialize serial
  Serial.begin(SERIAL_BAUD);
  delay(1000);
  
  Serial.println("\n\n===========================================");
  Serial.println("  DOUGH TRACKER v0.1");
  Serial.println("===========================================");
  Serial.printf("[SETUP] XIAO ESP32C6 - VL53L1X Sensor\n");
  Serial.printf("[SETUP] I2C Pins: SDA=%d, SCL=%d\n", I2C_SDA, I2C_SCL);
  Serial.printf("[SETUP] Measurement interval: %lu ms\n", MEASUREMENT_INTERVAL);
  
  // Initialize sensor
  Serial.println("\n[SETUP] Initializing sensor...");
  if (!sensorMgr.begin()) {
    Serial.println("[ERROR] Failed to initialize sensor! Halting.");
    while (1) {
      delay(100);
    }
  }
  
  // Initialize calibration manager
  Serial.println("[SETUP] Loading calibration...");
  calibMgr.begin();
  
  // Initialize data manager
  Serial.println("[SETUP] Initializing data manager...");
  dataMgr.begin();

  // Link data manager to calibration manager so it can use calibration time
  dataMgr.setCalibrationManager(&calibMgr);

  // Initialize webhook manager
  Serial.println("[SETUP] Initializing webhook manager...");
  webhookMgr.begin();
  
  // Initialize WiFi manager
  Serial.println("\n[SETUP] Initializing WiFi...");
  wifiMgr.begin();
  
  if (wifiMgr.isConnected()) {
    Serial.printf("[SETUP] WiFi connected: %s\n", wifiMgr.getSSID().c_str());
    Serial.printf("[SETUP] IP address: %s\n", wifiMgr.getLocalIP().c_str());
    
    // Synchronize time with NTP server (with timeout)
    Serial.println("[SETUP] Synchronizing time with NTP server...");
    configTime(1 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    time_t now = time(nullptr);
    int ntpAttempts = 0;
    const int NTP_TIMEOUT_ATTEMPTS = 100;  // 10 seconds max
    while (now < 24 * 3600 && ntpAttempts < NTP_TIMEOUT_ATTEMPTS) {
      delay(100);
      now = time(nullptr);
      ntpAttempts++;
    }
    if (now < 24 * 3600) {
      Serial.println("[SETUP] WARNING: NTP sync timed out - timestamps may be incorrect");
    } else {
      Serial.printf("[SETUP] Time synchronized: %s\n", ctime(&now));
    }
    
    // Setup mDNS
    if (wifiMgr.setupMDNS(MDNS_HOSTNAME)) {
      Serial.printf("[SETUP] Access device at: http://%s.local\n", MDNS_HOSTNAME);
    }
  } else {
    Serial.println("[SETUP] WiFi not connected - starting AP mode for setup");
    // Start WiFi hotspot so user can connect and configure
    wifiMgr.startAPMode("DoughTracker", "12345678");
    Serial.println("[SETUP] Connect to 'DoughTracker' WiFi and access http://192.168.4.1 to configure");
  }
  
  // Initialize web server
  Serial.println("\n[SETUP] Starting web server...");
  webServer.begin();
  
  // Print current status
  Serial.println("\n[SETUP] Setup complete!");
  printStatus();
  
  lastMeasurementTime = millis();
}

void loop() {
  // Handle WiFi events
  wifiMgr.handleEvents();
  
  // Handle web server clients
  webServer.handleClient();
  
  // Check if measurement time has come
  unsigned long currentTime = millis();
  if (currentTime - lastMeasurementTime >= measurementInterval) {
    performMeasurement();
    lastMeasurementTime = currentTime;
  }
  
  delay(10);  // Small delay to avoid overwhelming the chip
}

void performMeasurement() {
  Serial.println("\n=== MEASUREMENT CYCLE ===");
  Serial.printf("[MEASURE] Time: %lu ms\n", millis());

  // Check if container is calibrated (zero point set)
  if (!calibMgr.isCalibrated()) {
    Serial.println("[MEASURE] WARNING: Container not calibrated!");
    Serial.println("[MEASURE] Please access web interface and click 'Calibrate Zero'");
    return;
  }

  // Get initial thickness from calibration data (zeroPoint - doughHeight)
  uint16_t initialThickness = calibMgr.getInitialDoughThickness();
  if (initialThickness == 0) {
    Serial.println("[MEASURE] WARNING: Dough not calibrated!");
    Serial.println("[MEASURE] Please access web interface and click 'Calibrate Dough'");
    return;
  }

  // Take sensor measurement
  uint16_t distance = sensorMgr.getAveragedDistance(SAMPLES_PER_MEASUREMENT);

  if (distance == 0) {
    Serial.println("[MEASURE] ERROR: Failed to get distance from sensor!");
    return;
  }

  // Calculate dough thickness
  uint16_t thickness = calibMgr.calculateDoughThickness(distance);

  // Calculate rise percentage using calibrated initial thickness
  float risePercentage = calibMgr.calculateRisePercentage(thickness, initialThickness);

  // Add to data manager
  dataMgr.addMeasurement(thickness, risePercentage);

  // Check webhook thresholds and notify if needed
  webhookMgr.checkAndNotify(risePercentage);

  // Print summary
  Serial.printf("[MEASURE] Distance: %d mm\n", distance);
  Serial.printf("[MEASURE] Thickness: %d mm\n", thickness);
  Serial.printf("[MEASURE] Initial: %d mm (from calibration)\n", initialThickness);
  Serial.printf("[MEASURE] Rise: %.1f%%\n", risePercentage);
  Serial.printf("[MEASURE] Total measurements: %d\n", dataMgr.getCount());

  printStatus();
}

void printStatus() {
  Serial.println("\n--- CURRENT STATUS ---");
  
  // WiFi status
  if (wifiMgr.isConnected()) {
    Serial.printf("WiFi: CONNECTED (%s)\n", wifiMgr.getSSID().c_str());
    Serial.printf("IP: %s\n", wifiMgr.getLocalIP().c_str());
  } else {
    Serial.println("WiFi: DISCONNECTED");
  }
  
  // Calibration status
  Serial.printf("Calibration: %s\n", calibMgr.isCalibrated() ? "YES" : "NEEDED");
  if (calibMgr.isCalibrated()) {
    Serial.printf("  Zero point: %d mm\n", calibMgr.getZeroPoint());
    Serial.printf("  Offset: %d mm\n", calibMgr.getOffset());
  }
  
  // Data status
  Serial.printf("Measurements: %d\n", dataMgr.getCount());
  if (dataMgr.hasData()) {
    Serial.printf("  Current: %d mm (%.1f%%)\n", 
                  dataMgr.getCurrentThickness(), 
                  dataMgr.getCurrentRisePercentage());
    Serial.printf("  Initial: %d mm\n", dataMgr.getInitialThickness());
    
    unsigned long elapsed = dataMgr.getElapsedTime();
    unsigned long hours = elapsed / 3600;
    unsigned long minutes = (elapsed % 3600) / 60;
    Serial.printf("  Elapsed: %lu:%02lu\n", hours, minutes);
  }
  
  Serial.println("---------------------\n");
}

/*
 * SERIAL COMMAND HANDLER (Optional - for debugging)
 * 
 * You can add serial commands here for testing:
 * 'm' - Force measurement
 * 'c' - Calibrate zero
 * 'r' - Reset data
 * 's' - Print status
 * 'w' - Reset WiFi
 * 
 * This can be used to test functionality without web interface
 */

void resetMeasurementTimer() {
  lastMeasurementTime = millis();
  Serial.println("[TIMER] Measurement timer reset - first measurement in 15 minutes");
}

void serialEvent() {
  while (Serial.available()) {
    char cmd = Serial.read();
    
    switch (cmd) {
      case 'm':
      case 'M':
        Serial.println("[SERIAL] Manual measurement triggered");
        performMeasurement();
        break;
        
      case 'c':
      case 'C':
        Serial.println("[SERIAL] Calibrating zero point...");
        if (sensorMgr.isInitialized()) {
          uint16_t distance = sensorMgr.getAveragedDistance(5);
          calibMgr.setZeroPoint(distance);
          Serial.printf("[SERIAL] Zero point set: %d mm\n", distance);
        }
        break;
        
      case 'r':
      case 'R':
        Serial.println("[SERIAL] Resetting data...");
        dataMgr.reset();
        break;
        
      case 's':
      case 'S':
        Serial.println("[SERIAL] Status requested");
        printStatus();
        break;
        
      case 'w':
      case 'W':
        Serial.println("[SERIAL] Resetting WiFi...");
        wifiMgr.resetWiFi();
        delay(1000);
        ESP.restart();
        break;
        
      case 'h':
      case 'H':
      case '?':
        Serial.println("\n[SERIAL] Available commands:");
        Serial.println("  m - Force measurement");
        Serial.println("  c - Calibrate zero");
        Serial.println("  r - Reset data");
        Serial.println("  s - Print status");
        Serial.println("  w - Reset WiFi");
        Serial.println("  h - Show this help\n");
        break;
        
      default:
        if (cmd != '\n' && cmd != '\r') {
          Serial.printf("[SERIAL] Unknown command: '%c'\n", cmd);
          Serial.println("[SERIAL] Type 'h' for help");
        }
    }
  }
}
