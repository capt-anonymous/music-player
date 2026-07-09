#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include "KeyboardManager.h"
#include "DisplayManager.h"
#include <vector>

class ScreenManager;

class Screen {
public:
    virtual ~Screen() {}
    virtual void init() = 0;
    virtual void update(uint32_t dt) = 0;
    virtual void draw(M5Canvas& canvas) = 0;
    virtual void handleKey(const KeyInput& key) = 0;
    
    void setManager(ScreenManager* manager) { _manager = manager; }
    
protected:
    ScreenManager* _manager = nullptr;
};

class ScreenManager {
public:
    ScreenManager();
    ~ScreenManager();
    
    void update(uint32_t dt);
    void draw(M5Canvas& canvas);
    void handleKey(const KeyInput& key);
    
    // Deferred transitions to prevent screen deletion during its own update/event processing
    void pushScreen(Screen* screen);
    void popScreen();
    void replaceScreen(Screen* screen);
    
    void processPendingTransitions();
    Screen* getActiveScreen() const;

private:
    std::vector<Screen*> _screenStack;
    
    Screen* _pendingPush;
    Screen* _pendingReplace;
    bool _pendingPop;
    
    void executePush(Screen* screen);
    void executePop();
    void executeReplace(Screen* screen);
};

#endif
