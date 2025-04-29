#include "Interface.hpp"
#include <iostream>
#include <fstream>
#include <utility>

Interface::Interface(const std::string& filename) : fs(filename) {
}

void Interface::run() {
    printHelp();

    while (running) {
        std::string input = getInput("> ");

        auto it = commands.find(input);
        if (it != commands.end()) {
            try {
                it->second(); // Execute command
            } catch (const std::exception& e) {
                std::cerr << "Error: " << e.what() << "\n";
            }
        } else {
            std::cout << "Invalid command. Type 'help' for options.\n";
        }
    }
}

// Command implementations
void Interface::handleAdd() {
    std::string filename = getInput("Enter file name: ");
    std::string filepath = getInput("Enter path to file: ");

    std::string full_path = filepath + "/" + filename;

    std::ifstream source(full_path, std::ios::binary);
    if (!source) {
        std::cerr << "Error: Could not open " << full_path << "\n";
        return;
    }

    if (fs.addFile(filename, source) == 0) {
        std::cout << "File added successfully\n";
    } else {
        std::cerr << "Error: Failed to add file\n";
    }
}

void Interface::handleDelete() {
    std::string filename = getInput("Enter file name to delete: ");

    if (fs.deleteFile(filename) == 0) {
        std::cout << "File deleted successfully\n";
    } else {
        std::cerr << "Error: File not found or deletion failed\n";
    }
}

void Interface::handleList() {
    std::cout << "\nDirectory Listing:\n";
    std::cout << fs.getDirectory() << "\n";
}

void Interface::handleReset() {
    if (fs.buildFileSystem() == 0) {
        std::cout << "File system reset successfully\n";
    } else {
        std::cerr << "Error: Reset failed\n";
    }
}

void Interface::handleExit() {
    running = false;
    std::cout << "Exiting...\n";
}

void Interface::handleView() {
    std::string filename = getInput("Enter file name to view: ");

    try {
        std::string content = fs.getFile(filename);
        if (content == "File Not Found.") {
            std::cerr << "Error: File not found\n";
        } else {
            std::cout << "\n--- " << filename << " contents ---\n";
            std::cout << content << "\n";
            std::cout << "--- End of file ---\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "Error retrieving file: " << e.what() << "\n";
    }
}

void Interface::printHelp() const {
    std::cout << "\nAvailable commands:\n"
              << "add    - Add a text file to the system\n"
              << "delete - Remove a file\n"
              << "view   - View text file contents\n"  // New command
              << "list   - Show directory contents\n"
              << "reset  - Wipe and recreate file system\n"
              << "exit   - Quit the program\n"
              << "help   - Show this help\n\n";
}

std::string Interface::getInput(const std::string& prompt) const {
    std::string input;
    std::cout << prompt;
    std::getline(std::cin, input);
    return input;
}