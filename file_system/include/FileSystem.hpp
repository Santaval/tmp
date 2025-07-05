#pragma once

#include <string>
#include <vector>

#include "FileHandler.hpp"
#include "Bitmap.hpp"
#include "Directory.hpp"

constexpr const char* FILE_NAME = "file_sys_1.txt";

/// @brief Manages the overall logic of a file system.
class FileSystem {
 public:
  // Constructor and destructor
  FileSystem(const std::string& fileName = FILE_NAME);
  ~FileSystem();

  // Basic operations
  /// @brief builds an empty file system with the basic logic information.
  /// @return returns 0 on success.
  int buildFileSystem();
  /// @brief Gets the file with the name fileName. If not found, returns "File
  /// Not Found"
  /// @param fileName The name of the file.
  /// @return returns the contents of the file in a string.
  std::string getFile(std::string fileName);
  /// @brief Gets the entries in the directory
  /// @return returns a string with available information in the directory.
  std::string getDirectory();
  /// @brief Gets the list of files in the directory
  /// @return returns a string with the list of files in the directory.
  std::string listFiles();
  /// @brief Adds a file to the directory
  /// @param name The name of the file to be added
  /// @param fileContent The contents of the file
  /// @return retuns 0 on success.
  int addFile(std::string name, std::istream& fileContent);
  /// @brief Deletes a file from the directory
  /// @param name The name of the file to be deleted
  /// @return retuns 0 on success.
  int deleteFile(std::string name);
  /// @brief Loads all .txt files from a specified directory into the file system
  /// @param directoryPath The path to the directory containing .txt files
  /// @return returns 0 on success, -1 on failure
  int loadFilesFromDirectory(const std::string& directoryPath);

 private:
  /// @brief Used to write and read blocks from the file
  FileHandler file;
  /// @brief Bitmap of used and available blocks
  Bitmap bitmap;
  /// @brief The directory of the file system.
  Directory dir;
};
