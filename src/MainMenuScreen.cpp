#include "MainMenuScreen.h"
#include "PlaceholderScreens.h"
#include "WiFiSetupScreen.h"
#include "JellyfinLoginScreen.h"
#include "App.h"
#include <Arduino.h>

extern App app;

MainMenuScreen::MainMenuScreen() : _selectedIndex(0) {
}

void MainMenuScreen::init() {
    _menuItems = {
        "1. CONNECT WI-FI",
        "2. JELLYFIN LOGIN",
        "3. MUSIC LIBRARY",
        "4. SYSTEM SETTINGS",
        "5. SYSTEM ABOUT"
    };
    _selectedIndex = 0;
}

void MainMenuScreen::update(uint32_t dt) {
}

void MainMenuScreen::draw(M5Canvas& canvas) {
    canvas.fillScreen(DisplayManager::COLOR_BG);
    
    // Draw Header
    canvas.drawFastHLine(0, 14, 240, DisplayManager::COLOR_CYAN);
    canvas.setTextColor(DisplayManager::COLOR_CYAN);
    canvas.setTextSize(1);
    canvas.setCursor(6, 3);
    canvas.print("CYBER MUSIC TERMINAL");
    
    // Draw Menu Title
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.setCursor(10, 22);
    canvas.print("[SYSTEM MAIN MENU]");
    
    // Draw Menu items
    int y = 38;
    for (size_t i = 0; i < _menuItems.size(); ++i) {
        if (i == _selectedIndex) {
            // Highlight selected item
            canvas.fillRect(8, y - 2, 224, 13, DisplayManager::COLOR_SEL_BG);
            canvas.setTextColor(DisplayManager::COLOR_SEL_FG);
        } else {
            canvas.setTextColor(DisplayManager::COLOR_GREEN);
        }
        
        canvas.setCursor(12, y);
        canvas.print(_menuItems[i]);
        y += 15;
    }
    
    // Draw Footer (Updated keys instruction)
    canvas.drawFastHLine(0, 120, 240, DisplayManager::COLOR_CYAN);
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.setCursor(6, 124);
    canvas.print("NAV: [;/Arrows] SELECT: [Enter]");
    
    canvas.setTextColor(DisplayManager::COLOR_CYAN);
    canvas.setCursor(218, 124);
    canvas.print(">_");
}

void MainMenuScreen::handleKey(const KeyInput& key) {
    bool upPressed = (key.keyType == CardputerKey::UP) || (key.keyType == CardputerKey::CHAR && key.character == ';');
    bool downPressed = (key.keyType == CardputerKey::DOWN) || (key.keyType == CardputerKey::CHAR && key.character == '/');
    
    if (upPressed) {
        if (_selectedIndex > 0) {
            _selectedIndex--;
        } else {
            _selectedIndex = _menuItems.size() - 1; // Wrap around
        }
    } else if (downPressed) {
        if (_selectedIndex < _menuItems.size() - 1) {
            _selectedIndex++;
        } else {
            _selectedIndex = 0; // Wrap around
        }
    } else if (key.keyType == CardputerKey::ENTER) {
        switch (_selectedIndex) {
            case 0:
                _manager->pushScreen(new WiFiSetupScreen(&app.getWiFiManager()));
                break;
            case 1:
                _manager->pushScreen(new JellyfinLoginScreen(&app.getJellyfinClient()));
                break;
            case 2:
                _manager->pushScreen(new MusicLibraryScreen());
                break;
            case 3:
                _manager->pushScreen(new SettingsScreen());
                break;
            case 4:
                _manager->pushScreen(new AboutScreen());
                break;
            default:
                break;
        }
    }
}

