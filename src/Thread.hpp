#pragma once

#include <thread>

class Thread {
 protected:
  std::thread* thread = nullptr;

 public:
  Thread();
  virtual ~Thread();
  int start();
  int join();

 protected:
  virtual int run() = 0;
};


