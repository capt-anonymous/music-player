#ifndef APP_H
#define APP_H

#include "KeyboardManager.h"
#include "DisplayManager.h"
#include "ScreenManager.h"
#include "WiFiManager.h"
#include "JellyfinClient.h"
#include "StorageManager.h"
#include "AudioOutputM5Speaker.h"
#include <AudioGeneratorMP3.h>
#include <AudioFileSourceHTTPStream.h>
#include <AudioFileSourceBuffer.h>

class App {
public:
    App();
    ~App();
    
    void begin();
    void loop();
    
    WiFiManager& getWiFiManager() { return _wifiManager; }
    JellyfinClient& getJellyfinClient() { return _jellyfinClient; }
    StorageManager& getStorageManager() { return _storageManager; }
    DisplayManager& getDisplayManager() { return _displayManager; }
    
    // Audio Control Wrapper Methods
    void playUrl(const String& url);
    void stopAudio();
    void togglePause();
    bool isAudioPlaying() const;
    bool isAudioPaused() const;
    void setVolume(int volume);
    int getVolume() const;
    uint32_t getAudioPosition() const;
    uint32_t getAudioDuration() const;
    const int16_t* getAudioBuffer() const;

private:
    KeyboardManager _keyboardManager;
    DisplayManager _displayManager;
    ScreenManager _screenManager;
    WiFiManager _wifiManager;
    JellyfinClient _jellyfinClient;
    StorageManager _storageManager;
    
    // ESP8266Audio Decoders and Handlers
    AudioOutputM5Speaker* _audioOut;
    AudioFileSourceHTTPStream* _audioHttp;
    AudioFileSourceBuffer* _audioBuffer;
    AudioGeneratorMP3* _audioMp3;
    int _volume; // Range: 0 to 21
    bool _isPaused;
    
    uint32_t _lastTickMs;
};

#endif
