#pragma once

#include <string>
#include <map>
#include <mutex>
#include <condition_variable>
#include <vector>

class Buffer {
public:
    // Constructor/Destructor
    Buffer() = default;
    ~Buffer() = default;

    /**
     * @brief Writes a key-value pair to the buffer.
     * 
     * This function stores the provided key and its associated value
     * into the buffer. The behavior of the function depends on the
     * implementation details of the buffer, such as whether it allows
     * overwriting existing keys or handles concurrency.
     * 
     * @param key The key to be written to the buffer.
     * @param value The value associated with the key.
     * @example buffer.write("name", "Alice");
     */
    void write(const std::string& key, const std::string& value);
    
    /**
     * @brief Reads the value associated with the given key from the buffer.
     * 
     * This function retrieves the value corresponding to the specified key
     * from the buffer. If the key does not exist, the behavior depends on
     * the implementation (e.g., it might return an empty string or throw an exception).
     * 
     * @param key The key whose associated value is to be retrieved.
     * @return A string containing the value associated with the key.
     * @example std::string name = buffer.read("name"); 
     *  if you wrote before buffer.write("name", "Alice"), name will be "Alice"
     * 
     */
    std::string read(const std::string& key);
    /**
     * @brief Checks if the buffer contains the specified key.
     * 
     * @param key The key to search for in the buffer.
     * @return true if the key is found in the buffer, false otherwise.
     * 
     * @example if (buffer.contains("name")) { ... }
     */
    bool contains(const std::string& key);

    /**
     * @brief Removes an entry from the buffer identified by the specified key.
     * 
     * @param key The key associated with the entry to be removed.
     * 
     * @note If the key does not exist in the buffer, the function may have no effect.
     */
    void remove(const std::string& key);

    /**
     * @brief Clears the contents of the buffer, resetting it to an empty state.
     */
    void clear();
    /**
     * @brief Splits the value of given key by the given delimiter
     * @param key The jey associated to the value to split
     * @param del The delimitator with which the splitting will be donde
     * @return a vector of type string of the tokens found
     */
    std::vector<std::string> splitValue(const std::string& key, char del);

    // Special operations
    /**
     * @brief Atomically checks if the current value associated with a key matches 
     *        the provided initial value, and if so, updates it to a new value.
     * 
     * @param key The key associated with the value to be checked and potentially updated.
     * @param initialValue The value to compare against the current value associated with the key.
     * @param newValue The value to set if the current value matches the initial value.
     * @return true If the current value matches the initial value and the update was successful.
     * @return false If the current value does not match the initial value.
     */
    bool checkAndSet(const std::string& key, 
                    const std::string& initialValue, 
                    const std::string& newValue);

    private:
        /**
         * @brief A map container that stores key-value pairs, where both the key and value are strings.
         * 
         * This map is used to associate a string key with a corresponding string value.
         * It provides fast retrieval, insertion, and deletion of elements based on the key.
         * 
         * Key Characteristics:
         * - Keys are unique within the map.
         * - Elements are automatically sorted by the key in ascending order.
         * - Provides logarithmic complexity for search, insert, and delete operations.
         * 
         * Example Use Case:
         * - Storing configuration settings where the key is the setting name and the value is the setting value.
         */
        std::map<std::string, std::string> data;
};