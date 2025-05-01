#pragma once

#include <stdlib.h>
#include <vector>

#include "FileHandler.hpp"

/// Manages the bit map logic for the file system.
class Bitmap {
 public:
  /// @brief Constructor for the Bitmap
  /// @param theFile  a pointer to the FileHandler class used by the file system.
  Bitmap(FileHandler* theFile) : file(theFile) {
    this->bitmap.resize(256);
    // Load bit map from the current file.
    theFile->readBlock(2, this->bitmap);
  }
  /// @brief A constructor for a brand new bit map. Used to initialize a raw file.
  void buildBitmap();
  /// @brief Loads from the current file the bitmap.
  int loadBitMap();
  /// @brief Allocates the next free block in the bit map and returns its number.
  /// @return The number of the recently allocated block.
  uint16_t allocateBlock();
  /// @brief Frees in the bit map the the block with block_num.
  /// @param block_num The number of the block to be freed.
  void freeBlock(size_t block_num);
 private:
  // PRIVATE ATTRIBUTES
  /// @brief The file used by the file system.
  FileHandler* file;
  /// @brief A memory copy of the bitmap.
  std::vector<unsigned char> bitmap;
  /// @brief A tracker for the last free block of the bit map.
  size_t last_free_block = 3;

  // PRIVATE FUNCTIONS
  /// @brief Searches in the bitmap for the next free blocks and returns its
  /// corresponding block number.
  /// @return The block number of the next free block.
  uint16_t getNextFreeBlock();
  /// @brief Checks in the bit map if the block is free.
  /// @param block The block to be checked.
  /// @return Returns true if the block is free; false otherwise.
  bool isBlockFree(size_t block);
};