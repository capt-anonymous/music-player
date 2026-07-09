#include "WiFiSetupScreen.h"
#include <Arduino.h>

WiFiSetupScreen::WiFiSetupScreen(WiFiManager* wifiManager)
    : _wifiManager(wifiManager),
      _focusIndex(0),
      _isConnecting(false),
      _animCounter(0) {
}

void WiFiSetupScreen::init() {
    // Pre-populate input buffers from saved credentials
    Preferences prefs;
    prefs.begin("wifi-config", true);
    _ssidBuffer = prefs.getString("ssid", "");
    _passBuffer = prefs.getString("pass", "");
    prefs.end();
    
    _focusIndex = 0;
    _isConnecting = false;
    _animCounter = 0;
}

void WiFiSetupScreen::update(uint32_t dt) {
    _animCounter += dt;
    
    // Auto-update connecting flag based on Wi-Fi Manager status
    WiFiStatus status = _wifiManager->getStatus();
    if (status == WiFiStatus::CONNECTING) {
        _isConnecting = true;
    } else {
        _isConnecting = false;
    }
}

void WiFiSetupScreen::draw(M5Canvas& canvas) {
    canvas.fillScreen(DisplayManager::COLOR_BG);
    
    // Draw Header
    canvas.drawFastHLine(0, 14, 240, DisplayManager::COLOR_CYAN);
    canvas.setTextColor(DisplayManager::COLOR_CYAN);
    canvas.setTextSize(1);
    canvas.setCursor(6, 3);
    canvas.print("WI-FI CONFIGURATION");
    
    // SSID Input box
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.setCursor(10, 22);
    canvas.print("SSID:");
    
    int ssidBoxX = 70;
    int ssidBoxY = 19;
    int boxW = 160;
    int boxH = 14;
    
    if (_focusIndex == 0) {
        canvas.drawRect(ssidBoxX, ssidBoxY, boxW, boxH, DisplayManager::COLOR_CYAN);
    } else {
        canvas.drawRect(ssidBoxX, ssidBoxY, boxW, boxH, DisplayManager::COLOR_GRAY);
    }
    canvas.setCursor(ssidBoxX + 4, ssidBoxY + 3);
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.print(_ssidBuffer);
    // Draw cursor in active box
    if (_focusIndex == 0 && (_animCounter / 300) % 2 == 0) {
        canvas.print("_");
    }
    
    // Password Input box
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.setCursor(10, 42);
    canvas.print("PASSWORD:");
    
    int passBoxX = 70;
    int passBoxY = 39;
    
    if (_focusIndex == 1) {
        canvas.drawRect(passBoxX, passBoxY, boxW, boxH, DisplayManager::COLOR_CYAN);
    } else {
        canvas.drawRect(passBoxX, passBoxY, boxW, boxH, DisplayManager::COLOR_GRAY);
    }
    canvas.setCursor(passBoxX + 4, passBoxY + 3);
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    
    // Print password directly (visible text)
    canvas.print(_passBuffer);
    if (_focusIndex == 1 && (_animCounter / 300) % 2 == 0) {
        canvas.print("_");
    }
    
    // Connect & Cancel Buttons
    int btnW = 90;
    int btnH = 16;
    
    // CONNECT Button
    int connX = 20;
    int connY = 60;
    if (_focusIndex == 2) {
        canvas.fillRect(connX, connY, btnW, btnH, DisplayManager::COLOR_SEL_BG);
        canvas.setTextColor(DisplayManager::COLOR_SEL_FG);
    } else {
        canvas.drawRect(connX, connY, btnW, btnH, DisplayManager::COLOR_GRAY);
        canvas.setTextColor(DisplayManager::COLOR_GREEN);
    }
    canvas.setCursor(connX + 16, connY + 4);
    canvas.print("[ CONNECT ]");
    
    // CANCEL Button
    int cancX = 130;
    int cancY = 60;
    if (_focusIndex == 3) {
        canvas.fillRect(cancX, cancY, btnW, btnH, DisplayManager::COLOR_SEL_BG);
        canvas.setTextColor(DisplayManager::COLOR_SEL_FG);
    } else {
        canvas.drawRect(cancX, cancY, btnW, btnH, DisplayManager::COLOR_GRAY);
        canvas.setTextColor(DisplayManager::COLOR_GREEN);
    }
    canvas.setCursor(cancX + 16, cancY + 4);
    canvas.print("[ CANCEL  ]");
    
    // Diagnostic / Status logs area
    int statusY = 85;
    canvas.drawFastHLine(10, statusY, 220, DisplayManager::COLOR_GRAY);
    
    WiFiStatus wifiStat = _wifiManager->getStatus();
    canvas.setCursor(10, statusY + 6);
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.print("NET STATUS: ");
    
    switch (wifiStat) {
        case WiFiStatus::CONNECTED:
            canvas.setTextColor(DisplayManager::COLOR_CYAN);
            canvas.println("CONNECTED");
            canvas.setTextColor(DisplayManager::COLOR_GREEN);
            canvas.setCursor(10, statusY + 16);
            canvas.print("LOCAL IP:   ");
            canvas.setTextColor(DisplayManager::COLOR_CYAN);
            canvas.println(_wifiManager->getIP());
            
            canvas.setTextColor(DisplayManager::COLOR_GREEN);
            canvas.setCursor(10, statusY + 26);
            canvas.print("RSSI/SIGNAL: ");
            canvas.setTextColor(DisplayManager::COLOR_CYAN);
            canvas.print(_wifiManager->getRSSI());
            canvas.println(" dBm");
            break;
            
        case WiFiStatus::CONNECTING:
            canvas.setTextColor(DisplayManager::COLOR_GREEN);
            canvas.print("CONNECTING");
            // Animated dots
            for (int d = 0; d < (_animCounter / 400) % 4; d++) {
                canvas.print(".");
            }
            canvas.println();
            canvas.setCursor(10, statusY + 16);
            canvas.setTextColor(DisplayManager::COLOR_GRAY);
            canvas.println("Please check server visibility...");
            break;
            
        case WiFiStatus::CONNECT_FAILED:
            canvas.setTextColor(DisplayManager::COLOR_RED);
            canvas.println("CONNECTION FAILED");
            canvas.setCursor(10, statusY + 16);
            canvas.setTextColor(DisplayManager::COLOR_GRAY);
            canvas.println("Verify SSID, credentials or distance.");
            break;
            
        case WiFiStatus::DISCONNECTED:
        default:
            canvas.setTextColor(DisplayManager::COLOR_GRAY);
            canvas.println("DISCONNECTED");
            canvas.setCursor(10, statusY + 16);
            canvas.println("Enter details to initiate link.");
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

void WiFiSetupScreen::handleKey(const KeyInput& key) {
    if (key.keyType == CardputerKey::TAB) {
        _focusIndex = (_focusIndex + 1) % 4;
    } else if (key.keyType == CardputerKey::UP) {
        if (_focusIndex > 0) {
            _focusIndex--;
        } else {
            _focusIndex = 3;
        }
    } else if (key.keyType == CardputerKey::DOWN) {
        if (_focusIndex < 3) {
            _focusIndex++;
        } else {
            _focusIndex = 0;
        }
    } else if (key.keyType == CardputerKey::ESC) {
        _manager->popScreen();
    } else if (key.keyType == CardputerKey::BACKSPACE) {
        if (_focusIndex == 0) {
            if (_ssidBuffer.length() > 0) {
                _ssidBuffer.remove(_ssidBuffer.length() - 1);
            }
        } else if (_focusIndex == 1) {
            if (_passBuffer.length() > 0) {
                _passBuffer.remove(_passBuffer.length() - 1);
            }
        } else {
            // "Backspace to go back" decision
            _manager->popScreen();
        }
    } else if (key.keyType == CardputerKey::ENTER) {
        if (_focusIndex == 0) {
            _focusIndex = 1; // Move to password
        } else if (_focusIndex == 1) {
            _focusIndex = 2; // Move to connect button
        } else if (_focusIndex == 2) {
            // Trigger connection
            _wifiManager->connect(_ssidBuffer.c_str(), _passBuffer.c_str());
        } else if (_focusIndex == 3) {
            _manager->popScreen();
        }
    } else if (key.keyType == CardputerKey::CHAR) {
        if (_focusIndex == 0) {
            _ssidBuffer += key.character;
        } else if (_focusIndex == 1) {
            _passBuffer += key.character;
        }
    }
}
