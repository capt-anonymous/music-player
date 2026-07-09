#ifndef APP_H
#define APP_H

#include "KeyboardManager.h"
#include "DisplayManager.h"
#include "ScreenManager.h"
#include "WiFiManager.h"
#include "JellyfinClient.h"

class App {
public:
    App();
    ~App();
    
    void begin();
    void loop();
    
    WiFiManager& getWiFiManager() { return _wifiManager; }
    JellyfinClient& getJellyfinClient() { return _jellyfinClient; }

private:
    KeyboardManager _keyboardManager;
    DisplayManager _displayManager;
    ScreenManager _screenManager;
    WiFiManager _wifiManager;
    JellyfinClient _jellyfinClient;
    
    uint32_t _lastTickMs;
};

#endif
