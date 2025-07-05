#include <utility>

#include "Directory.hpp"
#include "BlockOper.hpp"

std::string Directory::getDirectory() {
  if (this->fileList.empty()) return "No files available";

  // Append dir info to a string (format) and send in return.
  std::string directoryData;
  for (const std::pair<std::string, uint16_t>& fileLog : this->fileList) {
    directoryData += "> " + fileLog.first + " " + std::to_string(fileLog.second) + "\n";
  }
  return directoryData;
}

std::string Directory::listFiles() {
  if (this->fileList.empty()) return "No files available";

  // Append dir info to a string (format) and send in return.
  std::string directoryData;
  for (const std::pair<std::string, uint16_t>& fileLog : this->fileList) {
    directoryData += fileLog.first + "\n";
  }
  return directoryData;
}
uint16_t Directory::getFileBlockTable(std::string fileName) {
  for (const std::pair<std::string, uint16_t>& fileLog : this->fileList) {
    if (fileLog.first == fileName) {
      return fileLog.second;
    }
  }
  return 0;
}

int Directory::addFile(std::string fileName, uint16_t blockTable) {
  // Make sure name is not repeated.
  if (this->getFileBlockTable(fileName) != 0) return -1;
  this->fileList.emplace_back(std::move(fileName), blockTable);
  return this->updateDir();
}

int Directory::removeFile(std::string fileName) {
  // Delete file from directory and update directory.
  for (auto fileLog = this->fileList.begin(); fileLog != this->fileList.end(); ++fileLog) {
    // If they're equal, return the file content table pointer
    if (fileLog->first == fileName) {
      std::swap(*fileLog, this->fileList.back());
      this->fileList.pop_back();
      return this->updateDir();
    }
  }
  // File name not found.
  return -1;
}

void Directory::cleanDir() {
  this->fileList.clear();
}

int Directory::loadDirectory() {
  // Read block 1 and analyze it
  // Directory is always in block 1. Start there
  uint16_t blockTable = 1;
  std::vector<unsigned char> blockBuffer(256);
  uint16_t blockTablePointer;

  // Obtain data from the directory
  while (blockTable != 0) {
    // Load directory table's content
    if (this->file->readBlock(blockTable, blockBuffer) != 0) return -1;
    for (uint16_t currentByte = 0; currentByte < 256; currentByte += 34) {
      // Get file name and pointer to block table
      std::string filename = readString32B(blockBuffer, currentByte);
      blockTablePointer = read2BPointer(blockBuffer, currentByte + 32);
      // Void pointer => no entry.
      if (blockTablePointer == 0) {
        return 0;
      // End of block directory, swith to next block directory
      } else if (currentByte == 238) {
        blockTable = read2BPointer(blockBuffer, currentByte);
        break;
      } else {
        // Store directory entry
        this->fileList.emplace_back(std::move(filename), blockTablePointer);
      }
    }
  }
  if (this->fileList.size() == 0) return -1;
  return 0;
}

int Directory::updateDir() {
  // Obtener número de directory blocks (logs de directory / 7)
  uint16_t dirBlockCount = this->fileList.size() / 7;
  ++dirBlockCount;

  // Obtener el directorio del bloque 1
  std::vector<unsigned char> oldDirBlock(256, 0);
  size_t currentDirTable = 1;
  this->file->readBlock(currentDirTable, oldDirBlock);
  std::vector<unsigned char> newDirBlock(256, 0);
  std::vector<unsigned char> wipeBlock(256, 0);

  // para cada Directory Block
  for (uint16_t dirblock = 0; dirblock < dirBlockCount; ++dirblock) {
    // Wipe newDirBlock
    newDirBlock = wipeBlock;
    // Guardar puntero a bloque siguiente en buffer (hacerlo aunque sea 0)
    uint16_t nextDirTable = read2BPointer(oldDirBlock, 238);

    // Corner case: last dir block is free => must be freed (deletion)
    if (nextDirTable != 0 && dirBlockCount == (dirBlockCount - 1)) {
      this->bitmap->freeBlock(nextDirTable);
      this->file->writeBlock(nextDirTable, wipeBlock);
      nextDirTable = 0;
    }
    // Corner case: needs new dir block => must be allocated (addition)
    if (nextDirTable == 0 && dirBlockCount == (dirBlockCount - 2)) {
      nextDirTable = this->bitmap->allocateBlock();
      if (nextDirTable == 0) return -1;
    }
    write2BPointer(newDirBlock, 238, nextDirTable);
    // for (uint16_t currentByte = 0; currentByte < 256; currentByte += 34) {
    size_t currentByte = 0;
    // por cada archivo (de este bloque), escribirlos en el buffer (nombre + pointer)
    for (uint16_t fileLogNum = dirblock * 7
            ; (fileLogNum < (dirblock + 1) * 7 && fileLogNum < this->fileList.size())
            ; ++fileLogNum) {
      writeString32B(newDirBlock, currentByte, this->fileList[fileLogNum].first);
      write2BPointer(newDirBlock, currentByte + 32, this->fileList[fileLogNum].second);
      currentByte += 34;
    }
    // sobre escribir el bloque
    this->file->writeBlock(currentDirTable, newDirBlock);
    // Cargar siguiente bloque de directory.
    currentDirTable = nextDirTable;
    this->file->readBlock(currentDirTable, oldDirBlock);
  }
  return 0;
}