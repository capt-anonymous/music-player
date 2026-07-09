#include "KeyboardManager.h"
#include <Arduino.h>

KeyboardManager::KeyboardManager() : _prevFn(false), _prevShift(false), _prevCtrl(false) {
    _prevKeys.clear();
}

bool KeyboardManager::isKeyInList(const Point2D_t& key, const std::vector<Point2D_t>& list) {
    for (const auto& item : list) {
        if (item.x == key.x && item.y == key.y) {
            return true;
        }
    }
    return false;
}

void KeyboardManager::update() {
    auto& kb = M5Cardputer.Keyboard;
    auto state = kb.keysState();
    const auto& currentKeys = kb.keyList();
    
    for (const auto& key : currentKeys) {
        if (!isKeyInList(key, _prevKeys)) {
            // New key press detected (rising edge)
            KeyValue_t val = kb.getKeyValue(key);
            char first = val.value_first;
            char second = val.value_second;
            
            KeyInput input;
            input.keyType = CardputerKey::NONE;
            input.character = 0;
            
            // Check modifier layers
            if (state.fn) {
                // Fn mapping (arrows and escape)
                if (first == ';') {
                    input.keyType = CardputerKey::UP;
                } else if (first == '/') {
                    input.keyType = CardputerKey::DOWN;
                } else if (first == ',') {
                    input.keyType = CardputerKey::LEFT;
                } else if (first == '.') {
                    input.keyType = CardputerKey::RIGHT;
                } else if (first == '`') {
                    input.keyType = CardputerKey::ESC;
                }
            }
            
            // If it wasn't handled by Fn layer, check normal mappings
            if (input.keyType == CardputerKey::NONE) {
                if (first == KEY_ENTER) {
                    input.keyType = CardputerKey::ENTER;
                } else if (first == KEY_BACKSPACE) {
                    input.keyType = CardputerKey::BACKSPACE;
                } else if (first == KEY_TAB) {
                    input.keyType = CardputerKey::TAB;
                } else {
                    // Alphanumeric character (including space, letters, numbers, symbols)
                    char activeChar = (state.shift || kb.capslocked()) ? second : first;
                    if (activeChar >= 32 && activeChar <= 126) {
                        input.keyType = CardputerKey::CHAR;
                        input.character = activeChar;
                    }
                }
            }
            
            if (input.keyType != CardputerKey::NONE) {
                _inputQueue.push_back(input);
            }
        }
    }
    
    // Store states for next tick comparison
    _prevKeys = currentKeys;
    _prevFn = state.fn;
    _prevShift = state.shift;
    _prevCtrl = state.ctrl;
}

bool KeyboardManager::hasKey() {
    return !_inputQueue.empty();
}

KeyInput KeyboardManager::getNextKey() {
    if (_inputQueue.empty()) {
        return {CardputerKey::NONE, 0};
    }
    KeyInput key = _inputQueue.front();
    _inputQueue.erase(_inputQueue.begin());
    return key;
}
