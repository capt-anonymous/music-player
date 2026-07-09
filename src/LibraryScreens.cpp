#include "LibraryScreens.h"
#include "App.h"
#include <Arduino.h>

// Helper to draw standard scrollbar on the right side of the list
static void drawListScrollbar(M5Canvas& canvas, int startY, int height, size_t selectedIdx, size_t totalItems, int visibleRows) {
    if (totalItems <= (size_t)visibleRows) return; // No scrollbar needed if everything fits
    
    int barX = 233;
    int barW = 3;
    
    // Draw track
    canvas.drawFastVLine(barX, startY, height, DisplayManager::COLOR_GRAY);
    
    // Calculate thumb height and vertical offset
    float viewFrac = (float)visibleRows / totalItems;
    int thumbH = (int)(viewFrac * height);
    if (thumbH < 10) thumbH = 10; // Enforce minimum thumb size
    
    float scrollFrac = (float)selectedIdx / (totalItems - 1);
    int thumbY = startY + (int)(scrollFrac * (height - thumbH));
    
    // Draw thumb
    canvas.fillRect(barX - 1, thumbY, barW + 2, thumbH, DisplayManager::COLOR_CYAN);
}

// ==========================================
// ARTISTS SCREEN
// ==========================================

ArtistsScreen::ArtistsScreen(JellyfinClient* client)
    : _jellyfinClient(client), _selectedIndex(0), _scrollOffset(0), _isLoading(true), _hasError(false) {
}

void ArtistsScreen::init() {
    _items.clear();
    _selectedIndex = 0;
    _scrollOffset = 0;
    _isLoading = true;
    _hasError = false;
    fetchArtists();
}

void ArtistsScreen::update(uint32_t dt) {
}

void ArtistsScreen::fetchArtists() {
    String jsonResponse;
    if (_jellyfinClient->getArtists(jsonResponse, 30)) {
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, jsonResponse);
        if (!err) {
            JsonArray arr = doc["Items"].as<JsonArray>();
            for (JsonObject item : arr) {
                LibraryItem li;
                li.name = item["Name"].as<String>();
                li.id = item["Id"].as<String>();
                li.detail = "";
                _items.push_back(li);
            }
            _isLoading = false;
        } else {
            _hasError = true;
            _isLoading = false;
            Serial.println("[Artists] JSON parse error");
        }
    } else {
        _hasError = true;
        _isLoading = false;
        Serial.println("[Artists] HTTP request failed");
    }
}

void ArtistsScreen::draw(M5Canvas& canvas) {
    canvas.fillScreen(DisplayManager::COLOR_BG);
    
    // Header
    canvas.drawFastHLine(0, 14, 240, DisplayManager::COLOR_CYAN);
    canvas.setTextColor(DisplayManager::COLOR_CYAN);
    canvas.setCursor(6, 3);
    canvas.print("ARTISTS BROWSER");
    
    int visibleRows = 7;
    int itemHeight = 14;
    int startY = 20;
    
    if (_isLoading) {
        canvas.setTextColor(DisplayManager::COLOR_GREEN);
        canvas.setCursor(10, 50);
        canvas.print("Loading artists...");
    } else if (_hasError) {
        canvas.setTextColor(DisplayManager::COLOR_RED);
        canvas.setCursor(10, 50);
        canvas.print("Failed to load library data.");
    } else if (_items.empty()) {
        canvas.setTextColor(DisplayManager::COLOR_GRAY);
        canvas.setCursor(10, 50);
        canvas.print("No artists found.");
    } else {
        // Draw visible rows
        int drawY = startY;
        for (int i = 0; i < visibleRows; ++i) {
            size_t idx = _scrollOffset + i;
            if (idx >= _items.size()) break;
            
            if (idx == _selectedIndex) {
                canvas.fillRect(8, drawY - 2, 218, 12, DisplayManager::COLOR_SEL_BG);
                canvas.setTextColor(DisplayManager::COLOR_SEL_FG);
            } else {
                canvas.setTextColor(DisplayManager::COLOR_GREEN);
            }
            
            canvas.setCursor(12, drawY);
            // Truncate name if too long
            String name = _items[idx].name;
            if (name.length() > 30) name = name.substring(0, 27) + "...";
            canvas.print(name);
            
            drawY += itemHeight;
        }
        
        // Draw Scrollbar
        drawListScrollbar(canvas, startY, 95, _selectedIndex, _items.size(), visibleRows);
    }
    
    // Footer
    canvas.drawFastHLine(0, 120, 240, DisplayManager::COLOR_CYAN);
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.setCursor(6, 124);
    canvas.print("NAV: [;/Arrows] ENTER: [Open] BACK: [Backsp]");
    
    canvas.setTextColor(DisplayManager::COLOR_CYAN);
    canvas.setCursor(218, 124);
    canvas.print(">_");
}

