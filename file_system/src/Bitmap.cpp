#include <inttypes.h>

#include "Bitmap.hpp"

void Bitmap::buildBitmap() {
  std::vector<unsigned char> bitmap(256, 0xFF); // Start with all blocks "free" (1=free)

  // Mark blocks 0-2 as used (bit=0)
  for (int index = 0; index <= 2; ++index) {
      int byte_pos = index / 8;
      int bit_pos = index % 8;
      bitmap[byte_pos] &= ~(1 << bit_pos);
  }
  this->bitmap = bitmap;
  // Write to Block 2
  this->file->writeBlock(2, this->bitmap);
}

int Bitmap::loadBitMap() {
  // read block 2 from the file
  if (this->file->readBlock(2, this->bitmap) != 0) return -1;
  // If the value of 1 is free, then there's no bit map.
  if (this->isBlockFree(2)) return -1;
  return 0;
}

uint16_t Bitmap::allocateBlock() {
  size_t block = getNextFreeBlock();
  if (block != 0) {
      // Update bitmap
      size_t byte = block / 8;
      size_t bit = block % 8;
      bitmap[byte] &= ~(1 << bit);  // Clear bit
      file->writeBlock(2, bitmap);    // Persist
  }
  return block;
}

void Bitmap::freeBlock(size_t block_num) {
  // Rango válido
  if (block_num <= 2 || block_num >= 2048) return;

  // Actualizar el bitmap
  size_t byte_pos = block_num / 8;
  size_t bit_pos = block_num % 8;
  bitmap[byte_pos] |= (1 << bit_pos);
  file->writeBlock(2, bitmap);

  // Llevar registro del último bloque libre
  if (block_num < last_free_block) {
      last_free_block = block_num; // Now the earliest available block
  }
}

uint16_t Bitmap::getNextFreeBlock() {
  // Quick check near last allocation (spatial locality)
  for (uint16_t i = last_free_block; i < 2048; ++i) {
      if (isBlockFree(i)) {
          last_free_block = i + 1;
          return i;
      }
  }

  // Full scan if no blocks found after last_free_block
  for (uint16_t i = 3; i < last_free_block; ++i) {
      if (isBlockFree(i)) {
          last_free_block = i + 1;
          return i;
      }
  }
  return 0;
}

bool Bitmap::isBlockFree(size_t block) {
  size_t byte = block / 8;
  size_t bit = block % 8;
  return (bitmap[byte] >> bit) & 1;
}