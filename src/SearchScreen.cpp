#include "SearchScreen.h"
#include "PlaybackScreen.h"
#include "App.h"
#include <Arduino.h>

SearchScreen::SearchScreen(JellyfinClient* client)
    : _jellyfinClient(client),
      _queryBuffer(""),
      _focusIndex(0),
      _selectedIndex(0),
      _scrollOffset(0),
      _isLoading(false),
      _hasError(false),
      _animCounter(0) {
}

void SearchScreen::init() {
    _items.clear();
    _queryBuffer = "";
    _focusIndex = 0;
    _selectedIndex = 0;
    _scrollOffset = 0;
    _isLoading = false;
    _hasError = false;
    _animCounter = 0;
}

void SearchScreen::update(uint32_t dt) {
    _animCounter += dt;
}

void SearchScreen::performSearch() {
    if (_queryBuffer.length() == 0) return;
    
    _items.clear();
    _isLoading = true;
    _hasError = false;
    _selectedIndex = 0;
    _scrollOffset = 0;
    
    String jsonResponse;
    if (_jellyfinClient->searchItems(_queryBuffer, jsonResponse)) {
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, jsonResponse);
        if (!err) {
            JsonArray arr = doc["Items"].as<JsonArray>();
            for (JsonObject item : arr) {
                LibraryItem li;
                String type = item["Type"].as<String>();
                String name = item["Name"].as<String>();
                
                li.id = item["Id"].as<String>();
                li.detail = type; // Store item type in detail for routing on select
                
                // Format display text based on type
                if (type == "Audio") {
                    li.name = "[T] " + name;
                } else if (type == "MusicAlbum") {
                    li.name = "[Al] " + name;
                } else if (type == "MusicArtist") {
                    li.name = "[Ar] " + name;
                } else {
                    li.name = name;
                }
                
                _items.push_back(li);
            }
            _isLoading = false;
        } else {
            _hasError = true;
            _isLoading = false;
            Serial.println("[Search] JSON parse error");
        }
    } else {
        _hasError = true;
        _isLoading = false;
        Serial.println("[Search] HTTP request failed");
    }
}

void SearchScreen::draw(M5Canvas& canvas) {
    canvas.fillScreen(DisplayManager::COLOR_BG);
    
    // Header
    canvas.drawFastHLine(0, 14, 240, DisplayManager::COLOR_CYAN);
    canvas.setTextColor(DisplayManager::COLOR_CYAN);
    canvas.setCursor(6, 3);
    canvas.print("SEARCH MUSIC LIBRARY");
    
    // Query search box
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.setCursor(10, 20);
    canvas.print("FIND:");
    
    int boxX = 45;
    int boxY = 17;
    int boxW = 185;
    int boxH = 13;
    
    if (_focusIndex == 0) {
        canvas.drawRect(boxX, boxY, boxW, boxH, DisplayManager::COLOR_CYAN);
    } else {
        canvas.drawRect(boxX, boxY, boxW, boxH, DisplayManager::COLOR_GRAY);
    }
    
    canvas.setCursor(boxX + 4, boxY + 3);
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.print(_queryBuffer);
    if (_focusIndex == 0 && (_animCounter / 300) % 2 == 0) {
        canvas.print("_");
    }
    
    // Results area
    int visibleRows = 6;
    int itemHeight = 14;
    int startY = 35;
    
    if (_isLoading) {
        canvas.setTextColor(DisplayManager::COLOR_GREEN);
        canvas.setCursor(10, 60);
        canvas.print("Searching database...");
    } else if (_hasError) {
        canvas.setTextColor(DisplayManager::COLOR_RED);
        canvas.setCursor(10, 60);
        canvas.print("Search query failed.");
    } else if (_items.empty() && _queryBuffer.length() > 0) {
        canvas.setTextColor(DisplayManager::COLOR_GRAY);
        canvas.setCursor(10, 60);
        canvas.print("No matching items found.");
    } else if (!_items.empty()) {
        int drawY = startY;
        for (int i = 0; i < visibleRows; ++i) {
            size_t idx = _scrollOffset + i;
            if (idx >= _items.size()) break;
            
            if (_focusIndex == 1 && idx == _selectedIndex) {
                canvas.fillRect(8, drawY - 2, 218, 12, DisplayManager::COLOR_SEL_BG);
                canvas.setTextColor(DisplayManager::COLOR_SEL_FG);
            } else {
                canvas.setTextColor(DisplayManager::COLOR_GREEN);
            }
            
            canvas.setCursor(12, drawY);
            String itemText = _items[idx].name;
            if (itemText.length() > 30) itemText = itemText.substring(0, 27) + "...";
            canvas.print(itemText);
            
            drawY += itemHeight;
        }
        
        // Draw scrollbar if list has focus or items exist
        if (_focusIndex == 1) {
            // Draw standard scrollbar track and thumb on the right
            int barX = 233;
            int barW = 3;
            int scrollbarH = 80;
            canvas.drawFastVLine(barX, startY, scrollbarH, DisplayManager::COLOR_GRAY);
            
            float viewFrac = (float)visibleRows / _items.size();
            int thumbH = (int)(viewFrac * scrollbarH);
            if (thumbH < 10) thumbH = 10;
            
            float scrollFrac = (float)_selectedIndex / (_items.size() - 1);
            int thumbY = startY + (int)(scrollFrac * (scrollbarH - thumbH));
            canvas.fillRect(barX - 1, thumbY, barW + 2, thumbH, DisplayManager::COLOR_CYAN);
        }
    }
    
    // Footer
    canvas.drawFastHLine(0, 120, 240, DisplayManager::COLOR_CYAN);
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.setCursor(6, 124);
    if (_focusIndex == 0) {
        canvas.print("TYPE QUERY. Press [Enter] to Search / [Tab]");
    } else {
        canvas.print("NAV: [;/Arrows] ENTER: [Open] [Tab]: Input");
    }
    
    canvas.setTextColor(DisplayManager::COLOR_CYAN);
    canvas.setCursor(218, 124);
    canvas.print(">_");
}

