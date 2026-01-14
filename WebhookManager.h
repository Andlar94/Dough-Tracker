#ifndef WEBHOOK_MANAGER_H
#define WEBHOOK_MANAGER_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <Preferences.h>

class WebhookManager {
public:
  WebhookManager();

  // Initialize - load settings from NVS
  void begin();

  // Set Discord webhook URL
  void setWebhookURL(const String& url);

  // Get current webhook URL
  String getWebhookURL();

  // Check if webhook is configured
  bool isConfigured();

  // Check thresholds and send notifications if needed
  void checkAndNotify(float currentRisePercentage);

  // Reset threshold flags (called when starting new fermentation)
  void resetThresholds();

  // Enable/disable webhook notifications
  void setEnabled(bool enabled);
  bool isEnabled();

  // Get threshold status for UI display
  bool isThreshold50Reached();
  bool isThreshold100Reached();
  bool isThreshold200Reached();

  // Send a test notification
  bool sendTestNotification();

private:
  Preferences preferences;
  String webhookURL = "";
  bool enabled = true;

  // Threshold flags - track which thresholds have been crossed
  bool threshold50Reached = false;
  bool threshold100Reached = false;
  bool threshold200Reached = false;

  // Last rise percentage to detect direction
  float lastRisePercentage = 0.0;

  // Send Discord webhook notification
  bool sendDiscordMessage(const String& message);

  // NVS persistent storage methods
  void loadFromNVS();
  void saveToNVS();
  void saveThresholdsToNVS();
  void loadThresholdsFromNVS();
};

#endif
