#include "StorageManager.h"
#include <Arduino.h>

StorageManager::StorageManager() : _isReady(false) {
}

bool StorageManager::begin() {
    Serial.println("[Storage] Initializing SPI bus for SD Card...");
    SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_CS);
    
    // Mount the SD card at 15MHz SPI frequency
    if (SD.begin(PIN_CS, SPI, 15000000)) {
        _isReady = true;
        Serial.println("[Storage] microSD card mounted successfully!");
        createDir("/cache");
        return true;
    } else {
        _isReady = false;
        Serial.println("[Storage] WARNING: microSD card mount failed. Operating in bypass mode.");
        return false;
    }
}

bool StorageManager::exists(const char* path) {
    if (!_isReady) return false;
    return SD.exists(path);
}

bool StorageManager::deleteFile(const char* path) {
    if (!_isReady) return false;
    return SD.remove(path);
}

void StorageManager::createDir(const char* path) {
    if (!_isReady) return;
    if (!SD.exists(path)) {
        Serial.print("[Storage] Creating directory: ");
        Serial.println(path);
        SD.mkdir(path);
    }
}
