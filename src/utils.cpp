#include "utils.hpp"


std::vector<std::string> splitValue(const std::string& content, char del) {
    std::vector<std::string> keys;
    // Create a stringstream object to value of buffer
    std::stringstream ss(content);
	std::string token;
   	// Splitting the str string by delimiter
    while (getline(ss, token, del))
        keys.push_back(token);
    return keys;
}