void SearchScreen::handleKey(const KeyInput& key) {
    if (key.keyType == CardputerKey::TAB) {
        if (_focusIndex == 0) {
            if (!_items.empty()) {
                _focusIndex = 1; // switch to results list
            }
        } else {
            _focusIndex = 0; // switch back to query input
        }
    } else if (key.keyType == CardputerKey::ESC) {
        _manager->popScreen();
    } else if (key.keyType == CardputerKey::BACKSPACE) {
        if (_focusIndex == 0) {
            if (_queryBuffer.length() > 0) {
                _queryBuffer.remove(_queryBuffer.length() - 1);
            }
        } else {
            _manager->popScreen(); // Go back
        }
    } else if (key.keyType == CardputerKey::ENTER) {
        if (_focusIndex == 0) {
            performSearch();
            if (!_items.empty()) {
                _focusIndex = 1; // Auto focus results on search complete
            }
        } else if (_focusIndex == 1 && !_items.empty()) {
            // Route based on type stored in detail
            String type = _items[_selectedIndex].detail;
            String itemId = _items[_selectedIndex].id;
            // Extract display name by removing prefix "[X] " (4 characters)
            String name = _items[_selectedIndex].name.substring(5);
            
            if (type == "Audio") {
                extern App app;
                _manager->pushScreen(new PlaybackScreen(_jellyfinClient, &app.getAudio(), itemId, name));
            } else if (type == "MusicAlbum") {
                _manager->pushScreen(new SongsScreen(_jellyfinClient, itemId, name));
            } else if (type == "MusicArtist") {
                _manager->pushScreen(new AlbumsScreen(_jellyfinClient, itemId, name));
            }
        }
    } else if (key.keyType == CardputerKey::CHAR) {
        if (_focusIndex == 0) {
            _queryBuffer += key.character;
        }
    } else if (_focusIndex == 1 && !_items.empty()) {
        // Handle scrolling in results list using custom ; / / keys
        bool upPressed = (key.keyType == CardputerKey::UP) || (key.keyType == CardputerKey::CHAR && key.character == ';');
        bool downPressed = (key.keyType == CardputerKey::DOWN) || (key.keyType == CardputerKey::CHAR && key.character == '/');
        int visibleRows = 6;
        
        if (upPressed) {
            if (_selectedIndex > 0) {
                _selectedIndex--;
            } else {
                _selectedIndex = _items.size() - 1;
            }
            if (_selectedIndex < _scrollOffset) {
                _scrollOffset = _selectedIndex;
            } else if (_selectedIndex >= _scrollOffset + visibleRows) {
                _scrollOffset = _selectedIndex - visibleRows + 1;
            }
        } else if (downPressed) {
            if (_selectedIndex < _items.size() - 1) {
                _selectedIndex++;
            } else {
                _selectedIndex = 0;
            }
            if (_selectedIndex < _scrollOffset) {
                _scrollOffset = _selectedIndex;
            } else if (_selectedIndex >= _scrollOffset + visibleRows) {
                _scrollOffset = _selectedIndex - visibleRows + 1;
            }
        }
    }
}
