#include <cstring>
#include <stdexcept>
#include <cassert>
#include <iostream>
#include <filesystem>
#include <algorithm>

#include "FileHandler.hpp"

constexpr const size_t BLOCK_COUNT = 2048; // Cantidad de bloques
constexpr const size_t BLOCK_SIZE = 256; // Cantidad de bloques
constexpr size_t TOTAL_FILE_SIZE = BLOCK_SIZE * BLOCK_COUNT; // 256 * 2048

FileHandler::~FileHandler() {
  if (closeFile() != 0) {
    throw std::runtime_error("Failed to close file");
  }
}

int FileHandler::openFile() {
  // Try opening existing file first
  file.open(this->fileName, std::ios::binary | std::ios::in | std::ios::out);

  if (file.is_open()) {
      // Verify existing file size
      file.seekg(0, std::ios::end);
      size_t actualSize = file.tellg();
      file.seekg(0); // Rewind

      if (actualSize != TOTAL_FILE_SIZE) {
          std::cerr << "Error: Invalid file size (" << actualSize
                    << " bytes, expected " << TOTAL_FILE_SIZE << ")\n";
          file.close();
          return -2; // Special error code for size mismatch
      }
      return 0;
  }

  // Create new file with correct size
  file.open(fileName, std::ios::binary | std::ios::out);
  if (!file.is_open()) {
      return -1; // Failed to create
  }

  // Pre-size the file
  file.seekp(TOTAL_FILE_SIZE - 1);
  file.write("\0", 1);
  file.flush();

  // Verify the size was set
  file.seekp(0, std::ios::end);
  if (file.tellp() != TOTAL_FILE_SIZE) {
      file.close();
      std::filesystem::remove(fileName); // Clean up
      return -3; // Failed to resize
  }

  file.close();

  // Reopen with proper flags
  file.open(fileName, std::ios::binary | std::ios::in | std::ios::out);
  return file.is_open() ? 0 : -4;
}

int FileHandler::closeFile() {
  if(this->file.is_open()) {
    this->file.close();
    return 0;
  } else {
    return -1;
  }
}

int FileHandler::wipeFile() {
    if (!this->file.is_open()) {
        return -1; // Failed to open
    }

    // Create a buffer filled with zeros
    std::vector<unsigned char> zeros(BLOCK_COUNT * BLOCK_SIZE, 0);

    // Position at the start of the file
    file.seekp(0);
    file.write(reinterpret_cast<const char*>(zeros.data()), zeros.size());
    file.flush();

    return file ? 0 : -1;
}

int FileHandler::readBlock(size_t blockNumber, std::vector<unsigned char>& buffer) {
  // Validate block number first
  if (blockNumber >= BLOCK_COUNT) {
      std::cerr << "Error: Invalid block number " << blockNumber 
                << " (max " << BLOCK_COUNT << ")\n";
      return -1;
  }

  // Check file state thoroughly
  if (!file.is_open()) {
      std::cerr << "Error: File not open\n";
      return -2;
  }
  if (!file.good()) {
      std::cerr << "Error: File in bad state (failbit: " << file.fail() 
                << ", badbit: " << file.bad() << ", eofbit: " << file.eof() << ")\n";
      return -3;
  }

  // Calculate and validate offset
  size_t offset = calculateFileOffset(blockNumber);
  if (offset % BLOCK_SIZE != 0) {
      std::cerr << "Error: Invalid block alignment\n";
      return -4;
  }

  // Prepare buffer
  buffer.resize(BLOCK_SIZE);
  
  // Perform seek and verify
  file.seekg(offset);
  if (file.fail()) {
      std::cerr << "Error: Seek failed to position " << offset << "\n";
      return -5;
  }

  // Perform read
  file.read(reinterpret_cast<char*>(buffer.data()), BLOCK_SIZE);
  const size_t bytesRead = file.gcount();

  // Handle read results
  if (file.eof()) {
      std::cerr << "Warning: Reached EOF after reading " << bytesRead << " bytes\n";
  }
  if (file.fail() && !file.eof()) {
      std::cerr << "Error: Read operation failed\n";
      return -6;
  }

  return (bytesRead == BLOCK_SIZE) ? 0 : -7;
}

int FileHandler::writeBlock(size_t blockNumber, const std::vector<unsigned char>& buffer) {
  if (blockNumber >= BLOCK_COUNT) return -1;
  if (!file.is_open()) return -2;

  // Seek to EXACT block position
  const size_t offset = blockNumber * BLOCK_SIZE;
  file.seekp(offset);
  if (file.fail()) return -3;

  // Write ENTIRE block (pad with zeros if needed)
  std::vector<unsigned char> blockData(BLOCK_SIZE, 0);
  const size_t bytesToWrite = std::min(buffer.size(), BLOCK_SIZE);
  std::copy_n(buffer.begin(), bytesToWrite, blockData.begin());

  file.write(reinterpret_cast<const char*>(blockData.data()), BLOCK_SIZE);
  file.flush();

  return file.good() ? 0 : -4;
}

size_t FileHandler::calculateFileOffset(size_t blockNumber) const {
  return blockNumber * BLOCK_SIZE;
}