#include "AlbumArtManager.h"
#include <HTTPClient.h>
#include <WiFi.h>

AlbumArtManager::AlbumArtManager(JellyfinClient* client, StorageManager* storage)
    : _jellyfinClient(client), _storageManager(storage) {
}

void AlbumArtManager::begin() {
}

bool AlbumArtManager::isCached(const String& albumId) {
    if (!_storageManager->isReady()) return false;
    String localPath = "/cache/" + albumId + ".jpg";
    return _storageManager->exists(localPath.c_str());
}

String AlbumArtManager::getArtworkPath(const String& albumId) {
    if (albumId.length() == 0) return "";
    
    String localPath = "/cache/" + albumId + ".jpg";
    
    // Return path immediately if already stored in cache
    if (isCached(albumId)) {
        return localPath;
    }
    
    // Attempt download if SD card is mounted and we are online
    if (_storageManager->isReady() && WiFi.status() == WL_CONNECTED && _jellyfinClient->hasToken()) {
        Serial.print("[AlbumArt] Image missing from cache. Downloading ID: ");
        Serial.println(albumId);
        if (downloadArtwork(albumId, localPath)) {
            Serial.println("[AlbumArt] Download completed and saved.");
            return localPath;
        }
    }
    
    return "";
}

bool AlbumArtManager::downloadArtwork(const String& albumId, const String& localPath) {
    HTTPClient http;
    String url = _jellyfinClient->getHost() + "/Items/" + albumId + "/Images/Primary?maxWidth=96&maxHeight=96";
    
    http.begin(url);
    http.setTimeout(8000); // 8 seconds timeout
    
    // Inject headers
    http.addHeader("X-MediaBrowser-Token", _jellyfinClient->getToken());
    
    int httpCode = http.GET();
    bool success = false;
    
    if (httpCode == 200) {
        File file = SD.open(localPath.c_str(), FILE_WRITE);
        if (file) {
            WiFiClient* stream = http.getStreamPtr();
            int totalBytes = http.getSize();
            int remainingBytes = totalBytes;
            
            uint8_t buffer[512];
            
            while (http.connected() && (remainingBytes > 0 || totalBytes == -1)) {
                size_t availableBytes = stream->available();
                if (availableBytes > 0) {
                    size_t readLimit = (availableBytes > sizeof(buffer)) ? sizeof(buffer) : availableBytes;
                    int readBytes = stream->readBytes(buffer, readLimit);
                    file.write(buffer, readBytes);
                    
                    if (totalBytes != -1) {
                        remainingBytes -= readBytes;
                    }
                }
                delay(1);
            }
            file.close();
            success = true;
        } else {
            Serial.println("[AlbumArt] Error: Failed to open file on SD card for write");
        }
    } else {
        Serial.print("[AlbumArt] Error: HTTP GET failed, code: ");
        Serial.println(httpCode);
    }
    
    http.end();
    return success;
}
