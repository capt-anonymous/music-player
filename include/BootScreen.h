#ifndef BOOT_SCREEN_H
#define BOOT_SCREEN_H

#include "ScreenManager.h"
#include <vector>

class BootScreen : public Screen {
public:
    BootScreen();
    virtual void init() override;
    virtual void update(uint32_t dt) override;
    virtual void draw(M5Canvas& canvas) override;
    virtual void handleKey(const KeyInput& key) override;

private:
    std::vector<const char*> _logLines;
    size_t _visibleLinesCount;
    uint32_t _timeAccumulator;
    uint32_t _logIntervalMs;
    uint32_t _holdTimeMs;
    bool _isDone;
    
    float _progress;
};

#endif
