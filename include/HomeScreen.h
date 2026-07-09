#ifndef HOME_SCREEN_H
#define HOME_SCREEN_H

#include "ScreenManager.h"
#include "JellyfinClient.h"
#include <vector>

class HomeScreen : public Screen {
public:
    HomeScreen(JellyfinClient* client);
    virtual void init() override;
    virtual void update(uint32_t dt) override;
    virtual void draw(M5Canvas& canvas) override;
    virtual void handleKey(const KeyInput& key) override;

private:
    JellyfinClient* _jellyfinClient;
    std::vector<const char*> _menuItems;
    size_t _selectedIndex;
};

#endif
