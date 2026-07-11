#include "DisplayManager.h"
#include "App.h"
#include <SD.h>

DisplayManager::DisplayManager() : _canvas(&M5Cardputer.Display) {
}

void DisplayManager::begin() {
    _canvas.createSprite(240, 135);
    _canvas.setTextSize(1);
    _canvas.setTextColor(COLOR_GREEN, COLOR_BG);
    clear();
}

void DisplayManager::clear() {
    _canvas.fillScreen(COLOR_BG);
}

void DisplayManager::publish() {
    _canvas.pushSprite(0, 0);
}

void DisplayManager::drawHeader(const char* title) {
    // Draw top horizontal line
    _canvas.drawFastHLine(0, 14, 240, COLOR_CYAN);
    
    // Draw left-aligned app prefix in Green, and right-aligned title in Cyan
    _canvas.setTextSize(1);
    _canvas.setTextColor(COLOR_GREEN, COLOR_BG);
    _canvas.setCursor(6, 3);
    _canvas.print("CYBER-TERMINAL");
    
    _canvas.setTextColor(COLOR_CYAN, COLOR_BG);
    int16_t titleWidth = _canvas.textWidth(title);
    _canvas.setCursor(234 - titleWidth, 3);
    _canvas.print(title);
}

void DisplayManager::drawFooter(const char* status) {
    // Draw bottom horizontal line
    _canvas.drawFastHLine(0, 120, 240, COLOR_CYAN);
    
    // Draw status on the left
    _canvas.setTextSize(1);
    _canvas.setTextColor(COLOR_GREEN, COLOR_BG);
    _canvas.setCursor(6, 124);
    _canvas.print("STATUS: ");
    
    _canvas.setTextColor(COLOR_CYAN, COLOR_BG);
    _canvas.print(status);
    
    // Draw standard terminal prompt symbol on the right
    _canvas.setTextColor(COLOR_GREEN, COLOR_BG);
    _canvas.setCursor(220, 124);
    _canvas.print(">_");
}
