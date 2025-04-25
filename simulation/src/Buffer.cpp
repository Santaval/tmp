#include "Buffer.hpp"
#include <bits/stdc++.h>
// Writes a value to the buffer with a given key
void Buffer::write(const std::string& key, const std::string& value) {
    data[key] = value;
}

// Reads a value from the buffer with a given key
std::string Buffer::read(const std::string& key) {
    std::string value = this->data[key];
    this->clear();
    return value;
}

// Checks if a key exists in the buffer (non-blocking)
bool Buffer::contains(const std::string& key) {
    return data.find(key) != data.end();
}

// Removes a key from the buffer
void Buffer::remove(const std::string& key) {
    data.erase(key);
}

// Clears the entire buffer
void Buffer::clear() {
    data.clear();
}



// Special method for start procedure
bool Buffer::checkAndSet(const std::string& key, 
                              const std::string& initialValue, 
                              const std::string& newValue) {
    if (data.find(key) == data.end() || data[key] == initialValue) {
        data[key] = newValue;
        return true;
    }
    return false;
}