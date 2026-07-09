#include "JellyfinLoginScreen.h"
#include <Arduino.h>

JellyfinLoginScreen::JellyfinLoginScreen(JellyfinClient* client)
    : _jellyfinClient(client),
      _focusIndex(0),
      _isConnecting(false),
      _animCounter(0) {
}

void JellyfinLoginScreen::init() {
    // Pre-populate URL and username from saved credentials
    Preferences prefs;
    prefs.begin("jf-config", true);
    _hostBuffer = prefs.getString("host", "http://");
    _userBuffer = prefs.getString("user", "");
    prefs.end();
    
    _passBuffer = ""; // Passwords should never be pre-populated from NVS for security
    _focusIndex = 0;
    _isConnecting = false;
    _animCounter = 0;
}

void JellyfinLoginScreen::update(uint32_t dt) {
    _animCounter += dt;
    
    JellyfinStatus stat = _jellyfinClient->getStatus();
    if (stat == JellyfinStatus::AUTHENTICATING) {
        _isConnecting = true;
    } else {
        _isConnecting = false;
    }
}

void JellyfinLoginScreen::draw(M5Canvas& canvas) {
    canvas.fillScreen(DisplayManager::COLOR_BG);
    
    // Draw Header
    canvas.drawFastHLine(0, 14, 240, DisplayManager::COLOR_CYAN);
    canvas.setTextColor(DisplayManager::COLOR_CYAN);
    canvas.setTextSize(1);
    canvas.setCursor(6, 3);
    canvas.print("JELLYFIN ACCESS PANEL");
    
    int boxX = 60;
    int boxW = 170;
    int boxH = 13;
    
    // HOST Input
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.setCursor(10, 20);
    canvas.print("URL:");
    
    int hostY = 17;
    if (_focusIndex == 0) {
        canvas.drawRect(boxX, hostY, boxW, boxH, DisplayManager::COLOR_CYAN);
    } else {
        canvas.drawRect(boxX, hostY, boxW, boxH, DisplayManager::COLOR_GRAY);
    }
    canvas.setCursor(boxX + 4, hostY + 3);
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.print(_hostBuffer);
    if (_focusIndex == 0 && (_animCounter / 300) % 2 == 0) {
        canvas.print("_");
    }
    
    // USER Input
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.setCursor(10, 35);
    canvas.print("USER:");
    
    int userY = 32;
    if (_focusIndex == 1) {
        canvas.drawRect(boxX, userY, boxW, boxH, DisplayManager::COLOR_CYAN);
    } else {
        canvas.drawRect(boxX, userY, boxW, boxH, DisplayManager::COLOR_GRAY);
    }
    canvas.setCursor(boxX + 4, userY + 3);
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.print(_userBuffer);
    if (_focusIndex == 1 && (_animCounter / 300) % 2 == 0) {
        canvas.print("_");
    }
    
    // PASSWORD Input
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.setCursor(10, 50);
    canvas.print("PASS:");
    
    int passY = 47;
    if (_focusIndex == 2) {
        canvas.drawRect(boxX, passY, boxW, boxH, DisplayManager::COLOR_CYAN);
    } else {
        canvas.drawRect(boxX, passY, boxW, boxH, DisplayManager::COLOR_GRAY);
    }
    canvas.setCursor(boxX + 4, passY + 3);
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    
    // Draw password in plain text as requested by user
    canvas.print(_passBuffer);
    if (_focusIndex == 2 && (_animCounter / 300) % 2 == 0) {
        canvas.print("_");
    }
    
    // Buttons
    int btnW = 90;
    int btnH = 15;
    
    // LOGIN Button
    int logX = 20;
    int logY = 65;
    if (_focusIndex == 3) {
        canvas.fillRect(logX, logY, btnW, btnH, DisplayManager::COLOR_SEL_BG);
        canvas.setTextColor(DisplayManager::COLOR_SEL_FG);
    } else {
        canvas.drawRect(logX, logY, btnW, btnH, DisplayManager::COLOR_GRAY);
        canvas.setTextColor(DisplayManager::COLOR_GREEN);
    }
    canvas.setCursor(logX + 22, logY + 4);
    canvas.print("[ LOGIN ]");
    
    // CANCEL Button
    int cancX = 130;
    int cancY = 65;
    if (_focusIndex == 4) {
        canvas.fillRect(cancX, cancY, btnW, btnH, DisplayManager::COLOR_SEL_BG);
        canvas.setTextColor(DisplayManager::COLOR_SEL_FG);
    } else {
        canvas.drawRect(cancX, cancY, btnW, btnH, DisplayManager::COLOR_GRAY);
        canvas.setTextColor(DisplayManager::COLOR_GREEN);
    }
    canvas.setCursor(cancX + 20, cancY + 4);
    canvas.print("[ CANCEL ]");
    
    // Status text section
    int statusY = 85;
    canvas.drawFastHLine(10, statusY, 220, DisplayManager::COLOR_GRAY);
    
    JellyfinStatus stat = _jellyfinClient->getStatus();
    canvas.setCursor(10, statusY + 6);
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.print("LINK STATUS: ");
    
    switch (stat) {
        case JellyfinStatus::LOGGED_IN:
            canvas.setTextColor(DisplayManager::COLOR_CYAN);
            canvas.println("AUTH SUCCESSFUL");
            canvas.setTextColor(DisplayManager::COLOR_GREEN);
            canvas.setCursor(10, statusY + 16);
            canvas.print("USER ID: ");
            canvas.setTextColor(DisplayManager::COLOR_CYAN);
            canvas.println(_jellyfinClient->getUserId().substring(0, 16) + "...");
            break;
            
        case JellyfinStatus::AUTHENTICATING:
            canvas.setTextColor(DisplayManager::COLOR_GREEN);
            canvas.print("CONNECTING");
            for (int d = 0; d < (_animCounter / 400) % 4; d++) {
                canvas.print(".");
            }
            canvas.println();
            canvas.setCursor(10, statusY + 16);
            canvas.setTextColor(DisplayManager::COLOR_GRAY);
            canvas.println("Establishing REST handshake...");
            break;
            
        case JellyfinStatus::ERROR_CREDENTIALS:
            canvas.setTextColor(DisplayManager::COLOR_RED);
            canvas.println("ERROR - INVALID CREDENTIALS");
            canvas.setCursor(10, statusY + 16);
            canvas.setTextColor(DisplayManager::COLOR_GRAY);
            canvas.println("Check username, password or host url.");
            break;
            
        case JellyfinStatus::ERROR_SERVER:
            canvas.setTextColor(DisplayManager::COLOR_RED);
            canvas.println("ERROR - SERVER UNREACHABLE");
            canvas.setCursor(10, statusY + 16);
            canvas.setTextColor(DisplayManager::COLOR_GRAY);
            canvas.println("Verify IP address, port and server state.");
            break;
            
        case JellyfinStatus::ERROR_NETWORK:
            canvas.setTextColor(DisplayManager::COLOR_RED);
            canvas.println("ERROR - NO WI-FI LINK");
            canvas.setCursor(10, statusY + 16);
            canvas.setTextColor(DisplayManager::COLOR_GRAY);
            canvas.println("Verify Wi-Fi link parameters.");
            break;
            
        case JellyfinStatus::LOGGED_OUT:
        default:
            canvas.setTextColor(DisplayManager::COLOR_GRAY);
            canvas.println("OFFLINE");
            canvas.setCursor(10, statusY + 16);
            canvas.println("Enter server details to sync.");
            break;
    }
    
    // Footer
    canvas.drawFastHLine(0, 120, 240, DisplayManager::COLOR_CYAN);
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.setCursor(6, 124);
    canvas.print("NAV: [Tab/Arrows] ACTION: [Enter]");
    
    canvas.setTextColor(DisplayManager::COLOR_CYAN);
    canvas.setCursor(218, 124);
    canvas.print(">_");
}

