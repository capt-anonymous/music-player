#ifndef ALBUM_ART_MANAGER_H
#define ALBUM_ART_MANAGER_H

#include "JellyfinClient.h"
#include "StorageManager.h"
#include <Arduino.h>

class AlbumArtManager {
public:
    AlbumArtManager(JellyfinClient* client, StorageManager* storage);
    ~AlbumArtManager();
    
    void begin();
    
    // Checks if cached locally on SD, downloads it if missing, and returns filepath.
    // Falls back to RAM downloading ("RAM") if SD card is offline.
    String getArtworkPath(const String& albumId);
    
    bool isCached(const String& albumId);
    
    // RAM Buffer Getters
    const uint8_t* getRamBuffer() const { return _ramArtBuffer; }
    size_t getRamSize() const { return _ramArtSize; }

private:
    JellyfinClient* _jellyfinClient;
    StorageManager* _storageManager;
    
    uint8_t* _ramArtBuffer;
    size_t _ramArtSize;
    String _ramArtAlbumId;
    
    bool downloadArtwork(const String& albumId, const String& localPath);
    bool downloadArtworkToRam(const String& albumId);
    void freeRamBuffer();
};

#endif
