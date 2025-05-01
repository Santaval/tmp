# HTTP Server

A simple HTTP server implementation that serves files from a virtual file system.

## Prerequisites

Before compiling the server, you need to have:
- C++17 compatible compiler (g++ or clang++)
- Make
- The file system library (included in the project)

## Project Structure

```
server/
├── src/           # Source files
├── bin/           # Compiled binaries
├── build/         # Object files
├── resources/     # Files to be served
└── tests/         # Test files
```

## Compilation Steps

1. First, compile the file system library:
```bash
cd file_system
make
```

2. Then, compile the client:
```bash
cd ../client
make
```

3. Finally, compile the server:
```bash
cd ../server
make
```

The compiled binaries will be placed in their respective `bin/` directories.

## Running the Server

To start the server:
```bash
cd server/bin
./server
```

The server will start listening on port 5051 by default.

## Testing with the Client

1. Start the server first:
```bash
cd server/bin
./server
```

2. In a new terminal, start the client:
```bash
cd client/bin
./client
```

3. The client will prompt you for commands. You can use:
   - `GET /resources` - List all available resources
   - `GET /resources/filename.txt` - Get a specific file
   - `exit` - Quit the client

## Available Resources

The server serves files from the `resources/` directory. By default, it includes:
- Text files
- HTML files
- JSON files

## Features

- HTTP/1.1 compliant responses
- Support for different content types
- Proper handling of large files
- Directory listing
- Error handling (404, 400)

## Troubleshooting

If you encounter any issues:

1. Make sure all components are compiled:
   - File system library
   - Client
   - Server

2. Check that the server is running and listening on port 5051

3. Verify that the client is connecting to the correct address and port

4. Check the server's console output for any error messages

## Notes

- The server uses a virtual file system to store and serve files
- Files are loaded from the `resources/` directory at startup
- The server supports UTF-8 encoding for text files
- Large files are handled properly with chunked reading 