void JellyfinLoginScreen::handleKey(const KeyInput& key) {
    if (key.keyType == CardputerKey::TAB) {
        _focusIndex = (_focusIndex + 1) % 5;
    } else if (key.keyType == CardputerKey::UP) {
        if (_focusIndex > 0) {
            _focusIndex--;
        } else {
            _focusIndex = 4;
        }
    } else if (key.keyType == CardputerKey::DOWN) {
        if (_focusIndex < 4) {
            _focusIndex++;
        } else {
            _focusIndex = 0;
        }
    } else if (key.keyType == CardputerKey::ESC) {
        _manager->popScreen();
    } else if (key.keyType == CardputerKey::BACKSPACE) {
        if (_focusIndex == 0) {
            if (_hostBuffer.length() > 0) {
                _hostBuffer.remove(_hostBuffer.length() - 1);
            }
        } else if (_focusIndex == 1) {
            if (_userBuffer.length() > 0) {
                _userBuffer.remove(_userBuffer.length() - 1);
            }
        } else if (_focusIndex == 2) {
            if (_passBuffer.length() > 0) {
                _passBuffer.remove(_passBuffer.length() - 1);
            }
        } else {
            _manager->popScreen(); // Backspace to go back in buttons
        }
    } else if (key.keyType == CardputerKey::ENTER) {
        if (_focusIndex == 0) {
            _focusIndex = 1;
        } else if (_focusIndex == 1) {
            _focusIndex = 2;
        } else if (_focusIndex == 2) {
            _focusIndex = 3;
        } else if (_focusIndex == 3) {
            // Trigger login handshake
            _jellyfinClient->login(_hostBuffer.c_str(), _userBuffer.c_str(), _passBuffer.c_str());
        } else if (_focusIndex == 4) {
            _manager->popScreen();
        }
    } else if (key.keyType == CardputerKey::CHAR) {
        if (_focusIndex == 0) {
            _hostBuffer += key.character;
        } else if (_focusIndex == 1) {
            _userBuffer += key.character;
        } else if (_focusIndex == 2) {
            _passBuffer += key.character;
        }
    }
}
