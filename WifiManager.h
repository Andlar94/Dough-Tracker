#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

class WifiManager {
public:
  WifiManager();
  
  // Initialize WiFi manager
  void begin();
  
  // Start WiFi setup (scanning networks)
  void startSetup();
  
  // Connect to a network
  bool connectToNetwork(const char* ssid, const char* password);
  
  // Try to auto-connect using stored credentials
  bool autoConnect();
  
  // Check if connected
  bool isConnected();
  
  // Get current SSID
  String getSSID();
  
  // Get local IP
  String getLocalIP();
  
  // Setup mDNS
  bool setupMDNS(const char* hostname);
  
  // Get available networks as JSON
  String getNetworksJSON();
  
  // Reset WiFi settings
  void resetWiFi();
  
  // Handle WiFi events
  void handleEvents();
  
  // Start WiFi hotspot (AP mode)
  void startAPMode(const char* ssid, const char* password);
  
  // Check if in AP mode
  bool isAPModeActive();
  
private:
  bool connected = false;
  String currentSSID = "";
  bool apModeActive = false;
  
  // Store credentials in NVS
  void saveCredentials(const char* ssid, const char* password);
  void loadCredentials(String& ssid, String& password);
  void clearCredentials();
};

#endif
