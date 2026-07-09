#ifndef WIFI_SETUP_SCREEN_H
#define WIFI_SETUP_SCREEN_H

#include "ScreenManager.h"
#include "WiFiManager.h"

class WiFiSetupScreen : public Screen {
public:
    WiFiSetupScreen(WiFiManager* wifiManager);
    virtual void init() override;
    virtual void update(uint32_t dt) override;
    virtual void draw(M5Canvas& canvas) override;
    virtual void handleKey(const KeyInput& key) override;

private:
    WiFiManager* _wifiManager;
    
    String _ssidBuffer;
    String _passBuffer;
    
    int _focusIndex; // 0 = SSID, 1 = Password, 2 = Connect, 3 = Cancel
    bool _isConnecting;
    uint32_t _animCounter;
};

#endif
