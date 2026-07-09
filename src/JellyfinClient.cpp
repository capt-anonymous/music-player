#include "JellyfinClient.h"
#include <WiFi.h>

JellyfinClient::JellyfinClient() 
    : _status(JellyfinStatus::LOGGED_OUT), 
      _host(""), 
      _username(""), 
      _token(""), 
      _userId("") {
}

void JellyfinClient::begin() {
    loadSession();
    if (_host.length() > 0 && _token.length() > 0) {
        // If we have saved credentials, check connection viability
        Serial.print("[Jellyfin] Checking session viability for host: ");
        Serial.println(_host);
        
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[Jellyfin] Wi-Fi disconnected. Deferring verification.");
            _status = JellyfinStatus::LOGGED_OUT;
            return;
        }
        
        String response;
        // Verify token viability by fetching system/info (which requires auth)
        if (sendGetRequest("/System/Info", response)) {
            _status = JellyfinStatus::LOGGED_IN;
            Serial.println("[Jellyfin] Auto-login successful (session token valid)!");
        } else {
            Serial.println("[Jellyfin] Session token invalid or server offline.");
            _status = JellyfinStatus::LOGGED_OUT;
            clearSession(); // Clean stale session
        }
    }
}

String JellyfinClient::getAuthHeader() {
    // Format Emby client authorization details
    String header = "MediaBrowser Client=\"CyberMusicTerminal\", Device=\"M5Cardputer\", DeviceId=\"M5-STAMP-S3\", Version=\"1.0.0\"";
    if (_token.length() > 0) {
        header += ", Token=\"" + _token + "\"";
    }
    return header;
}

void JellyfinClient::login(const char* host, const char* username, const char* password) {
    _status = JellyfinStatus::AUTHENTICATING;
    
    // Check Wi-Fi state first
    if (WiFi.status() != WL_CONNECTED) {
        _status = JellyfinStatus::ERROR_NETWORK;
        Serial.println("[Jellyfin] Login failed: Wi-Fi offline");
        return;
    }
    
    // Sanitize host url: remove trailing slash if present
    String sanitizedHost = String(host);
    if (sanitizedHost.endsWith("/")) {
        sanitizedHost.remove(sanitizedHost.length() - 1);
    }
    
    HTTPClient http;
    String url = sanitizedHost + "/Users/AuthenticateByName";
    
    Serial.print("[Jellyfin] Authenticating against: ");
    Serial.println(url);
    
    http.begin(url);
    http.setTimeout(6000); // 6 seconds timeout
    
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-Emby-Authorization", getAuthHeader());
    
    // Create authentication JSON payload
    JsonDocument doc;
    doc["Username"] = username;
    doc["Pw"] = password;
    
    String payload;
    serializeJson(doc, payload);
    
    int httpCode = http.POST(payload);
    
    if (httpCode == 200) {
        String response = http.getString();
        
        JsonDocument resDoc;
        DeserializationError err = deserializeJson(resDoc, response);
        
        if (!err) {
            _token = resDoc["AccessToken"].as<String>();
            _userId = resDoc["User"]["Id"].as<String>();
            _host = sanitizedHost;
            _username = username;
            
            _status = JellyfinStatus::LOGGED_IN;
            saveSession();
            Serial.println("[Jellyfin] Authentication successful!");
        } else {
            _status = JellyfinStatus::ERROR_SERVER;
            Serial.println("[Jellyfin] Failed to parse JSON response");
        }
    } else if (httpCode == 401 || httpCode == 400) {
        _status = JellyfinStatus::ERROR_CREDENTIALS;
        Serial.print("[Jellyfin] Unauthorized: httpCode=");
        Serial.println(httpCode);
    } else {
        _status = JellyfinStatus::ERROR_SERVER;
        Serial.print("[Jellyfin] Connection failed: httpCode=");
        Serial.println(httpCode);
    }
    
    http.end();
}

void JellyfinClient::logout() {
    clearSession();
    _status = JellyfinStatus::LOGGED_OUT;
    Serial.println("[Jellyfin] Logged out.");
}

