#ifndef PLACEHOLDER_SCREENS_H
#define PLACEHOLDER_SCREENS_H

#include "ScreenManager.h"

class JellyfinLoginScreen : public Screen {
public:
    virtual void init() override {}
    virtual void update(uint32_t dt) override {}
    virtual void draw(M5Canvas& canvas) override {
        canvas.fillScreen(DisplayManager::COLOR_BG);
        canvas.drawFastHLine(0, 14, 240, DisplayManager::COLOR_CYAN);
        canvas.setTextColor(DisplayManager::COLOR_CYAN);
        canvas.setCursor(6, 3);
        canvas.print("JELLYFIN LOGIN");
        
        canvas.setTextColor(DisplayManager::COLOR_GREEN);
        canvas.setCursor(10, 30);
        canvas.println("[JELLYFIN SERVER LINKAGE]");
        canvas.println();
        canvas.println(" HOST:     --");
        canvas.println(" USERNAME: --");
        canvas.println(" STATUS:   OFFLINE");
        canvas.println();
        canvas.setTextColor(DisplayManager::COLOR_GRAY);
        canvas.println(" Press Backspace to return...");
    }
    virtual void handleKey(const KeyInput& key) override {
        if (key.keyType == CardputerKey::ESC || key.keyType == CardputerKey::BACKSPACE) {
            _manager->popScreen();
        }
    }
};

class MusicLibraryScreen : public Screen {
public:
    virtual void init() override {}
    virtual void update(uint32_t dt) override {}
    virtual void draw(M5Canvas& canvas) override {
        canvas.fillScreen(DisplayManager::COLOR_BG);
        canvas.drawFastHLine(0, 14, 240, DisplayManager::COLOR_CYAN);
        canvas.setTextColor(DisplayManager::COLOR_CYAN);
        canvas.setCursor(6, 3);
        canvas.print("MUSIC LIBRARY");
        
        canvas.setTextColor(DisplayManager::COLOR_GREEN);
        canvas.setCursor(10, 30);
        canvas.println("[CYBER AUDIO CORE]");
        canvas.println();
        canvas.println(" DB INDEX:   0 TRACKS");
        canvas.println(" QUEUE STATE: EMPTY");
        canvas.println();
        canvas.setTextColor(DisplayManager::COLOR_GRAY);
        canvas.println(" Press Backspace to return...");
    }
    virtual void handleKey(const KeyInput& key) override {
        if (key.keyType == CardputerKey::ESC || key.keyType == CardputerKey::BACKSPACE) {
            _manager->popScreen();
        }
    }
};

class SettingsScreen : public Screen {
public:
    virtual void init() override {}
    virtual void update(uint32_t dt) override {}
    virtual void draw(M5Canvas& canvas) override {
        canvas.fillScreen(DisplayManager::COLOR_BG);
        canvas.drawFastHLine(0, 14, 240, DisplayManager::COLOR_CYAN);
        canvas.setTextColor(DisplayManager::COLOR_CYAN);
        canvas.setCursor(6, 3);
        canvas.print("SETTINGS");
        
        canvas.setTextColor(DisplayManager::COLOR_GREEN);
        canvas.setCursor(10, 30);
        canvas.println("[TERMINAL PREFERENCES]");
        canvas.println();
        canvas.println(" AUDIO BUFF:  128 KB");
        canvas.println(" BRIGHTNESS:  80%");
        canvas.println(" VOL LIMIT:   90%");
        canvas.println();
        canvas.setTextColor(DisplayManager::COLOR_GRAY);
        canvas.println(" Press Backspace to return...");
    }
    virtual void handleKey(const KeyInput& key) override {
        if (key.keyType == CardputerKey::ESC || key.keyType == CardputerKey::BACKSPACE) {
            _manager->popScreen();
        }
    }
};

class AboutScreen : public Screen {
public:
    virtual void init() override {}
    virtual void update(uint32_t dt) override {}
    virtual void draw(M5Canvas& canvas) override {
        canvas.fillScreen(DisplayManager::COLOR_BG);
        canvas.drawFastHLine(0, 14, 240, DisplayManager::COLOR_CYAN);
        canvas.setTextColor(DisplayManager::COLOR_CYAN);
        canvas.setCursor(6, 3);
        canvas.print("ABOUT TERMINAL");
        
        canvas.setTextColor(DisplayManager::COLOR_GREEN);
        canvas.setCursor(10, 25);
        canvas.println("CYBER MUSIC TERMINAL");
        canvas.setTextSize(1);
        canvas.println("Jellyfin Audio Client");
        canvas.println("Created by Antigravity AI");
        canvas.println();
        canvas.setTextColor(DisplayManager::COLOR_CYAN);
        canvas.println("Hardware: M5Cardputer ADV");
        canvas.println("Framework: ESP32 Arduino Core");
        canvas.println();
        canvas.setTextColor(DisplayManager::COLOR_GRAY);
        canvas.println("Press Backspace to return...");
    }
    virtual void handleKey(const KeyInput& key) override {
        if (key.keyType == CardputerKey::ESC || key.keyType == CardputerKey::BACKSPACE) {
            _manager->popScreen();
        }
    }
};

#endif
