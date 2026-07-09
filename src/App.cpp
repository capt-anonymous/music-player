#include "App.h"
#include "BootScreen.h"
#include <Arduino.h>

App::App() : _lastTickMs(0) {
}

App::~App() {
}

void App::begin() {
    auto cfg = M5.config();
    // Initialize Stamps3 board, keyboard matrix, and core hardware interfaces
    M5Cardputer.begin(cfg, true);
    
    // Set up screen canvas buffer
    _displayManager.begin();
    
    // Initialize Wi-Fi and load saved credentials
    _wifiManager.begin();
    
    // Initialize Jellyfin client and verify saved session
    _jellyfinClient.begin();
    
    // Start with the cyberpunk Boot animation
    _screenManager.pushScreen(new BootScreen());
    _screenManager.processPendingTransitions();
    
    _lastTickMs = millis();
}

void App::loop() {
    uint32_t currentMs = millis();
    uint32_t dt = currentMs - _lastTickMs;
    _lastTickMs = currentMs;
    
    // Update internal M5Unified key and power registers
    M5Cardputer.update();
    
    // Scan keyboard and update event queue
    _keyboardManager.update();
    
    // Update background Wi-Fi connection states
    _wifiManager.update();
    
    // Process keyboard events sequentially
    while (_keyboardManager.hasKey()) {
        KeyInput key = _keyboardManager.getNextKey();
        _screenManager.handleKey(key);
    }
    
    // Update logic for active screen (e.g. animation frames, cursor blinks)
    _screenManager.update(dt);
    
    // Render scene into double buffer canvas offscreen
    _displayManager.clear();
    _screenManager.draw(_displayManager.getCanvas());
    
    // Draw canvas to physical TFT
    _displayManager.publish();
    
    // Perform deferred screen transitions (pop/push/replace) safely outside logic loops
    _screenManager.processPendingTransitions();
    
    // Give CPU time to network stack and RTOS
    delay(10);
}