bool JellyfinClient::sendGetRequest(const String& endpoint, String& response) {
    if (WiFi.status() != WL_CONNECTED) {
        return false;
    }
    
    HTTPClient http;
    String url = _host + endpoint;
    
    http.begin(url);
    http.setTimeout(5000);
    http.addHeader("X-Emby-Authorization", getAuthHeader());
    if (_token.length() > 0) {
        http.addHeader("X-MediaBrowser-Token", _token);
    }
    
    int httpCode = http.GET();
    bool success = false;
    
    if (httpCode == 200) {
        response = http.getString();
        success = true;
    }
    
    http.end();
    return success;
}

void JellyfinClient::saveSession() {
    _prefs.begin("jf-config", false);
    _prefs.putString("host", _host);
    _prefs.putString("user", _username);
    _prefs.putString("token", _token);
    _prefs.putString("userid", _userId);
    _prefs.end();
}

void JellyfinClient::loadSession() {
    _prefs.begin("jf-config", true);
    _host = _prefs.getString("host", "");
    _username = _prefs.getString("user", "");
    _token = _prefs.getString("token", "");
    _userId = _prefs.getString("userid", "");
    _prefs.end();
}

void JellyfinClient::clearSession() {
    _prefs.begin("jf-config", false);
    _prefs.remove("host");
    _prefs.remove("user");
    _prefs.remove("token");
    _prefs.remove("userid");
    _prefs.end();
    _host = "";
    _username = "";
    _token = "";
    _userId = "";
}

// Simple URL Encoder for query searches
static String urlEncodeQuery(const String& value) {
    String encoded = "";
    char c;
    for (size_t i = 0; i < value.length(); i++) {
        c = value.charAt(i);
        if (isalnum(c)) {
            encoded += c;
        } else if (c == ' ') {
            encoded += "%20";
        } else {
            encoded += '%';
            char hex[3];
            sprintf(hex, "%02X", (unsigned char)c);
            encoded += hex;
        }
    }
    return encoded;
}

bool JellyfinClient::getArtists(String& jsonOut, int limit) {
    String endpoint = "/Artists?userId=" + _userId + "&Limit=" + String(limit) + "&SortBy=SortName";
    return sendGetRequest(endpoint, jsonOut);
}

bool JellyfinClient::getAlbums(String& jsonOut, int limit) {
    String endpoint = "/Items?userId=" + _userId + "&IncludeItemTypes=MusicAlbum&Recursive=true&Limit=" + String(limit) + "&SortBy=SortName";
    return sendGetRequest(endpoint, jsonOut);
}

bool JellyfinClient::getSongs(String& jsonOut, int limit) {
    String endpoint = "/Items?userId=" + _userId + "&IncludeItemTypes=Audio&Recursive=true&Limit=" + String(limit) + "&SortBy=SortName";
    return sendGetRequest(endpoint, jsonOut);
}

bool JellyfinClient::getArtistAlbums(const String& artistId, String& jsonOut) {
    String endpoint = "/Items?userId=" + _userId + "&IncludeItemTypes=MusicAlbum&ArtistIds=" + artistId + "&Recursive=true&SortBy=SortName";
    return sendGetRequest(endpoint, jsonOut);
}

bool JellyfinClient::getAlbumSongs(const String& albumId, String& jsonOut) {
    String endpoint = "/Items?userId=" + _userId + "&ParentId=" + albumId + "&SortBy=SortName,IndexNumber";
    return sendGetRequest(endpoint, jsonOut);
}

bool JellyfinClient::searchItems(const String& query, String& jsonOut) {
    String encodedQuery = urlEncodeQuery(query);
    String endpoint = "/Items?userId=" + _userId + "&SearchTerm=" + encodedQuery + "&IncludeItemTypes=Audio,MusicAlbum,MusicArtist&Recursive=true&Limit=20";
    return sendGetRequest(endpoint, jsonOut);
}
