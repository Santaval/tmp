#include "Interface.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        // Use default filename or take from command line
        std::string filename = "files/file_sys_1.txt";
        if (argc > 1) {
            filename = argv[1];
        }

        std::cout << "Starting File System Interface\n";
        std::cout << "Using file: " << filename << "\n\n";

        Interface interface(filename);
        interface.run();

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << "\n";
        return 1;
    }
}