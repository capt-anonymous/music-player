#include "ScreenManager.h"
#include <Arduino.h>

ScreenManager::ScreenManager() : _pendingPush(nullptr), _pendingReplace(nullptr), _pendingPop(false) {
}

ScreenManager::~ScreenManager() {
    for (auto* screen : _screenStack) {
        delete screen;
    }
    _screenStack.clear();
    
    if (_pendingPush) delete _pendingPush;
    if (_pendingReplace) delete _pendingReplace;
}

void ScreenManager::update(uint32_t dt) {
    Screen* active = getActiveScreen();
    if (active) {
        active->update(dt);
    }
}

void ScreenManager::draw(M5Canvas& canvas) {
    Screen* active = getActiveScreen();
    if (active) {
        active->draw(canvas);
    }
}

void ScreenManager::handleKey(const KeyInput& key) {
    Screen* active = getActiveScreen();
    if (active) {
        active->handleKey(key);
    }
}

void ScreenManager::pushScreen(Screen* screen) {
    if (_pendingPush) {
        delete _pendingPush;
    }
    _pendingPush = screen;
}

void ScreenManager::popScreen() {
    _pendingPop = true;
}

void ScreenManager::replaceScreen(Screen* screen) {
    if (_pendingReplace) {
        delete _pendingReplace;
    }
    _pendingReplace = screen;
}

Screen* ScreenManager::getActiveScreen() const {
    if (_screenStack.empty()) {
        return nullptr;
    }
    return _screenStack.back();
}

void ScreenManager::executePush(Screen* screen) {
    if (!screen) return;
    screen->setManager(this);
    screen->init();
    _screenStack.push_back(screen);
}

void ScreenManager::executePop() {
    if (!_screenStack.empty()) {
        Screen* top = _screenStack.back();
        _screenStack.pop_back();
        delete top;
    }
}

void ScreenManager::executeReplace(Screen* screen) {
    if (!screen) return;
    if (!_screenStack.empty()) {
        Screen* top = _screenStack.back();
        _screenStack.pop_back();
        delete top;
    }
    executePush(screen);
}

void ScreenManager::processPendingTransitions() {
    if (_pendingReplace) {
        executeReplace(_pendingReplace);
        _pendingReplace = nullptr;
    }
    if (_pendingPop) {
        executePop();
        _pendingPop = false;
    }
    if (_pendingPush) {
        executePush(_pendingPush);
        _pendingPush = nullptr;
    }
}
