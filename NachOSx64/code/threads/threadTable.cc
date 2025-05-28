#include "threadTable.h"
#include "system.h" 

ThreadTable::ThreadTable() {
    for (int i = 0; i < MAX_THREADS; i++) {
        table[i] = nullptr;
    }
}

ThreadTable::~ThreadTable() {
    for (int i = 0; i < MAX_THREADS; i++) {
        if (table[i] != nullptr) {
            delete table[i];
        }
    }
}

int ThreadTable::addThread(Thread* thread) {
    for (int i = 0; i < MAX_THREADS; i++) {
        if (table[i] == nullptr) {
            table[i] = thread;
            return i;
        }
    }
    return -1; // if table is full
}

Thread* ThreadTable::getThread(int tid) {
    if (tid >= 0 && tid < MAX_THREADS) {
        return table[tid];
    }
    return nullptr;
}

void ThreadTable::removeThread(int tid) {
    // set given index in table as null
    if (tid >= 0 && tid < MAX_THREADS) {
        table[tid] = nullptr;
    }
}

bool ThreadTable::exists(int tid) {
    // find if given id is valid and the content in table is not null
    return tid >= 0 && tid < MAX_THREADS && table[tid] != nullptr;
}