void ArtistsScreen::handleKey(const KeyInput& key) {
    bool upPressed = (key.keyType == CardputerKey::UP) || (key.keyType == CardputerKey::CHAR && key.character == ';');
    bool downPressed = (key.keyType == CardputerKey::DOWN) || (key.keyType == CardputerKey::CHAR && key.character == '/');
    
    int visibleRows = 7;
    
    if (upPressed && !_items.empty()) {
        if (_selectedIndex > 0) {
            _selectedIndex--;
        } else {
            _selectedIndex = _items.size() - 1;
        }
        // Adjust scroll window
        if (_selectedIndex < _scrollOffset) {
            _scrollOffset = _selectedIndex;
        } else if (_selectedIndex >= _scrollOffset + visibleRows) {
            _scrollOffset = _selectedIndex - visibleRows + 1;
        }
    } else if (downPressed && !_items.empty()) {
        if (_selectedIndex < _items.size() - 1) {
            _selectedIndex++;
        } else {
            _selectedIndex = 0;
        }
        // Adjust scroll window
        if (_selectedIndex < _scrollOffset) {
            _scrollOffset = _selectedIndex;
        } else if (_selectedIndex >= _scrollOffset + visibleRows) {
            _scrollOffset = _selectedIndex - visibleRows + 1;
        }
    } else if (key.keyType == CardputerKey::ESC || key.keyType == CardputerKey::BACKSPACE) {
        _manager->popScreen();
    } else if (key.keyType == CardputerKey::ENTER && !_items.empty()) {
        _manager->pushScreen(new AlbumsScreen(_jellyfinClient, _items[_selectedIndex].id, _items[_selectedIndex].name));
    }
}

// ==========================================
// ALBUMS SCREEN
// ==========================================

AlbumsScreen::AlbumsScreen(JellyfinClient* client)
    : _jellyfinClient(client), _artistId(""), _artistName(""), _selectedIndex(0), _scrollOffset(0), _isLoading(true), _hasError(false) {
}

AlbumsScreen::AlbumsScreen(JellyfinClient* client, const String& artistId, const String& artistName)
    : _jellyfinClient(client), _artistId(artistId), _artistName(artistName), _selectedIndex(0), _scrollOffset(0), _isLoading(true), _hasError(false) {
}

void AlbumsScreen::init() {
    _items.clear();
    _selectedIndex = 0;
    _scrollOffset = 0;
    _isLoading = true;
    _hasError = false;
    fetchAlbums();
}

void AlbumsScreen::update(uint32_t dt) {
}

void AlbumsScreen::fetchAlbums() {
    String jsonResponse;
    bool success = false;
    if (_artistId.length() > 0) {
        success = _jellyfinClient->getArtistAlbums(_artistId, jsonResponse);
    } else {
        success = _jellyfinClient->getAlbums(jsonResponse, 30);
    }
    
    if (success) {
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, jsonResponse);
        if (!err) {
            JsonArray arr = doc["Items"].as<JsonArray>();
            for (JsonObject item : arr) {
                LibraryItem li;
                li.name = item["Name"].as<String>();
                li.id = item["Id"].as<String>();
                if (item.containsKey("AlbumArtist")) {
                    li.detail = item["AlbumArtist"].as<String>();
                } else {
                    li.detail = "";
                }
                _items.push_back(li);
            }
            _isLoading = false;
        } else {
            _hasError = true;
            _isLoading = false;
            Serial.println("[Albums] JSON parse error");
        }
    } else {
        _hasError = true;
        _isLoading = false;
        Serial.println("[Albums] HTTP request failed");
    }
}

