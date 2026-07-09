#ifndef LIBRARY_SCREENS_H
#define LIBRARY_SCREENS_H

#include "ScreenManager.h"
#include "JellyfinClient.h"
#include <vector>

struct LibraryItem {
    String name;
    String id;
    String detail;
};

class ArtistsScreen : public Screen {
public:
    ArtistsScreen(JellyfinClient* client);
    virtual void init() override;
    virtual void update(uint32_t dt) override;
    virtual void draw(M5Canvas& canvas) override;
    virtual void handleKey(const KeyInput& key) override;

private:
    JellyfinClient* _jellyfinClient;
    std::vector<LibraryItem> _items;
    size_t _selectedIndex;
    size_t _scrollOffset;
    bool _isLoading;
    bool _hasError;
    
    void fetchArtists();
};

class AlbumsScreen : public Screen {
public:
    AlbumsScreen(JellyfinClient* client);
    AlbumsScreen(JellyfinClient* client, const String& artistId, const String& artistName);
    virtual void init() override;
    virtual void update(uint32_t dt) override;
    virtual void draw(M5Canvas& canvas) override;
    virtual void handleKey(const KeyInput& key) override;

private:
    JellyfinClient* _jellyfinClient;
    String _artistId;
    String _artistName;
    std::vector<LibraryItem> _items;
    size_t _selectedIndex;
    size_t _scrollOffset;
    bool _isLoading;
    bool _hasError;
    
    void fetchAlbums();
};

class SongsScreen : public Screen {
public:
    SongsScreen(JellyfinClient* client);
    SongsScreen(JellyfinClient* client, const String& albumId, const String& albumName);
    virtual void init() override;
    virtual void update(uint32_t dt) override;
    virtual void draw(M5Canvas& canvas) override;
    virtual void handleKey(const KeyInput& key) override;

private:
    JellyfinClient* _jellyfinClient;
    String _albumId;
    String _albumName;
    std::vector<LibraryItem> _items;
    size_t _selectedIndex;
    size_t _scrollOffset;
    bool _isLoading;
    bool _hasError;
    
    void fetchSongs();
};

#endif
