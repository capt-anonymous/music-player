#include "BootScreen.h"
#include "MainMenuScreen.h"
#include <Arduino.h>

BootScreen::BootScreen() 
    : _visibleLinesCount(0), 
      _timeAccumulator(0), 
      _logIntervalMs(300), 
      _holdTimeMs(800), 
      _isDone(false), 
      _progress(0.0f) {
}

void BootScreen::init() {
    _logLines = {
        "CYBER MUSIC TERMINAL v1.0",
        "DISPLAY CORE......... [OK]",
        "KEYBOARD DEBOUNCER... [OK]",
        "STORAGE MANAGER...... [BYPASS]",
        "WIFI CONTROLLER...... [STANDBY]",
        "JELLYFIN ENDPOINT.... [OFFLINE]",
        "SYSTEM STATUS........ [READY]"
    };
    _visibleLinesCount = 0;
    _timeAccumulator = 0;
    _isDone = false;
    _progress = 0.0f;
}

void BootScreen::update(uint32_t dt) {
    if (_isDone) {
        _timeAccumulator += dt;
        if (_timeAccumulator >= _holdTimeMs) {
            _manager->replaceScreen(new MainMenuScreen());
        }
        return;
    }
    
    _timeAccumulator += dt;
    
    // Smooth progress bar calculation
    float targetProgress = (float)(_visibleLinesCount + 1) / (_logLines.size() + 1);
    if (_progress < targetProgress) {
        _progress += 0.05f;
        if (_progress > targetProgress) _progress = targetProgress;
    }
    
    if (_timeAccumulator >= _logIntervalMs) {
        _timeAccumulator = 0;
        if (_visibleLinesCount < _logLines.size()) {
            _visibleLinesCount++;
        } else {
            _isDone = true;
            _progress = 1.0f;
        }
    }
}

void BootScreen::draw(M5Canvas& canvas) {
    canvas.fillScreen(DisplayManager::COLOR_BG);
    
    // Draw Terminal Header
    canvas.drawFastHLine(0, 14, 240, DisplayManager::COLOR_CYAN);
    canvas.setTextColor(DisplayManager::COLOR_CYAN);
    canvas.setTextSize(1);
    canvas.setCursor(6, 3);
    canvas.print("CYBER SYSTEM BOOT MANAGER");
    
    // Draw log lines
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    int y = 22;
    for (size_t i = 0; i < _visibleLinesCount; ++i) {
        canvas.setCursor(10, y);
        canvas.print("> ");
        canvas.print(_logLines[i]);
        y += 11;
    }
    
    // Blinking prompt character if not fully booted
    if (!_isDone && (millis() / 250) % 2 == 0) {
        canvas.print("_");
    }
    
    // Draw Progress Bar at the bottom
    int barY = 105;
    int barW = 220;
    int barH = 8;
    canvas.drawRect(10, barY, barW, barH, DisplayManager::COLOR_GRAY);
    
    int fillW = (int)(_progress * (barW - 4));
    if (fillW < 0) fillW = 0;
    canvas.fillRect(12, barY + 2, fillW, barH - 4, DisplayManager::COLOR_CYAN);
    
    // Status text in bottom bar
    canvas.drawFastHLine(0, 120, 240, DisplayManager::COLOR_CYAN);
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.setCursor(6, 124);
    canvas.print("BOOT: ");
    if (_isDone) {
        canvas.setTextColor(DisplayManager::COLOR_CYAN);
        canvas.print("COMPLETED");
    } else {
        canvas.print("INITIALIZING...");
    }
}

void BootScreen::handleKey(const KeyInput& key) {
    // If the user presses ENTER or SPACE, we skip the boot screen sequence immediately
    if (key.keyType == CardputerKey::ENTER || (key.keyType == CardputerKey::CHAR && key.character == ' ')) {
        _manager->replaceScreen(new MainMenuScreen());
    }
}