void AlbumsScreen::draw(M5Canvas& canvas) {
    canvas.fillScreen(DisplayManager::COLOR_BG);
    
    // Header
    canvas.drawFastHLine(0, 14, 240, DisplayManager::COLOR_CYAN);
    canvas.setTextColor(DisplayManager::COLOR_CYAN);
    canvas.setCursor(6, 3);
    if (_artistName.length() > 0) {
        String title = _artistName + " ALBUMS";
        if (title.length() > 22) title = title.substring(0, 19) + "...";
        canvas.print(title);
    } else {
        canvas.print("ALBUMS BROWSER");
    }
    
    int visibleRows = 7;
    int itemHeight = 14;
    int startY = 20;
    
    if (_isLoading) {
        canvas.setTextColor(DisplayManager::COLOR_GREEN);
        canvas.setCursor(10, 50);
        canvas.print("Loading albums...");
    } else if (_hasError) {
        canvas.setTextColor(DisplayManager::COLOR_RED);
        canvas.setCursor(10, 50);
        canvas.print("Failed to load album data.");
    } else if (_items.empty()) {
        canvas.setTextColor(DisplayManager::COLOR_GRAY);
        canvas.setCursor(10, 50);
        canvas.print("No albums found.");
    } else {
        int drawY = startY;
        for (int i = 0; i < visibleRows; ++i) {
            size_t idx = _scrollOffset + i;
            if (idx >= _items.size()) break;
            
            if (idx == _selectedIndex) {
                canvas.fillRect(8, drawY - 2, 218, 12, DisplayManager::COLOR_SEL_BG);
                canvas.setTextColor(DisplayManager::COLOR_SEL_FG);
            } else {
                canvas.setTextColor(DisplayManager::COLOR_GREEN);
            }
            
            canvas.setCursor(12, drawY);
            // Print album name, and artist detail if global mode
            String itemText = _items[idx].name;
            if (_artistId.length() == 0 && _items[idx].detail.length() > 0) {
                itemText += " - " + _items[idx].detail;
            }
            if (itemText.length() > 30) itemText = itemText.substring(0, 27) + "...";
            canvas.print(itemText);
            
            drawY += itemHeight;
        }
        
        drawListScrollbar(canvas, startY, 95, _selectedIndex, _items.size(), visibleRows);
    }
    
    // Footer
    canvas.drawFastHLine(0, 120, 240, DisplayManager::COLOR_CYAN);
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.setCursor(6, 124);
    canvas.print("NAV: [;/Arrows] ENTER: [Open] BACK: [Backsp]");
    
    canvas.setTextColor(DisplayManager::COLOR_CYAN);
    canvas.setCursor(218, 124);
    canvas.print(">_");
}

void AlbumsScreen::handleKey(const KeyInput& key) {
    bool upPressed = (key.keyType == CardputerKey::UP) || (key.keyType == CardputerKey::CHAR && key.character == ';');
    bool downPressed = (key.keyType == CardputerKey::DOWN) || (key.keyType == CardputerKey::CHAR && key.character == '/');
    
    int visibleRows = 7;
    
    if (upPressed && !_items.empty()) {
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
    } else if (downPressed && !_items.empty()) {
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
    } else if (key.keyType == CardputerKey::ESC || key.keyType == CardputerKey::BACKSPACE) {
        _manager->popScreen();
    } else if (key.keyType == CardputerKey::ENTER && !_items.empty()) {
        _manager->pushScreen(new SongsScreen(_jellyfinClient, _items[_selectedIndex].id, _items[_selectedIndex].name));
    }
}

// ==========================================
// SONGS SCREEN
// ==========================================

SongsScreen::SongsScreen(JellyfinClient* client)
    : _jellyfinClient(client), _albumId(""), _albumName(""), _selectedIndex(0), _scrollOffset(0), _isLoading(true), _hasError(false) {
}

SongsScreen::SongsScreen(JellyfinClient* client, const String& albumId, const String& albumName)
    : _jellyfinClient(client), _albumId(albumId), _albumName(albumName), _selectedIndex(0), _scrollOffset(0), _isLoading(true), _hasError(false) {
}

void SongsScreen::init() {
    _items.clear();
    _selectedIndex = 0;
    _scrollOffset = 0;
    _isLoading = true;
    _hasError = false;
    fetchSongs();
}

void SongsScreen::update(uint32_t dt) {
}

