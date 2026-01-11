#include "MyWebServer.h"
#include "WebPages.h"
#include "SensorManager.h"
#include "CalibrationManager.h"
#include "DataManager.h"
#include "WifiManager.h"
#include "config.h"

// Declare external function from doughtracker.ino
extern void resetMeasurementTimer();

MyWebServer::MyWebServer(SensorManager* sensor, CalibrationManager* calib, DataManager* data, WifiManager* wifi)
  : sensorManager(sensor), calibManager(calib), dataManager(data), wifiManager(wifi) {
  server = new WebServer(WEB_SERVER_PORT);
}

MyWebServer::~MyWebServer() {
  if (server) {
    delete server;
    server = nullptr;
  }
}

void MyWebServer::begin() {
  Serial.println("[WebServer] Initializing web server...");
  
  if (!server) {
    Serial.println("[WebServer] ERROR: Server not initialized!");
    return;
  }
  
  // Ensure WiFi stack is initialized (required for WebServer on ESP32)
  if (WiFi.getMode() == WIFI_OFF) {
    Serial.println("[WebServer] WiFi stack not initialized, setting to STA mode...");
    WiFi.mode(WIFI_STA);
    delay(100);
  }
  
  // Setup routes
  server->on("/", HTTP_GET, [this]() { this->handleRoot(); });
  server->on("/data", HTTP_GET, [this]() { this->handleData(); });
  server->on("/status", HTTP_GET, [this]() { this->handleStatus(); });
  server->on("/api/calibrate", HTTP_POST, [this]() { this->handleCalibrate(); });
  server->on("/api/calibrate-dough", HTTP_POST, [this]() { this->handleCalibrateDough(); });
  server->on("/api/measure", HTTP_POST, [this]() { this->handleMeasure(); });
  server->on("/api/offset", HTTP_POST, [this]() { this->handleOffset(); });
  server->on("/api/reset-data", HTTP_POST, [this]() { this->handleResetData(); });
  server->on("/api/reset-wifi", HTTP_POST, [this]() { this->handleResetWifi(); });
  server->on("/api/scan-networks", HTTP_GET, [this]() { this->handleScanNetworks(); });
  server->on("/api/connect-wifi", HTTP_POST, [this]() { this->handleConnectWiFi(); });
  server->onNotFound([this]() { this->handleNotFound(); });
  
  // Small delay to ensure network stack is ready
  delay(100);
  
  server->begin();
  Serial.printf("[WebServer] Web server started on port %d\n", WEB_SERVER_PORT);
}

void MyWebServer::handleClient() {
  if (server) {
    server->handleClient();
  }
}

void MyWebServer::stop() {
  if (server) {
    server->stop();
    Serial.println("[WebServer] Web server stopped");
  }
}

void MyWebServer::handleRoot() {
  Serial.println("[WebServer] GET /");
  server->send(200, "text/html; charset=UTF-8", WebPages::getIndexHTML());
}

void MyWebServer::handleData() {
  Serial.println("[WebServer] GET /data");
  
  String json = "{\"measurements\":[";
  
  for (uint16_t i = 0; i < dataManager->getCount(); i++) {
    DataPoint dp = dataManager->getMeasurement(i);
    
    if (i > 0) json += ",";
    
    // Convert timestamp to 24-hour format
    time_t t = dp.timestamp;
    struct tm *timeinfo = localtime(&t);
    
    char timeBuffer[20];
    strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", timeinfo);
    
    json += "{\"time\":\"";
    json += timeBuffer;
    json += "\",\"timestamp\":";
    json += (unsigned long)dp.timestamp;
    json += ",\"thickness\":";
    json += dp.thickness;
    json += ",\"rise\":";
    json += dp.risePercentage;
    json += "}";
  }
  
  json += "]}";
  server->send(200, "application/json", json);
}

void MyWebServer::handleStatus() {
  Serial.println("[WebServer] GET /status");
  
  String json = "{\"wifiConnected\":";
  json += wifiManager->isConnected() ? "true" : "false";
  json += ",\"ip\":\"";
  json += wifiManager->getLocalIP();
  json += "\",\"ssid\":\"";
  json += wifiManager->getSSID();
  json += "\",\"calibrated\":";
  json += calibManager->isCalibrated() ? "true" : "false";
  json += ",\"hasData\":";
  json += dataManager->hasData() ? "true" : "false";
  json += ",\"dataPoints\":";
  json += dataManager->getCount();
  json += ",\"initialThickness\":";
  json += calibManager->getInitialDoughThickness();
  json += ",\"calibrationTime\":";
  json += calibManager->getCalibrationTime();
  json += "}";
  
  server->send(200, "application/json", json);
}

void MyWebServer::handleCalibrate() {
  Serial.println("[WebServer] POST /api/calibrate");
  
  if (!sensorManager->isInitialized()) {
    server->send(500, "application/json", "{\"error\":\"Sensor not initialized\"}");
    return;
  }
  
  // Take measurement for zero point
  uint16_t distance = sensorManager->getAveragedDistance(5);
  calibManager->setZeroPoint(distance);
  
  String json = "{\"success\":true,\"zeroPoint\":";
  json += distance;
  json += "}";
  
  server->send(200, "application/json", json);
}

