#include "Buffer.hpp"

// Writes a value to the buffer with a given key
void Buffer::write(const std::string& key, const std::string& value) {
    std::lock_guard<std::mutex> lock(mtx);
    data[key] = value;
    cv.notify_all();
}

// Reads a value from the buffer with a given key
std::string Buffer::read(const std::string& key) {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this, key]{ return data.find(key) != data.end(); });
    return data[key];
}

// Checks if a key exists in the buffer (non-blocking)
bool Buffer::contains(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx);
    return data.find(key) != data.end();
}

// Removes a key from the buffer
void Buffer::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(mtx);
    data.erase(key);
}

// Clears the entire buffer
void Buffer::clear() {
    std::lock_guard<std::mutex> lock(mtx);
    data.clear();
}

// Gets all keys in the buffer
std::vector<std::string> Buffer::getKeys() {
    std::lock_guard<std::mutex> lock(mtx);
    std::vector<std::string> keys;
    for (const auto& pair : data) {
        keys.push_back(pair.first);
    }
    return keys;
}

// Special method for start procedure
bool Buffer::checkAndSet(const std::string& key, 
                              const std::string& initialValue, 
                              const std::string& newValue) {
    std::lock_guard<std::mutex> lock(mtx);
    if (data.find(key) == data.end() || data[key] == initialValue) {
        data[key] = newValue;
        cv.notify_all();
        return true;
    }
    return false;
}