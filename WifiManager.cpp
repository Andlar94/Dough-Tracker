#include "WifiManager.h"
#include <Preferences.h>
#include <ESPmDNS.h>

static Preferences preferences;

WifiManager::WifiManager() {
}

void WifiManager::begin() {
  Serial.println("[WifiManager] Initializing WiFi manager...");
  
  // Try to auto-connect
  if (autoConnect()) {
    connected = true;
    Serial.printf("[WifiManager] Connected to: %s\n", currentSSID.c_str());
  } else {
    Serial.println("[WifiManager] No stored credentials found or connection failed");
    connected = false;
  }
}

void WifiManager::startSetup() {
  Serial.println("[WifiManager] Starting WiFi setup...");
  
  // Scan networks
  Serial.println("[WifiManager] Scanning WiFi networks...");
  int networks = WiFi.scanNetworks();
  
  Serial.printf("[WifiManager] Found %d networks\n", networks);
  for (int i = 0; i < networks; i++) {
    Serial.printf("[WifiManager]   %d. %s (RSSI: %d)\n", i + 1, WiFi.SSID(i).c_str(), WiFi.RSSI(i));
  }
}

bool WifiManager::connectToNetwork(const char* ssid, const char* password) {
  Serial.printf("[WifiManager] Connecting to: %s\n", ssid);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  // Wait for connection (up to 20 seconds)
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 40) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    connected = true;
    currentSSID = ssid;
    saveCredentials(ssid, password);
    
    Serial.printf("\n[WifiManager] Connected successfully!\n");
    Serial.printf("[WifiManager] IP Address: %s\n", WiFi.localIP().toString().c_str());
    
    return true;
  } else {
    Serial.println("\n[WifiManager] Connection failed!");
    connected = false;
    return false;
  }
}

bool WifiManager::autoConnect() {
  String ssid, password;
  loadCredentials(ssid, password);
  
  if (ssid.length() == 0) {
    Serial.println("[WifiManager] No stored credentials found");
    return false;
  }
  
  Serial.printf("[WifiManager] Attempting auto-connect to: %s\n", ssid.c_str());
  
  return connectToNetwork(ssid.c_str(), password.c_str());
}

bool WifiManager::isConnected() {
  return (WiFi.status() == WL_CONNECTED);
}

String WifiManager::getSSID() {
  if (isConnected()) {
    return WiFi.SSID();
  }
  return "Not connected";
}

String WifiManager::getLocalIP() {
  if (isConnected()) {
    return WiFi.localIP().toString();
  }
  return "0.0.0.0";
}

bool WifiManager::setupMDNS(const char* hostname) {
  if (!isConnected()) {
    Serial.println("[WifiManager] Cannot setup mDNS - not connected to WiFi");
    return false;
  }
  
  Serial.printf("[WifiManager] Setting up mDNS hostname: %s.local\n", hostname);
  
  if (!MDNS.begin(hostname)) {
    Serial.println("[WifiManager] ERROR: Failed to start mDNS!");
    return false;
  }
  
  Serial.printf("[WifiManager] mDNS started - access at %s.local\n", hostname);
  return true;
}

String WifiManager::getNetworksJSON() {
  Serial.println("[WifiManager] Scanning networks for JSON...");
  
  String json = "[";
  int networks = WiFi.scanNetworks();
  
  for (int i = 0; i < networks; i++) {
    if (i > 0) json += ",";
    
    json += "{\"ssid\":\"";
    json += WiFi.SSID(i);
    json += "\",\"rssi\":";
    json += WiFi.RSSI(i);
    json += "}";
  }
  
  json += "]";
  return json;
}

void WifiManager::resetWiFi() {
  Serial.println("[WifiManager] Resetting WiFi settings...");
  
  WiFi.disconnect(true);  // Disconnect and turn off WiFi
  clearCredentials();
  
  connected = false;
  currentSSID = "";
  
  Serial.println("[WifiManager] WiFi reset complete");
}

void WifiManager::handleEvents() {
  if (isConnected() && !connected) {
    connected = true;
    Serial.println("[WifiManager] WiFi connected!");
  } else if (!isConnected() && connected) {
    connected = false;
    Serial.println("[WifiManager] WiFi disconnected!");
  }
}

void WifiManager::startAPMode(const char* ssid, const char* password) {
  Serial.printf("[WifiManager] Starting AP mode: %s\n", ssid);
  
  // Stop any existing connection
  WiFi.disconnect(true);
  delay(100);
  
  // Start AP mode
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  
  apModeActive = true;
  
  IPAddress apIP = WiFi.softAPIP();
  Serial.printf("[WifiManager] AP Mode started!\n");
  Serial.printf("[WifiManager] AP SSID: %s\n", ssid);
  Serial.printf("[WifiManager] AP IP: %s\n", apIP.toString().c_str());
  Serial.printf("[WifiManager] Clients can connect and access web interface at: http://%s\n", apIP.toString().c_str());
}

bool WifiManager::isAPModeActive() {
  return apModeActive;
}

void WifiManager::saveCredentials(const char* ssid, const char* password) {
  preferences.begin("wifi", false);
  preferences.putString("ssid", ssid);
  preferences.putString("password", password);
  preferences.end();
  
  Serial.println("[WifiManager] Credentials saved");
}

void WifiManager::loadCredentials(String& ssid, String& password) {
  preferences.begin("wifi", true);
  
  ssid = preferences.getString("ssid", "");
  password = preferences.getString("password", "");
  
  preferences.end();
  
  if (ssid.length() > 0) {
    Serial.printf("[WifiManager] Credentials loaded: SSID=%s\n", ssid.c_str());
  }
}

void WifiManager::clearCredentials() {
  preferences.begin("wifi", false);
  preferences.clear();
  preferences.end();
  
  Serial.println("[WifiManager] Credentials cleared");
}
