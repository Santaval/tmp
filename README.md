# Distributed Network File System

A distributed network file system implementation with fork-based server architecture and HTTP protocol support.

## Project Overview

This project implements a distributed system that combines:
- A custom file system implementation
- Multiple server nodes with fork capabilities
- HTTP-based communication
- SSL socket support for secure connections

## Project Components

### 1. File System (`file_system/`)
- Custom file system implementation with:
  - Block-based storage allocation
  - Bitmap-based space management
  - Directory and file operations
  - File handlers for read/write operations
- Key files:
  - `FileSystem.hpp/cpp`: Core file system implementation
  - `Bitmap.hpp/cpp`: Storage allocation management
  - `Directory.hpp/cpp`: Directory structure handling
  - `FileHandler.hpp/cpp`: File operations interface

### 2. Fork System (`fork/`)
Implements server forking and management:
- Server node management
- Broadcast communication
- Custom routing system
- Protocol implementations:
  - HTTP parsing and handling
  - Custom PIG (Protocol Interface Gateway)
- Socket implementations:
  - TCP/IP sockets
  - SSL secure sockets
  - Broadcast sockets

### 3. Server (`server/`)
Main server implementation:
- HTTP server functionality
- Resource management
- Broadcast listener capabilities
- Thread management
- Test suite included

## Directory Structure
```
.
├── file_system/       # Core file system implementation
├── fork/             # Fork and server management system
└── server/           # Main server implementation
```

## Building the Project

Each component has its own Makefile:

1. Build the file system:
```bash
cd file_system
make
```

2. Build the fork system:
```bash
cd fork
make
```

3. Build the server:
```bash
cd server
make
```

## Features

- **Distributed Architecture**: Multiple server nodes working together
- **Custom File System**: Efficient block-based storage
- **Protocol Support**: 
  - HTTP/1.1
  - Custom PIG protocol
  - Broadcast communication
- **Security**: SSL socket support
- **Resource Management**: Efficient handling of files and directories
- **Threading**: Multi-threaded server operations

## Resource Files

The system comes with sample resources in `server/resources/` including:
- Text files for testing
- Configuration files
- Sample content

## Testing

Test files are provided in the `server/tests/` directory:
- `testClient.cc`: Client-side testing
- `testServer.cc`: Server functionality testing

## Requirements

- C++ compiler with C++11 support
- OpenSSL for SSL socket support
- POSIX-compliant operating system
- Make build system

## Configuration

Server broadcast settings can be configured in:
- `server/config/broadcast.txt`
- `fork/config/broadcast.txt`

## Documentation

Each component includes its own documentation:
- File system design diagrams in `file_system/img/`
- Protocol documentation in respective README files
- Component-specific implementation details in source files
