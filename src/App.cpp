#include "App.h"
#include "BootScreen.h"
#include <Arduino.h>

App::App() : _albumArtManager(&_jellyfinClient, &_storageManager),
             _audioOut(nullptr),
             _audioHttp(nullptr),
             _audioBuffer(nullptr),
             _audioMp3(nullptr),
             _volume(12),
             _isPaused(false),
             _lastTickMs(0) {
}

App::~App() {
    stopAudio();
}

void App::begin() {
    auto cfg = M5.config();
    cfg.fallback_board = m5::board_t::board_M5CardputerADV; // Force Cardputer ADV board profile to set I2C SCL=9, SDA=8
    cfg.internal_spk = true; // Enable internal speaker to let M5Unified initialize the ES8311 codec on Cardputer ADV
    
    // Initialize Stamps3 board, keyboard matrix, and core hardware interfaces
    M5Cardputer.begin(cfg, true);
    
    // Set Speaker default volume (12 * 12 = 144)
    M5Cardputer.Speaker.setVolume(144);
    
    // Set up screen canvas buffer
    _displayManager.begin();
    
    // Initialize storage mount (microSD Card)
    _storageManager.begin();
    
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
    M5Cardputer.update();
    
    // Run audio streaming engine updates in background
    if (_audioMp3 && _audioMp3->isRunning() && !_isPaused) {
        if (!_audioMp3->loop()) {
            _audioMp3->stop();
            stopAudio();
        }
    }
    
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
    delay(2);
}

void App::playUrl(const String& url) {
    stopAudio();
    _isPaused = false;
    
    Serial.print("[Audio] Playing HTTP URL: ");
    Serial.println(url);
    
    _audioHttp = new AudioFileSourceHTTPStream(url.c_str());
    _audioBuffer = new AudioFileSourceBuffer(_audioHttp, 2048);
    _audioOut = new AudioOutputM5Speaker(&M5Cardputer.Speaker, 0);
    _audioMp3 = new AudioGeneratorMP3();
    
    if (!_audioMp3->begin(_audioBuffer, _audioOut)) {
        Serial.println("[Audio] Failed to begin MP3 playback");
        stopAudio();
    } else {
        Serial.println("[Audio] MP3 streaming started successfully");
    }
}

void App::stopAudio() {
    if (_audioMp3) {
        _audioMp3->stop();
        delete _audioMp3;
        _audioMp3 = nullptr;
    }
    if (_audioOut) {
        _audioOut->stop();
        delete _audioOut;
        _audioOut = nullptr;
    }
    if (_audioBuffer) {
        _audioBuffer->close();
        delete _audioBuffer;
        _audioBuffer = nullptr;
    }
    if (_audioHttp) {
        _audioHttp->close();
        delete _audioHttp;
        _audioHttp = nullptr;
    }
    Serial.println("[Audio] Playback stopped and cleaned up");
}

void App::togglePause() {
    if (_audioMp3 && _audioMp3->isRunning()) {
        _isPaused = !_isPaused;
        if (_isPaused) {
            M5Cardputer.Speaker.stop(0);
            Serial.println("[Audio] Paused playback");
        } else {
            Serial.println("[Audio] Resumed playback");
        }
    }
}

bool App::isAudioPlaying() const {
    return _audioMp3 && _audioMp3->isRunning();
}

bool App::isAudioPaused() const {
    return _isPaused;
}

void App::setVolume(int volume) {
    if (volume < 0) volume = 0;
    if (volume > 21) volume = 21;
    _volume = volume;
    M5Cardputer.Speaker.setVolume(volume * 12);
    Serial.print("[Audio] Set Volume: ");
    Serial.println(volume);
}

int App::getVolume() const {
    return _volume;
}

uint32_t App::getAudioPosition() const {
    if (_audioHttp) {
        return _audioHttp->getPos() / 16000;
    }
    return 0;
}

uint32_t App::getAudioDuration() const {
    if (_audioHttp) {
        return _audioHttp->getSize() / 16000;
    }
    return 0;
}

