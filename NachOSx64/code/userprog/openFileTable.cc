#include "openFileTable.h"
#include "copyright.h"
#include "syscall.h"
#include "unistd.h"
#include "stdio.h"

#define MAX_OPEN_FILES 32
	
OpenFileTable::OpenFileTable() {
    openFiles = new int[MAX_OPEN_FILES];
    openFilesMap = new BitMap(MAX_OPEN_FILES);
    usage = 1;

    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        openFiles[i] = -1;
    }
    openFilesMap->Mark(0); // ConsoleInput
    openFilesMap->Mark(1); // ConsoleOutput
    openFilesMap->Mark(2); // ConsoleError
}

OpenFileTable::~OpenFileTable() {
    delete[] openFiles;
    delete openFilesMap;
}

int OpenFileTable::Open(int unixHandle) {
   
    int index = openFilesMap->Find();
    if (index == -1) return -1;

    openFiles[index] = unixHandle;
    return index;
}

int OpenFileTable::Close(int nachosHandle) {
    if (!openFilesMap->Test(nachosHandle)) return -1;

    int unixHandle = openFiles[nachosHandle];
    close(unixHandle); 

    openFiles[nachosHandle] = -1;
    openFilesMap->Clear(nachosHandle);
    return 0;
}

bool OpenFileTable::isOpened(int nachosHandle) {
    return openFilesMap->Test(nachosHandle);
}

int OpenFileTable::getUnixHandle(int nachosHandle) {
    if (!isOpened(nachosHandle)) return -1;
    int file = openFiles[nachosHandle];
    return file;
}

void OpenFileTable::addThread() {
    usage++;
}

void OpenFileTable::delThread() {
    usage--;
}

void OpenFileTable::Print() {
    printf("OpenFileTable:\n");
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (openFilesMap->Test(i)) {
            printf("  NachOS handle %d -> Unix handle %d\n", i, openFiles[i]);
        }
    }
}