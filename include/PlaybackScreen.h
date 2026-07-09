#ifndef PLAYBACK_SCREEN_H
#define PLAYBACK_SCREEN_H

#include "ScreenManager.h"
#include "JellyfinClient.h"
#include <Audio.h>

class PlaybackScreen : public Screen {
public:
    PlaybackScreen(JellyfinClient* client, Audio* audio, const String& trackId, const String& trackName);
    virtual void init() override;
    virtual void update(uint32_t dt) override;
    virtual void draw(M5Canvas& canvas) override;
    virtual void handleKey(const KeyInput& key) override;

private:
    JellyfinClient* _jellyfinClient;
    Audio* _audio;
    
    String _trackId;
    String _trackName;
    String _albumId;
    String _albumName;
    String _artistName;
    
    String _localArtPath;
    
    bool _isLoadingMetadata;
    bool _hasError;
    uint32_t _animCounter;
    
    void fetchTrackMetadata();
    String formatTime(uint32_t seconds);
};

#endif
