#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "FileSystem.hpp"
#include <string>
#include <functional>
#include <map>

class Interface {
public:
    Interface(const std::string& filename = "file_sys_1.txt");

    void run(); // Main interaction loop

private:
    FileSystem fs;
    bool running = true;

    // Command handlers
    void handleAdd();
    void handleDelete();
    void handleList();
    void handleReset();
    void handleExit();
    void handleView();

    // Utility functions
    void printHelp() const;
    std::string getInput(const std::string& prompt) const;

    // Command map
    using CommandHandler = std::function<void()>;
    std::map<std::string, CommandHandler> commands = {
        {"add",    [this]() { handleAdd(); }},
        {"delete", [this]() { handleDelete(); }},
        {"view",   [this]() { handleView(); }},
        {"list",   [this]() { handleList(); }},
        {"reset",  [this]() { handleReset(); }},
        {"exit",   [this]() { handleExit(); }},
        {"help",   [this]() { printHelp(); }}
    };
};

#endif // INTERFACE_HPP