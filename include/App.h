#ifndef APP_H
#define APP_H

#include "KeyboardManager.h"
#include "DisplayManager.h"
#include "ScreenManager.h"
#include "WiFiManager.h"

class App {
public:
    App();
    ~App();
    
    void begin();
    void loop();
    
    WiFiManager& getWiFiManager() { return _wifiManager; }

private:
    KeyboardManager _keyboardManager;
    DisplayManager _displayManager;
    ScreenManager _screenManager;
    WiFiManager _wifiManager;
    
    uint32_t _lastTickMs;
};

#endif
