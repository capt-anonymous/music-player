#include "App.h"
#include "BootScreen.h"
#include <Arduino.h>

App::App() : _albumArtManager(&_jellyfinClient, &_storageManager), _lastTickMs(0) {
}

App::~App() {
}

void App::begin() {
    auto cfg = M5.config();
    cfg.fallback_board = m5::board_t::board_M5CardputerADV; // Force Cardputer ADV board profile to set I2C SCL=9, SDA=8
    cfg.internal_spk = true; // Enable internal speaker to let M5Unified initialize the ES8311 codec on Cardputer ADV
    
    // Re-route M5Unified speaker driver to I2S_NUM_1 to prevent DMA/interrupt conflicts 
    // with ESP32-audioI2S (which requires exclusive access to I2S_NUM_0)
    auto spk_cfg = M5.Speaker.config();
    spk_cfg.i2s_port = I2S_NUM_1;
    M5.Speaker.config(spk_cfg);
    
    // Initialize Stamps3 board, keyboard matrix, and core hardware interfaces
    M5Cardputer.begin(cfg, true);
    
    // Set up screen canvas buffer
    _displayManager.begin();
    
    // Initialize storage mount (microSD Card)
    _storageManager.begin();
    
    // Initialize Wi-Fi and load saved credentials
    _wifiManager.begin();
    
    // Initialize Jellyfin client and verify saved session
    _jellyfinClient.begin();
    
    // Configure dedicated I2S speaker output routing
    // Pins on Cardputer: BCLK=41, LRCK=43, DOUT=42
    _audio.setPinout(41, 43, 42);
    _audio.setVolume(12); // Default volume (scale of 0-21)
    
    // Start with the cyberpunk Boot animation
    _screenManager.pushScreen(new BootScreen());
    _screenManager.processPendingTransitions();
    
    _lastTickMs = millis();
}

void App::loop() {
    M5Cardputer.update();
    
    // Run audio streaming engine updates in background
    _audio.loop();
    
    // Calculate delta time
    uint32_t currentMs = millis();
    uint32_t dt = currentMs - _lastTickMs;
    _lastTickMs = currentMs;
    
    // Update internal M5Unified key and power registers
    
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

