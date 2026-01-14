#include "WebhookManager.h"
#include <WiFi.h>

WebhookManager::WebhookManager() {
}

void WebhookManager::begin() {
  Serial.println("[WebhookManager] Initializing...");
  loadFromNVS();
  loadThresholdsFromNVS();

  if (webhookURL.length() > 0) {
    Serial.printf("[WebhookManager] Webhook URL loaded: %s\n", webhookURL.c_str());
  } else {
    Serial.println("[WebhookManager] No webhook URL configured");
  }

  Serial.printf("[WebhookManager] Notifications %s\n", enabled ? "enabled" : "disabled");
  Serial.printf("[WebhookManager] Thresholds - 50%%: %s, 100%%: %s, 200%%: %s\n",
                threshold50Reached ? "reached" : "pending",
                threshold100Reached ? "reached" : "pending",
                threshold200Reached ? "reached" : "pending");
}

void WebhookManager::setWebhookURL(const String& url) {
  webhookURL = url;
  saveToNVS();
  Serial.printf("[WebhookManager] Webhook URL saved: %s\n", url.c_str());
}

String WebhookManager::getWebhookURL() {
  return webhookURL;
}

bool WebhookManager::isConfigured() {
  return webhookURL.length() > 0;
}

void WebhookManager::setEnabled(bool en) {
  enabled = en;
  saveToNVS();
  Serial.printf("[WebhookManager] Notifications %s\n", enabled ? "enabled" : "disabled");
}

bool WebhookManager::isEnabled() {
  return enabled;
}

bool WebhookManager::isThreshold50Reached() {
  return threshold50Reached;
}

bool WebhookManager::isThreshold100Reached() {
  return threshold100Reached;
}

bool WebhookManager::isThreshold200Reached() {
  return threshold200Reached;
}

void WebhookManager::checkAndNotify(float currentRisePercentage) {
  if (!enabled || webhookURL.length() == 0) {
    return;
  }

  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WebhookManager] WiFi not connected, skipping notification");
    return;
  }

  // Only check rising thresholds (not falling)
  if (currentRisePercentage < lastRisePercentage) {
    lastRisePercentage = currentRisePercentage;
    return;
  }

  // Check 200% first (highest threshold)
  if (!threshold200Reached && currentRisePercentage >= 200.0) {
    Serial.println("[WebhookManager] 200% threshold reached, sending notification...");
    if (sendDiscordMessage("Wow, you dough has tripled!")) {
      threshold200Reached = true;
      saveThresholdsToNVS();
      Serial.println("[WebhookManager] 200% threshold notification sent successfully");
    }
  }
  // Check 100%
  else if (!threshold100Reached && currentRisePercentage >= 100.0) {
    Serial.println("[WebhookManager] 100% threshold reached, sending notification...");
    if (sendDiscordMessage("Your dough has risen 100%, it's doubled!")) {
      threshold100Reached = true;
      saveThresholdsToNVS();
      Serial.println("[WebhookManager] 100% threshold notification sent successfully");
    }
  }
  // Check 50%
  else if (!threshold50Reached && currentRisePercentage >= 50.0) {
    Serial.println("[WebhookManager] 50% threshold reached, sending notification...");
    if (sendDiscordMessage("Your dough has risen 50%, it's getting there!")) {
      threshold50Reached = true;
      saveThresholdsToNVS();
      Serial.println("[WebhookManager] 50% threshold notification sent successfully");
    }
  }

  lastRisePercentage = currentRisePercentage;
}

void WebhookManager::resetThresholds() {
  Serial.println("[WebhookManager] Resetting thresholds for new fermentation cycle");
  threshold50Reached = false;
  threshold100Reached = false;
  threshold200Reached = false;
  lastRisePercentage = 0.0;
  saveThresholdsToNVS();
}

bool WebhookManager::sendTestNotification() {
  if (!isConfigured()) {
    Serial.println("[WebhookManager] Cannot send test - webhook not configured");
    return false;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WebhookManager] Cannot send test - WiFi not connected");
    return false;
  }

  Serial.println("[WebhookManager] Sending test notification...");
  return sendDiscordMessage("Test notification from your DoughTracker! 🍞");
}

bool WebhookManager::sendDiscordMessage(const String& message) {
  HTTPClient http;

  http.begin(webhookURL);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(5000);

  // Build JSON payload - escape quotes in message
  String payload = "{\"content\":\"" + message + "\"}";

  int httpCode = http.POST(payload);

  bool success = (httpCode >= 200 && httpCode < 300);

  if (success) {
    Serial.printf("[WebhookManager] Discord notification sent: %s (HTTP %d)\n",
                  message.c_str(), httpCode);
  } else {
    Serial.printf("[WebhookManager] Failed to send notification. HTTP Code: %d\n", httpCode);
    if (httpCode > 0) {
      String response = http.getString();
      Serial.printf("[WebhookManager] Response: %s\n", response.c_str());
    }
  }

  http.end();
  return success;
}

void WebhookManager::loadFromNVS() {
  preferences.begin("webhook", true); // Read-only
  webhookURL = preferences.getString("url", "");
  enabled = preferences.getBool("enabled", true);
  preferences.end();
}

void WebhookManager::saveToNVS() {
  preferences.begin("webhook", false); // Read-write
  preferences.putString("url", webhookURL);
  preferences.putBool("enabled", enabled);
  preferences.end();
}

void WebhookManager::loadThresholdsFromNVS() {
  preferences.begin("webhook", true); // Read-only
  threshold50Reached = preferences.getBool("threshold50", false);
  threshold100Reached = preferences.getBool("threshold100", false);
  threshold200Reached = preferences.getBool("threshold200", false);
  preferences.end();
}

void WebhookManager::saveThresholdsToNVS() {
  preferences.begin("webhook", false); // Read-write
  preferences.putBool("threshold50", threshold50Reached);
  preferences.putBool("threshold100", threshold100Reached);
  preferences.putBool("threshold200", threshold200Reached);
  preferences.end();
}
