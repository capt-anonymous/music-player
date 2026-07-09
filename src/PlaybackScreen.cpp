#include "PlaybackScreen.h"
#include "App.h"
#include <Arduino.h>

PlaybackScreen::PlaybackScreen(JellyfinClient* client, Audio* audio, const String& trackId, const String& trackName)
    : _jellyfinClient(client),
      _audio(audio),
      _trackId(trackId),
      _trackName(trackName),
      _albumId(""),
      _albumName(""),
      _artistName(""),
      _localArtPath(""),
      _isLoadingMetadata(true),
      _hasError(false),
      _animCounter(0) {
}

void PlaybackScreen::init() {
    _isLoadingMetadata = true;
    _hasError = false;
    _animCounter = 0;
    
    // Stop any stale playback before starting new stream
    _audio->stopSong();
    
    fetchTrackMetadata();
}

void PlaybackScreen::update(uint32_t dt) {
    _animCounter += dt;
}

void PlaybackScreen::fetchTrackMetadata() {
    // Query full details from Jellyfin to get Album ID for artwork caching
    String endpoint = "/Items/" + _trackId + "?userId=" + _jellyfinClient->getUserId();
    String response;
    
    Serial.print("[Playback] Querying metadata for track ID: ");
    Serial.println(_trackId);
    
    if (_jellyfinClient->sendGetRequest(endpoint, response)) {
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, response);
        
        if (!err) {
            if (doc.containsKey("AlbumId")) {
                _albumId = doc["AlbumId"].as<String>();
            }
            if (doc.containsKey("Album")) {
                _albumName = doc["Album"].as<String>();
            }
            
            // Extract Artist name
            if (doc.containsKey("ArtistItems") && doc["ArtistItems"].size() > 0) {
                _artistName = doc["ArtistItems"][0]["Name"].as<String>();
            } else {
                _artistName = "Unknown Artist";
            }
            
            // Download/resolve album art cache path
            extern App app;
            if (_albumId.length() > 0) {
                _localArtPath = app.getAlbumArtManager().getArtworkPath(_albumId);
            }
            
            _isLoadingMetadata = false;
            
            // Construct direct Jellyfin static audio streaming link
            String streamUrl = _jellyfinClient->getHost() + "/Audio/" + _trackId + "/stream.mp3?static=true&api_key=" + _jellyfinClient->getToken();
            Serial.print("[Playback] Connecting to stream URL: ");
            Serial.println(streamUrl);
            
            // Trigger background streaming download
            _audio->connecttohost(streamUrl.c_str());
        } else {
            _hasError = true;
            _isLoadingMetadata = false;
            Serial.println("[Playback] JSON parse error");
        }
    } else {
        _hasError = true;
        _isLoadingMetadata = false;
        Serial.println("[Playback] HTTP query failed");
    }
}

