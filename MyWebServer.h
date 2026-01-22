#ifndef MY_WEB_SERVER_H
#define MY_WEB_SERVER_H

#include <Arduino.h>
#include <WebServer.h>

class SensorManager;
class CalibrationManager;
class DataManager;
class WifiManager;
class WebhookManager;

class MyWebServer {
public:
  MyWebServer(SensorManager* sensor, CalibrationManager* calib, DataManager* data, WifiManager* wifi, WebhookManager* webhook);
  ~MyWebServer();
  
  // Initialize web server
  void begin();
  
  // Handle client connections
  void handleClient();
  
  // Stop server
  void stop();
  
private:
  WebServer* server;
  SensorManager* sensorManager;
  CalibrationManager* calibManager;
  DataManager* dataManager;
  WifiManager* wifiManager;
  WebhookManager* webhookManager;
  
  // Request handlers
  void handleRoot();
  void handleData();
  void handleStatus();
  void handleCalibrate();
  void handleCalibrateDough();
  void handleMeasure();
  void handleOffset();
  void handleResetData();
  void handleResetWifi();
  void handleScanNetworks();
  void handleConnectWiFi();
  void handleGetWebhook();
  void handleSetWebhook();
  void handleTestWebhook();
  void handleGetPresets();
  void handleSavePreset();
  void handlePresetAction();
  void handleNotFound();
  
  // Helper methods
  String jsonResponse(const char* key, const char* value);
};

#endif
