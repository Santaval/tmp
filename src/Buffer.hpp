#pragma once

#include <string>
#include <map>
#include <mutex>
#include <condition_variable>
#include <vector>

class SharedBuffer {
public:
    // Constructor/Destructor
    SharedBuffer() = default;
    ~SharedBuffer() = default;

    // Basic operations
    void write(const std::string& key, const std::string& value);
    std::string read(const std::string& key);
    bool contains(const std::string& key);
    void remove(const std::string& key);
    void clear();
    std::vector<std::string> getKeys();

    // Special operations
    bool checkAndSet(const std::string& key, 
                    const std::string& initialValue, 
                    const std::string& newValue);

private:
    std::map<std::string, std::string> data;
    std::mutex mtx;
    std::condition_variable cv;
};
