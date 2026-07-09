#ifndef JELLYFIN_LOGIN_SCREEN_H
#define JELLYFIN_LOGIN_SCREEN_H

#include "ScreenManager.h"
#include "JellyfinClient.h"

class JellyfinLoginScreen : public Screen {
public:
    JellyfinLoginScreen(JellyfinClient* client);
    virtual void init() override;
    virtual void update(uint32_t dt) override;
    virtual void draw(M5Canvas& canvas) override;
    virtual void handleKey(const KeyInput& key) override;

private:
    JellyfinClient* _jellyfinClient;
    
    String _hostBuffer;
    String _userBuffer;
    String _passBuffer;
    
    int _focusIndex; // 0 = URL, 1 = Username, 2 = Password, 3 = Login, 4 = Cancel
    bool _isConnecting;
    uint32_t _animCounter;
};

#endif
