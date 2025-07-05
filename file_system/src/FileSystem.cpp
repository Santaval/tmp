#include <iostream>
#include <filesystem>
#include <fstream>

#include "FileSystem.hpp"
#include "BlockOper.hpp"

constexpr const size_t BLOCK_SIZE = 256; // Cantidad de bloques

FileSystem::FileSystem(const std::string& fileName)
        : file(fileName)
        , bitmap(&file), dir(&file, &bitmap){
  try {
      // Try building new filesystem if loading fails
      this->bitmap.buildBitmap();
      if (this->bitmap.loadBitMap() != 0) {
          std::cerr << "Info: Creating new filesystem\n";
          if (this->buildFileSystem() != 0) {
              throw std::runtime_error("Failed to create new filesystem");
          }
      } else {
        std::cerr << "Info: found existing FS\n";
      }
  } catch (const std::exception& e) {
      throw std::runtime_error(std::string("Filesystem initialization failed: ") + e.what());
  }
}


FileSystem::~FileSystem() {
}

int FileSystem::buildFileSystem() {
  int status = -1;
  status = this->file.wipeFile();
  this->bitmap.buildBitmap();
  this->dir.cleanDir();
  return status;
}

std::string FileSystem::getFile(std::string fileName) {
  // If no block table found, return a not found
  uint16_t blockTable = this->dir.getFileBlockTable(fileName);
  if (blockTable == 0) return "File Not Found.";

  std::string fileData;
  std::vector<unsigned char> blockTableBuffer(256);
  std::vector<unsigned char> blockBuffer(256);

  // Obtain data from the file
  while (blockTable != 0) {
    // Load block table's content
    if (this->file.readBlock(blockTable, blockTableBuffer) != 0) return "Error reading block";
    // read 2 bytes. If 0, stop. Else, read block and load into fileData.
    for (uint16_t currentByte = 0; currentByte < 256; currentByte+=2) {
      // Get 2 bytes for the pointer, using little endian.
      uint16_t blockPointer = read2BPointer(blockTableBuffer, currentByte);
      // End of table, pointer to null block
      if (blockPointer == 0) {
        return fileData;
      // End of block, get new block table
      } else if (currentByte == 254) {
        blockTable = blockPointer;
        break;
      } else {
        // Obtain data in new block
        if (this->file.readBlock(blockPointer, blockBuffer)) return "Error reading block";
        fileData.append(reinterpret_cast<const char*>(blockBuffer.data()), 256);
      }
    }
  }
  return fileData;
}

std::string FileSystem::getDirectory() {
  return this->dir.getDirectory();
}

std::string FileSystem::listFiles() {
  return this->dir.listFiles();
}

int FileSystem::addFile(std::string name, std::istream& fileContent) {
  // Allocate block for the first block table
  uint16_t currentTableBlock = bitmap.allocateBlock();
  if (currentTableBlock == 0) {
      std::cerr << "Error: No space for initial block table\n";
      return -1;
  }

  // Add to directory
  if (this->dir.addFile(name, currentTableBlock) != 0) {
      bitmap.freeBlock(currentTableBlock);
      return -1;
  }

  std::vector<unsigned char> blockTable(BLOCK_SIZE, 0);
  uint16_t currentEntry = 0;
  uint16_t dataBlockCount = 0;
  bool needsFinalWrite = false;

  try {
      std::vector<unsigned char> buffer(BLOCK_SIZE);
      while (fileContent.read(reinterpret_cast<char*>(buffer.data()), BLOCK_SIZE) || fileContent.gcount() > 0) {
          // Allocate and write data block
          uint16_t dataBlock = bitmap.allocateBlock();
          if (dataBlock == 0) throw std::runtime_error("No space for data block");

          // Store data block pointer in current block table
          write2BPointer(blockTable, currentEntry * 2, dataBlock);
          currentEntry++;
          dataBlockCount++;

          // Write actual data (full or partial block)
          size_t bytesToWrite = fileContent.gcount();
          if (bytesToWrite < BLOCK_SIZE) {
              buffer.resize(bytesToWrite);
          }
          file.writeBlock(dataBlock, buffer);

          // Handle block table overflow
          if (currentEntry >= (BLOCK_SIZE / 2) - 1) {  // 127 entries per table
              // Allocate new block table
              uint16_t nextTableBlock = bitmap.allocateBlock();
              if (nextTableBlock == 0) throw std::runtime_error("No space for new block table");

              // Write next table pointer in last entry
              write2BPointer(blockTable, BLOCK_SIZE - 2, nextTableBlock);

              // Write current block table to disk
              file.writeBlock(currentTableBlock, blockTable);

              // Prepare next block table
              currentTableBlock = nextTableBlock;
              blockTable.assign(BLOCK_SIZE, 0);
              currentEntry = 0;
              needsFinalWrite = false;
          } else {
              needsFinalWrite = true;
          }

          if (fileContent.eof()) break;
          buffer.resize(BLOCK_SIZE);  // Reset buffer size for next read
      }

      // Write final block table if needed
      if (needsFinalWrite) {
          write2BPointer(blockTable, BLOCK_SIZE - 2, 0);  // Terminate chain
          file.writeBlock(currentTableBlock, blockTable);
      }

      std::cout << "File added successfully. Used "
                << dataBlockCount + 1  // +1 for the block table
                << " blocks.\n";
      return 0;
  } catch (const std::exception& e) {
      // Cleanup allocated blocks on failure
      bitmap.freeBlock(currentTableBlock);
      // TODO: Free any allocated data blocks
      std::cerr << "Error: " << e.what() << "\n";
      return -1;
  }
}

int FileSystem::deleteFile(const std::string fileName) {
    uint16_t blockTable = dir.getFileBlockTable(fileName);
    if (blockTable == 0) return -1;

    std::vector<unsigned char> buffer(256);
    std::vector<unsigned char> wipeBlock(256, 0);

    while (blockTable != 0) {
        if (file.readBlock(blockTable, buffer) != 0) return -2;

        // Free data blocks
        for (uint16_t offset = 0; offset < 238; offset += 2) {
            uint16_t blockPointer = read2BPointer(buffer, offset);
            if (blockPointer == 0) break;

            file.writeBlock(blockPointer, wipeBlock);
            bitmap.freeBlock(blockPointer);
        }

        // Free the block table itself
        uint16_t nextTable = read2BPointer(buffer, 238);
        file.writeBlock(blockTable, wipeBlock);
        bitmap.freeBlock(blockTable);
        blockTable = nextTable;
    }

    return dir.removeFile(fileName);
}

int FileSystem::loadFilesFromDirectory(const std::string& directoryPath) {
    try {
        // Check if directory exists
        if (!std::filesystem::exists(directoryPath)) {
            std::cerr << "Directory does not exist: " << directoryPath << std::endl;
            return -1;
        }

        // Iterate through all files in the directory
        for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) {
            // Check if it's a regular file and has .txt extension
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                std::string fileName = entry.path().filename().string();
                std::ifstream fileStream(entry.path());
                
                if (!fileStream.is_open()) {
                    std::cerr << "Failed to open file: " << fileName << std::endl;
                    continue;
                }

                // Add the file to the file system
                int result = addFile(fileName, fileStream);
                if (result != 0) {
                    std::cerr << "Failed to add file to file system: " << fileName << std::endl;
                }
                
                fileStream.close();
            }
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error loading files from directory: " << e.what() << std::endl;
        return -1;
    }
}

