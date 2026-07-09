#ifndef MAIN_MENU_SCREEN_H
#define MAIN_MENU_SCREEN_H

#include "ScreenManager.h"
#include <vector>

class MainMenuScreen : public Screen {
public:
    MainMenuScreen();
    virtual void init() override;
    virtual void update(uint32_t dt) override;
    virtual void draw(M5Canvas& canvas) override;
    virtual void handleKey(const KeyInput& key) override;

private:
    std::vector<const char*> _menuItems;
    size_t _selectedIndex;
};

#endif
