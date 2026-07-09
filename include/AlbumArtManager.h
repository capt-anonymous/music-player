#ifndef ALBUM_ART_MANAGER_H
#define ALBUM_ART_MANAGER_H

#include "JellyfinClient.h"
#include "StorageManager.h"
#include <Arduino.h>

class AlbumArtManager {
public:
    AlbumArtManager(JellyfinClient* client, StorageManager* storage);
    
    void begin();
    
    // Checks if cached locally on SD, downloads it if missing, and returns filepath.
    // Returns empty string if SD is offline or download fails.
    String getArtworkPath(const String& albumId);
    
    bool isCached(const String& albumId);

private:
    JellyfinClient* _jellyfinClient;
    StorageManager* _storageManager;
    
    bool downloadArtwork(const String& albumId, const String& localPath);
};

#endif