void MyWebServer::handleCalibrateDough() {
  Serial.println("[WebServer] POST /api/calibrate-dough");
  
  if (!sensorManager->isInitialized()) {
    server->send(500, "application/json", "{\"error\":\"Sensor not initialized\"}");
    return;
  }
  
  if (!calibManager->isCalibrated()) {
    server->send(400, "application/json", "{\"error\":\"Container not calibrated first\"}");
    return;
  }
  
  // Take measurement for dough height
  uint16_t distance = sensorManager->getAveragedDistance(5);
  calibManager->setDoughHeight(distance);
  uint16_t initialThickness = calibManager->getInitialDoughThickness();
  
  // Reset the measurement timer now that dough is calibrated
  // This ensures the first measurement happens 15 minutes from now
  resetMeasurementTimer();
  
  String json = "{\"success\":true,\"doughHeight\":";
  json += distance;
  json += ",\"initialThickness\":";
  json += initialThickness;
  json += "}";
  
  server->send(200, "application/json", json);
}

void MyWebServer::handleMeasure() {
  Serial.println("[WebServer] POST /api/measure");
  
  if (!calibManager->isCalibrated()) {
    server->send(400, "application/json", "{\"error\":\"Not calibrated\"}");
    return;
  }
  
  if (!sensorManager->isInitialized()) {
    server->send(500, "application/json", "{\"error\":\"Sensor not initialized\"}");
    return;
  }
  
  // Take measurement
  uint16_t distance = sensorManager->getAveragedDistance(5);
  uint16_t thickness = calibManager->calculateDoughThickness(distance);
  
  // Use the initial dough thickness from calibration, not from first measurement
  uint16_t initialThickness = calibManager->getInitialDoughThickness();
  float risePercentage = calibManager->calculateRisePercentage(thickness, initialThickness);
  
  dataManager->addMeasurement(thickness, risePercentage);
  
  String json = "{\"success\":true,\"distance\":";
  json += distance;
  json += ",\"thickness\":";
  json += thickness;
  json += ",\"rise\":";
  json += risePercentage;
  json += "}";
  
  server->send(200, "application/json", json);
}

void MyWebServer::handleOffset() {
  Serial.println("[WebServer] POST /api/offset");
  
  if (!server->hasArg("plain")) {
    server->send(400, "application/json", "{\"error\":\"No data\"}");
    return;
  }
  
  String body = server->arg("plain");
  
  // Parse JSON (simple extraction)
  int offsetPos = body.indexOf("\"offset\":");
  if (offsetPos == -1) {
    server->send(400, "application/json", "{\"error\":\"Invalid JSON\"}");
    return;
  }
  
  int offset = body.substring(offsetPos + 9).toInt();
  calibManager->setOffset(offset);
  
  String json = "{\"success\":true,\"offset\":";
  json += offset;
  json += "}";
  
  server->send(200, "application/json", json);
}

void MyWebServer::handleResetData() {
  Serial.println("[WebServer] POST /api/reset-data");
  
  // Reset measurement data
  dataManager->reset();
  
  // Reset dough height only (preserves zero point calibration for the container)
  // This allows users to start a new bake without recalibrating the empty container
  calibManager->resetDoughHeight();
  
  server->send(200, "application/json", "{\"success\":true}");
}

void MyWebServer::handleResetWifi() {
  Serial.println("[WebServer] POST /api/reset-wifi");
  
  server->send(200, "application/json", "{\"success\":true}");
  delay(100);
  
  wifiManager->resetWiFi();
  delay(1000);
  
  ESP.restart();
}

void MyWebServer::handleScanNetworks() {
  Serial.println("[WebServer] GET /api/scan-networks");
  
  String json = wifiManager->getNetworksJSON();
  server->send(200, "application/json", json);
}

void MyWebServer::handleConnectWiFi() {
  Serial.println("[WebServer] POST /api/connect-wifi");
  
  if (!server->hasArg("plain")) {
    server->send(400, "application/json", "{\"error\":\"No data provided\"}");
    return;
  }
  
  String body = server->arg("plain");
  
  // Parse JSON - simple extraction of ssid and password
  int ssidPos = body.indexOf("\"ssid\":\"");
  int pwdPos = body.indexOf("\"password\":\"");
  
  if (ssidPos == -1 || pwdPos == -1) {
    server->send(400, "application/json", "{\"error\":\"Invalid JSON format\"}");
    return;
  }
  
  // Extract SSID
  int ssidStart = ssidPos + 8;
  int ssidEnd = body.indexOf("\"", ssidStart);
  String ssid = body.substring(ssidStart, ssidEnd);
  
  // Extract password
  int pwdStart = pwdPos + 12;
  int pwdEnd = body.indexOf("\"", pwdStart);
  String password = body.substring(pwdStart, pwdEnd);
  
  Serial.printf("[WebServer] Attempting to connect to: %s\n", ssid.c_str());
  
  // Attempt connection
  bool connected = wifiManager->connectToNetwork(ssid.c_str(), password.c_str());
  
  if (connected) {
    String json = "{\"success\":true,\"ssid\":\"";
    json += ssid;
    json += "\",\"ip\":\"";
    json += wifiManager->getLocalIP();
    json += "\"}";
    server->send(200, "application/json", json);
    
    // Try to setup mDNS after connection
    wifiManager->setupMDNS("dough");
  } else {
    server->send(400, "application/json", "{\"success\":false,\"error\":\"Failed to connect\"}");
  }
}

void MyWebServer::handleNotFound() {
  Serial.println("[WebServer] 404 - Not Found");
  server->send(404, "text/plain", "Not Found");
}

String MyWebServer::jsonResponse(const char* key, const char* value) {
  String json = "{\"";
  json += key;
  json += "\":\"";
  json += value;
  json += "\"}";
  return json;
}
