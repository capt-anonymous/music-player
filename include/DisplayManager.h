#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <M5Cardputer.h>

class DisplayManager {
public:
    // Cyberpunk Theme Colors (RGB565)
    static constexpr uint16_t COLOR_BG = 0x0000;         // Pure Black
    static constexpr uint16_t COLOR_GREEN = 0x07E0;      // Cyber Green
    static constexpr uint16_t COLOR_CYAN = 0x07FF;       // Neon Cyan
    static constexpr uint16_t COLOR_GRAY = 0x7BEF;       // Dark Gray
    static constexpr uint16_t COLOR_RED = 0xF800;        // Error Red
    static constexpr uint16_t COLOR_SEL_BG = 0x01A6;     // Selection background (dark cyan-blue)
    static constexpr uint16_t COLOR_SEL_FG = 0xFFFF;     // Selection foreground (white)

    DisplayManager();
    void begin();
    
    M5Canvas& getCanvas() { return _canvas; }
    void clear();
    void publish();

    // Reusable styling components
    void drawHeader(const char* title);
    void drawFooter(const char* status);
    void drawAlbumArt(const String& localPath, int x, int y, int w = 96, int h = 96);

private:
    M5Canvas _canvas;
};

#endif
