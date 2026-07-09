#ifndef SEARCH_SCREEN_H
#define SEARCH_SCREEN_H

#include "ScreenManager.h"
#include "JellyfinClient.h"
#include "LibraryScreens.h"
#include <vector>

class SearchScreen : public Screen {
public:
    SearchScreen(JellyfinClient* client);
    virtual void init() override;
    virtual void update(uint32_t dt) override;
    virtual void draw(M5Canvas& canvas) override;
    virtual void handleKey(const KeyInput& key) override;

private:
    JellyfinClient* _jellyfinClient;
    String _queryBuffer;
    std::vector<LibraryItem> _items;
    
    int _focusIndex; // 0 = Query Input, 1 = Results List
    size_t _selectedIndex;
    size_t _scrollOffset;
    bool _isLoading;
    bool _hasError;
    uint32_t _animCounter;
    
    void performSearch();
};

#endif
