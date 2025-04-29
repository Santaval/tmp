#pragma once

#include <string>
#include <fstream>
#include <vector>

class FileHandler {
 public:
  // Constructor and destructor
  FileHandler(const std::string& fileName)
          : fileName(fileName) {
    if (openFile() != 0) {
      throw std::runtime_error("FileHandler::Failed to open file");
    }
  }
  ~FileHandler();

  // Binary operations
  int readBlock(size_t blockNumber, std::vector<unsigned char>& buffer);
  int writeBlock(size_t blockNumber, const std::vector<unsigned char>& buffer);
  int wipeFile();

 private:
  std::string fileName;
  std::fstream file;

  // File operations
  int openFile();
  int closeFile();

  // Utility
  size_t calculateFileOffset(size_t blockNumber) const;
};
