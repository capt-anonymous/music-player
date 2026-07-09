#ifndef KEYBOARD_MANAGER_H
#define KEYBOARD_MANAGER_H

#include <M5Cardputer.h>
#include <vector>

enum class CardputerKey {
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    ENTER,
    ESC,
    TAB,
    BACKSPACE,
    CHAR
};

struct KeyInput {
    CardputerKey keyType;
    char character;
};

class KeyboardManager {
public:
    KeyboardManager();
    void update();
    bool hasKey();
    KeyInput getNextKey();

private:
    std::vector<KeyInput> _inputQueue;
    std::vector<Point2D_t> _prevKeys;
    bool _prevFn;
    bool _prevShift;
    bool _prevCtrl;
    
    bool isKeyInList(const Point2D_t& key, const std::vector<Point2D_t>& list);
};

#endif