void SongsScreen::fetchSongs() {
    String jsonResponse;
    bool success = false;
    if (_albumId.length() > 0) {
        success = _jellyfinClient->getAlbumSongs(_albumId, jsonResponse);
    } else {
        success = _jellyfinClient->getSongs(jsonResponse, 30);
    }
    
    if (success) {
        JsonDocument doc;
        DeserializationError err = deserializeJson(doc, jsonResponse);
        if (!err) {
            JsonArray arr = doc["Items"].as<JsonArray>();
            for (JsonObject item : arr) {
                LibraryItem li;
                li.name = item["Name"].as<String>();
                li.id = item["Id"].as<String>();
                li.detail = "";
                _items.push_back(li);
            }
            _isLoading = false;
        } else {
            _hasError = true;
            _isLoading = false;
            Serial.println("[Songs] JSON parse error");
        }
    } else {
        _hasError = true;
        _isLoading = false;
        Serial.println("[Songs] HTTP request failed");
    }
}

void SongsScreen::draw(M5Canvas& canvas) {
    canvas.fillScreen(DisplayManager::COLOR_BG);
    
    // Header
    canvas.drawFastHLine(0, 14, 240, DisplayManager::COLOR_CYAN);
    canvas.setTextColor(DisplayManager::COLOR_CYAN);
    canvas.setCursor(6, 3);
    if (_albumName.length() > 0) {
        String title = _albumName + " SONGS";
        if (title.length() > 22) title = title.substring(0, 19) + "...";
        canvas.print(title);
    } else {
        canvas.print("ALL SONGS BROWSER");
    }
    
    int visibleRows = 7;
    int itemHeight = 14;
    int startY = 20;
    
    if (_isLoading) {
        canvas.setTextColor(DisplayManager::COLOR_GREEN);
        canvas.setCursor(10, 50);
        canvas.print("Loading tracks...");
    } else if (_hasError) {
        canvas.setTextColor(DisplayManager::COLOR_RED);
        canvas.setCursor(10, 50);
        canvas.print("Failed to load tracks.");
    } else if (_items.empty()) {
        canvas.setTextColor(DisplayManager::COLOR_GRAY);
        canvas.setCursor(10, 50);
        canvas.print("No tracks found.");
    } else {
        int drawY = startY;
        for (int i = 0; i < visibleRows; ++i) {
            size_t idx = _scrollOffset + i;
            if (idx >= _items.size()) break;
            
            if (idx == _selectedIndex) {
                canvas.fillRect(8, drawY - 2, 218, 12, DisplayManager::COLOR_SEL_BG);
                canvas.setTextColor(DisplayManager::COLOR_SEL_FG);
            } else {
                canvas.setTextColor(DisplayManager::COLOR_GREEN);
            }
            
            canvas.setCursor(12, drawY);
            // Print track number and track title
            canvas.print(String(idx + 1) + ". ");
            String songName = _items[idx].name;
            if (songName.length() > 26) songName = songName.substring(0, 23) + "...";
            canvas.print(songName);
            
            drawY += itemHeight;
        }
        
        drawListScrollbar(canvas, startY, 95, _selectedIndex, _items.size(), visibleRows);
    }
    
    // Footer
    canvas.drawFastHLine(0, 120, 240, DisplayManager::COLOR_CYAN);
    canvas.setTextColor(DisplayManager::COLOR_GREEN);
    canvas.setCursor(6, 124);
    canvas.print("NAV: [;/Arrows] ENTER: [Select] BACK: [Backsp]");
    
    canvas.setTextColor(DisplayManager::COLOR_CYAN);
    canvas.setCursor(218, 124);
    canvas.print(">_");
}

void SongsScreen::handleKey(const KeyInput& key) {
    bool upPressed = (key.keyType == CardputerKey::UP) || (key.keyType == CardputerKey::CHAR && key.character == ';');
    bool downPressed = (key.keyType == CardputerKey::DOWN) || (key.keyType == CardputerKey::CHAR && key.character == '/');
    
    int visibleRows = 7;
    
    if (upPressed && !_items.empty()) {
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
    } else if (downPressed && !_items.empty()) {
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
    } else if (key.keyType == CardputerKey::ESC || key.keyType == CardputerKey::BACKSPACE) {
        _manager->popScreen();
    } else if (key.keyType == CardputerKey::ENTER && !_items.empty()) {
        // Output track details to Serial for debugging. Audio streaming will link here in Milestone 6!
        Serial.print("[Songs] Selected Track to Play: ");
        Serial.print(_items[_selectedIndex].name);
        Serial.print(" | ID: ");
        Serial.println(_items[_selectedIndex].id);
        
        // Trigger a test image download for this track's ID!
        extern App app;
        String path = app.getAlbumArtManager().getArtworkPath(_items[_selectedIndex].id);
        Serial.print("[Songs] Artwork cached path: ");
        Serial.println(path);
    }
}
