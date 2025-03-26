#include <cassert>
#include <cstdlib>

#include "Thread.hpp"

Thread::Thread() {
}

Thread::~Thread() {
  delete this->thread;
}

int Thread::start() {
  this->thread = new std::thread(&Thread::run, this);
  return EXIT_SUCCESS;
}

int Thread::join() {
  this->thread->join();
  delete this->thread;
  this->thread = nullptr;
  return EXIT_SUCCESS;
}