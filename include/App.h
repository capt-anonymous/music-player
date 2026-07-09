#ifndef APP_H
#define APP_H

#include "KeyboardManager.h"
#include "DisplayManager.h"
#include "ScreenManager.h"
#include "WiFiManager.h"
#include "JellyfinClient.h"
#include "StorageManager.h"
#include "AlbumArtManager.h"
#include "Audio.h"

class App {
public:
    App();
    ~App();
    
    void begin();
    void loop();
    
    WiFiManager& getWiFiManager() { return _wifiManager; }
    JellyfinClient& getJellyfinClient() { return _jellyfinClient; }
    StorageManager& getStorageManager() { return _storageManager; }
    AlbumArtManager& getAlbumArtManager() { return _albumArtManager; }
    Audio& getAudio() { return _audio; }

private:
    KeyboardManager _keyboardManager;
    DisplayManager _displayManager;
    ScreenManager _screenManager;
    WiFiManager _wifiManager;
    JellyfinClient _jellyfinClient;
    StorageManager _storageManager;
    AlbumArtManager _albumArtManager;
    Audio _audio;
    
    uint32_t _lastTickMs;
};

#endif
