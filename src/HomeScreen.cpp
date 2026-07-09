#include "HomeScreen.h"
#include "LibraryScreens.h"
#include "SearchScreen.h"
#include <Arduino.h>

HomeScreen::HomeScreen(JellyfinClient* client)
    : _jellyfinClient(client), _selectedIndex(0) {
}

void HomeScreen::init() {
    _menuItems = {
        "1. BROWSE ARTISTS",
        "2. BROWSE ALBUMS",
        "3. BROWSE SONGS",
        "4. SEARCH LIBRARY",
        "5. LOG OUT JELLYFIN"
    };
    _selectedIndex = 0;
}

void HomeScreen::update(uint32_t dt) {
}

void HomeScreen::draw(M5Canvas& canvas) {
    canvas.fillScreen(DisplayManager::COLOR_BG);
    
    // Draw Header
    canvas.drawFastHLine(0, 14, 240, DisplayManager::COLOR_CYAN);
    canvas.setTextColor(DisplayManager::COLOR_CYAN);
    canvas.setTextSize(1);
    canvas.setCursor(6, 3);
    canvas.print("MUSIC LIBRARY PORTAL");
    
    // Draw Sub-header with server state
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.setCursor(10, 22);
    canvas.print("[CONNECTED: ");
    canvas.setTextColor(DisplayManager::COLOR_CYAN);
    canvas.print(_jellyfinClient->getUsername());
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.print("]");
    
    // Draw menu options
    int y = 40;
    for (size_t i = 0; i < _menuItems.size(); ++i) {
        if (i == _selectedIndex) {
            canvas.fillRect(8, y - 2, 224, 13, DisplayManager::COLOR_SEL_BG);
            canvas.setTextColor(DisplayManager::COLOR_SEL_FG);
        } else {
            canvas.setTextColor(DisplayManager::COLOR_GREEN);
        }
        
        canvas.setCursor(12, y);
        canvas.print(_menuItems[i]);
        y += 15;
    }
    
    // Footer
    canvas.drawFastHLine(0, 120, 240, DisplayManager::COLOR_CYAN);
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.setCursor(6, 124);
    canvas.print("NAV: [;/Arrows] SELECT: [Enter]");
    
    canvas.setTextColor(DisplayManager::COLOR_CYAN);
    canvas.setCursor(218, 124);
    canvas.print(">_");
}

void HomeScreen::handleKey(const KeyInput& key) {
    bool upPressed = (key.keyType == CardputerKey::UP) || (key.keyType == CardputerKey::CHAR && key.character == ';');
    bool downPressed = (key.keyType == CardputerKey::DOWN) || (key.keyType == CardputerKey::CHAR && key.character == '/');
    
    if (upPressed) {
        if (_selectedIndex > 0) {
            _selectedIndex--;
        } else {
            _selectedIndex = _menuItems.size() - 1;
        }
    } else if (downPressed) {
        if (_selectedIndex < _menuItems.size() - 1) {
            _selectedIndex++;
        } else {
            _selectedIndex = 0;
        }
    } else if (key.keyType == CardputerKey::ESC || key.keyType == CardputerKey::BACKSPACE) {
        _manager->popScreen(); // Returns to Main Menu
    } else if (key.keyType == CardputerKey::ENTER) {
        switch (_selectedIndex) {
            case 0:
                _manager->pushScreen(new ArtistsScreen(_jellyfinClient));
                break;
            case 1:
                _manager->pushScreen(new AlbumsScreen(_jellyfinClient));
                break;
            case 2:
                _manager->pushScreen(new SongsScreen(_jellyfinClient));
                break;
            case 3:
                _manager->pushScreen(new SearchScreen(_jellyfinClient));
                break;
            case 4:
                _jellyfinClient->logout();
                _manager->popScreen(); // Returns to Main Menu
                break;
            default:
                break;
        }
    }
}
