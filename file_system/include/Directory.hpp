#pragma once

#include <vector>
#include <string>
#include <inttypes.h>
#include <utility> // Includes std::pair

#include "FileHandler.hpp"
#include "Bitmap.hpp"

/// @brief Manages the directory of the File System
class Directory {
 public:
  /// @brief Constructor for the class
  /// @param theFile The file used by the file system.
  Directory(FileHandler* theFile, Bitmap* bitMap)
          : file(theFile)
          , bitmap(bitMap) {
    if (this->loadDirectory() != 0) {
      throw std::runtime_error("Directory::Failed to open file");
    }
  }
  /// @brief Gets the directory in a string.
  /// @return returns the information stored in the directory.
  std::string getDirectory();
  /// @brief Gets the list of files in the directory.
  /// @return returns the list of files in the directory.
  std::string listFiles();
  /// @brief Gets the number of the block with the blockTable of the file.
  /// @param fileName The name of the file to be searched
  /// @return returns the number of the block with the blockTable of the file.
  uint16_t getFileBlockTable(std::string fileName);
  /// @brief Adds a file to the directory.
  /// @param fileName The name of the file
  /// @param blockTable The number of the blockTable of the file.
  /// @return returns 0 on success.
  int addFile(std::string fileName, uint16_t blockTable);
  /// @brief Removes a file from the directory.
  /// @param fileName The name of the file.
  /// @return returns 0 on success.
  int removeFile(std::string fileName);
  /// @brief removes all the data in the directory.
  void cleanDir();
  /// @brief Loads the directory from the file
  /// @return returns 0 on success.
  int loadDirectory();

 private:
  /// @brief A vector with the pairs of file names and BlockTable block numbers.
  std::vector<std::pair<std::string, uint16_t>> fileList;
  /// @brief The file used by the file system.
  FileHandler* file;
  /// @brief The bitmap of the file.
  Bitmap* bitmap;
  /// @brief Persist the directory after a modification.
  /// @return returns 0 on success.
  int updateDir();
};