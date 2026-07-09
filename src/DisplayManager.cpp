#include "DisplayManager.h"
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

void DisplayManager::drawAlbumArt(const String& localPath, int x, int y, int w, int h) {
    // If cache file is available on SD card, decode it directly
    if (localPath.length() > 0 && SD.exists(localPath.c_str())) {
        _canvas.drawJpgFile((fs::FS&)SD, localPath.c_str(), x, y, w, h);
    } else {
        // Draw cyberpunk decorative placeholder box
        _canvas.drawRect(x, y, w, h, COLOR_GRAY);
        _canvas.drawRect(x + 2, y + 2, w - 4, h - 4, COLOR_CYAN);
        
        // Draw a green vector music note icon inside the placeholder box
        int noteX = x + (w / 2) + 2;
        int noteY = y + (h / 2) - 10;
        _canvas.fillRect(noteX, noteY, 2, 14, COLOR_GREEN);
        _canvas.fillCircle(noteX - 2, noteY + 12, 3, COLOR_GREEN);
        
        _canvas.setTextColor(COLOR_CYAN, COLOR_BG);
        _canvas.setCursor(x + 8, y + 8);
        _canvas.print("NO ART");
    }
}
