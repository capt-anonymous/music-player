#ifndef STORAGE_MANAGER_H
#define STORAGE_MANAGER_H

#include <SPI.h>
#include <SD.h>
#include <FS.h>

class StorageManager {
public:
    // Physical SPI Pin Mappings for M5Cardputer microSD slot
    static constexpr int PIN_SCK = 40;
    static constexpr int PIN_MISO = 39;
    static constexpr int PIN_MOSI = 14;
    static constexpr int PIN_CS = 12;

    StorageManager();
    bool begin();
    
    bool isReady() const { return _isReady; }
    
    bool exists(const char* path);
    bool deleteFile(const char* path);
    void createDir(const char* path);

private:
    bool _isReady;
};

#endif