void PlaybackScreen::draw(M5Canvas& canvas) {
    canvas.fillScreen(DisplayManager::COLOR_BG);
    
    // Draw standard header
    canvas.drawFastHLine(0, 14, 240, DisplayManager::COLOR_CYAN);
    canvas.setTextColor(DisplayManager::COLOR_CYAN);
    canvas.setCursor(6, 3);
    canvas.print("NOW PLAYING");
    
    if (_isLoadingMetadata) {
        canvas.setTextColor(DisplayManager::COLOR_GREEN);
        canvas.setCursor(10, 60);
        canvas.print("Loading track metadata...");
        for (int d = 0; d < (_animCounter / 400) % 4; d++) {
            canvas.print(".");
        }
    } else if (_hasError) {
        canvas.setTextColor(DisplayManager::COLOR_RED);
        canvas.setCursor(10, 60);
        canvas.print("Playback failed. Verify network link.");
    } else {
        // Draw 96x96 Album Art on the left side
        _manager->getDisplayManager().drawAlbumArt(_localArtPath, 10, 20, 96, 96);
        
        // Draw metadata details on the right side
        int textX = 115;
        canvas.setTextColor(DisplayManager::COLOR_CYAN);
        canvas.setCursor(textX, 23);
        
        // Truncate track title if too long to fit
        String displayTrack = _trackName;
        if (displayTrack.length() > 18) displayTrack = displayTrack.substring(0, 15) + "...";
        canvas.print(displayTrack);
        
        // Artist details
        canvas.setTextColor(DisplayManager::COLOR_GREEN);
        canvas.setCursor(textX, 38);
        canvas.print("ARTIST: ");
        canvas.setTextColor(DisplayManager::COLOR_CYAN);
        String displayArtist = _artistName;
        if (displayArtist.length() > 11) displayArtist = displayArtist.substring(0, 8) + "...";
        canvas.print(displayArtist);
        
        // Album details
        canvas.setTextColor(DisplayManager::COLOR_GREEN);
        canvas.setCursor(textX, 51);
        canvas.print("ALBUM: ");
        canvas.setTextColor(DisplayManager::COLOR_CYAN);
        String displayAlbum = _albumName;
        if (displayAlbum.length() > 12) displayAlbum = displayAlbum.substring(0, 9) + "...";
        canvas.print(displayAlbum);
        
        // Volume details
        canvas.setTextColor(DisplayManager::COLOR_GREEN);
        canvas.setCursor(textX, 64);
        canvas.print("VOLUME: ");
        canvas.setTextColor(DisplayManager::COLOR_CYAN);
        canvas.print(String(_audio->getVolume()));
        
        // Progress Time Bar
        uint32_t currentSec = _audio->getAudioCurrentTime();
        uint32_t totalSec = _audio->getAudioFileDuration();
        
        int barY = 80;
        int barW = 115;
        canvas.drawFastHLine(textX, barY, barW, DisplayManager::COLOR_GRAY);
        if (totalSec > 0) {
            float progressFraction = (float)currentSec / totalSec;
            int filledWidth = (int)(progressFraction * barW);
            if (filledWidth > barW) filledWidth = barW;
            canvas.fillRect(textX, barY - 1, filledWidth, 3, DisplayManager::COLOR_CYAN);
        }
        
        // Time stamps
        canvas.setTextColor(DisplayManager::COLOR_GREEN);
        canvas.setCursor(textX, 88);
        canvas.print(formatTime(currentSec) + " / " + formatTime(totalSec));
        
        // Playback Status Indicator
        canvas.setCursor(textX, 102);
        if (_audio->isRunning()) {
            canvas.setTextColor(DisplayManager::COLOR_CYAN);
            canvas.print("[PLAYING]");
        } else {
            canvas.setTextColor(DisplayManager::COLOR_RED);
            canvas.print("[PAUSED]");
        }
    }
    
    // Draw standard footer
    canvas.drawFastHLine(0, 120, 240, DisplayManager::COLOR_CYAN);
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.setCursor(6, 124);
    canvas.print("SPACE: Play/Pause  +/-: Vol  BACK: Exit");
    
    canvas.setTextColor(DisplayManager::COLOR_CYAN);
    canvas.setCursor(218, 124);
    canvas.print(">_");
}

void PlaybackScreen::handleKey(const KeyInput& key) {
    if (key.keyType == CardputerKey::ESC || key.keyType == CardputerKey::BACKSPACE) {
        _audio->stopSong(); // Stop I2S streaming completely
        _manager->popScreen(); // Return to catalog list
    } else if (key.keyType == CardputerKey::ENTER || (key.keyType == CardputerKey::CHAR && key.character == ' ')) {
        _audio->pauseResume(); // Toggle play/pause state
    } else if (key.keyType == CardputerKey::CHAR && (key.character == '+' || key.character == '=')) {
        int vol = _audio->getVolume();
        if (vol < 21) {
            _audio->setVolume(vol + 1);
        }
    } else if (key.keyType == CardputerKey::CHAR && (key.character == '-' || key.character == '_')) {
        int vol = _audio->getVolume();
        if (vol > 0) {
            _audio->setVolume(vol - 1);
        }
    }
}

String PlaybackScreen::formatTime(uint32_t seconds) {
    uint32_t minutes = seconds / 60;
    uint32_t secs = seconds % 60;
    char buffer[10];
    sprintf(buffer, "%02d:%02d", minutes, secs);
    return String(buffer);
}
