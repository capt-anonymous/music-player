#ifndef JELLYFIN_CLIENT_H
#define JELLYFIN_CLIENT_H

#include <Arduino.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Preferences.h>

enum class JellyfinStatus {
    LOGGED_OUT,
    AUTHENTICATING,
    LOGGED_IN,
    ERROR_CREDENTIALS,
    ERROR_SERVER,
    ERROR_NETWORK,
    ERROR_TIMEOUT
};

class JellyfinClient {
public:
    JellyfinClient();
    
    void begin();
    
    void login(const char* host, const char* username, const char* password);
    void logout();
    
    JellyfinStatus getStatus() const { return _status; }
    void setStatus(JellyfinStatus stat) { _status = stat; }
    
    String getHost() const { return _host; }
    String getUsername() const { return _username; }
    String getToken() const { return _token; }
    String getUserId() const { return _userId; }
    
    bool hasToken() const { return _token.length() > 0; }
    
    bool sendGetRequest(const String& endpoint, String& response);
    
    // Catalog browse helpers (pagination-limited to protect heap)
    bool getArtists(String& jsonOut, int limit = 30);
    bool getAlbums(String& jsonOut, int limit = 30);
    bool getSongs(String& jsonOut, int limit = 30);
    bool getArtistAlbums(const String& artistId, String& jsonOut);
    bool getAlbumSongs(const String& albumId, String& jsonOut);
    bool searchItems(const String& query, String& jsonOut);

private:
    JellyfinStatus _status;
    String _host;
    String _username;
    String _token;
    String _userId;
    
    Preferences _prefs;
    
    void saveSession();
    void loadSession();
    void clearSession();
    
    String getAuthHeader();
};

#endif
