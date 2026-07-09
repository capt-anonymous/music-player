#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>
#include <Preferences.h>

enum class WiFiStatus {
    DISCONNECTED,
    CONNECTING,
    CONNECTED,
    CONNECT_FAILED
};

class WiFiManager {
public:
    WiFiManager();
    void begin();
    void update();
    
    void connect(const char* ssid, const char* password);
    void disconnect();
    
    WiFiStatus getStatus() const { return _status; }
    String getSSID() const { return _ssid; }
    String getIP() const { return _ip; }
    int getRSSI() const { return _rssi; }
    
    bool hasSavedCredentials();
    void clearSavedCredentials();

private:
    WiFiStatus _status;
    String _ssid;
    String _password;
    String _ip;
    int _rssi;
    
    Preferences _prefs;
    uint32_t _lastStatusCheckMs;
    
    void saveCredentials(const char* ssid, const char* password);
    void loadCredentials();
};

#endif
