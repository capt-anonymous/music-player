#include "WiFiManager.h"
#include <Arduino.h>

WiFiManager::WiFiManager() 
    : _status(WiFiStatus::DISCONNECTED), 
      _rssi(0), 
      _lastStatusCheckMs(0) {
    _ssid = "";
    _password = "";
    _ip = "--";
}

void WiFiManager::begin() {
    loadCredentials();
    if (_ssid.length() > 0) {
        // Log to Serial without printing password for security
        Serial.print("[WiFi] Auto-connecting to saved network: ");
        Serial.println(_ssid);
        connect(_ssid.c_str(), _password.c_str());
    }
}

void WiFiManager::connect(const char* ssid, const char* password) {
    _ssid = ssid;
    _password = password;
    _status = WiFiStatus::CONNECTING;
    
    WiFi.disconnect(true, true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    
    // Save to Preferences (NVS storage)
    saveCredentials(ssid, password);
    
    _lastStatusCheckMs = millis();
}

void WiFiManager::disconnect() {
    WiFi.disconnect(true, true);
    _status = WiFiStatus::DISCONNECTED;
    _ip = "--";
    _rssi = 0;
}

bool WiFiManager::hasSavedCredentials() {
    _prefs.begin("wifi-config", true);
    String savedSSID = _prefs.getString("ssid", "");
    _prefs.end();
    return savedSSID.length() > 0;
}

void WiFiManager::clearSavedCredentials() {
    _prefs.begin("wifi-config", false);
    _prefs.remove("ssid");
    _prefs.remove("pass");
    _prefs.end();
    _ssid = "";
    _password = "";
}

void WiFiManager::saveCredentials(const char* ssid, const char* password) {
    _prefs.begin("wifi-config", false);
    _prefs.putString("ssid", ssid);
    _prefs.putString("pass", password);
    _prefs.end();
}

void WiFiManager::loadCredentials() {
    _prefs.begin("wifi-config", true);
    _ssid = _prefs.getString("ssid", "");
    _password = _prefs.getString("pass", "");
    _prefs.end();
}

void WiFiManager::update() {
    uint32_t currentMs = millis();
    if (currentMs - _lastStatusCheckMs < 1000) {
        return;
    }
    _lastStatusCheckMs = currentMs;
    
    wl_status_t wl = WiFi.status();
    
    if (_status == WiFiStatus::CONNECTING) {
        if (wl == WL_CONNECTED) {
            _status = WiFiStatus::CONNECTED;
            _ip = WiFi.localIP().toString();
            _rssi = WiFi.RSSI();
            Serial.print("[WiFi] Connected! IP: ");
            Serial.println(_ip);
        } else if (wl == WL_CONNECT_FAILED || wl == WL_NO_SSID_AVAIL) {
            _status = WiFiStatus::CONNECT_FAILED;
            Serial.println("[WiFi] Connection failed.");
        }
    } else if (_status == WiFiStatus::CONNECTED) {
        if (wl != WL_CONNECTED) {
            _status = WiFiStatus::DISCONNECTED;
            _ip = "--";
            _rssi = 0;
            Serial.println("[WiFi] Disconnected.");
        } else {
            _rssi = WiFi.RSSI();
        }
    } else if (_status == WiFiStatus::DISCONNECTED) {
        // If we have saved credentials and lost connection, try to auto-reconnect
        if (wl != WL_CONNECTED && _ssid.length() > 0) {
            WiFi.begin(_ssid.c_str(), _password.c_str());
            _status = WiFiStatus::CONNECTING;
            Serial.println("[WiFi] Lost connection. Attempting auto-reconnection...");
        }
    }
}
