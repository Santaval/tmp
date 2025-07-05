# Protocol Parser System

A robust C++ implementation of a protocol parser for a collaborative network system involving Servers (SERVIDOR), Forks (TENEDOR), and Clients.

## Features

- Message parsing and validation for UDP/TCP communications
- Resource management for drawings and objects
- State management for active servers and forks
- Thread-safe operations
- Error handling with standardized error codes
- Timeout and retry mechanisms for TCP connections

## Building the Project

### Prerequisites

- CMake (version 3.10 or higher)
- C++17 compatible compiler
- Thread support library

### Build Steps

```bash
# Create a build directory
mkdir build && cd build

# Generate build files
cmake ..

# Build the project
make
```

## Usage

### Basic Message Parsing

```cpp
#include "ProtocolParser.h"
#include "Message.h"

// Create a protocol parser instance
ProtocolParser parser;

// Parse a UDP announcement message
std::string rawMessage = "BEGIN/ON/SERVIDOR/192.168.1.10/8080/END";
std::shared_ptr<Message> message = parser.parseMessage(rawMessage);

// Check message type and content
if (message->getType() == MessageType::ANNOUNCEMENT) {
    std::string ip = message->getIP();
    int port = message->getPort();
    // Process the announcement...
}
```

### Network Communication

```cpp
#include "NetworkManager.h"

// Create a network manager instance
NetworkManager networkManager;

// Initialize UDP and TCP
networkManager.initializeUDP(8080);
networkManager.initializeTCP(8081);

// Register message callback
networkManager.registerMessageCallback([](const Message& msg) {
    // Handle incoming messages
});

// Start listening for messages
networkManager.startListening();

// Send a UDP message
networkManager.sendUDPMessage("192.168.1.10", 8080, 
    "BEGIN/ON/SERVIDOR/192.168.1.10/8080/END");
```

### Resource Management

```cpp
#include "ResourceManager.h"

// Create a resource manager instance
ResourceManager resourceManager;

// Load a drawing
if (resourceManager.loadDrawing("ballena")) {
    // Get drawing content
    std::string content = resourceManager.getDrawingContent("ballena");
}

// Get available drawings
std::vector<std::string> drawings = resourceManager.getAvailableDrawings();
```

### State Management

```cpp
#include "StateManager.h"

// Create a state manager instance
StateManager stateManager;

// Add a new server
stateManager.addEntity(EntityType::SERVIDOR, "192.168.1.10", 8080);

// Update server's available objects
std::vector<std::string> objects = {"object1", "object2"};
stateManager.updateEntityObjects(EntityType::SERVIDOR, "192.168.1.10", objects);

// Get active servers
std::vector<EntityInfo> activeServers = stateManager.getActiveEntities(EntityType::SERVIDOR);
```

## Message Formats

### UDP Announcements
```
BEGIN/ON/SERVIDOR/{IP}/{PORT}/END
BEGIN/ON/TENEDOR/{IP}/{PORT}/END
```

### UDP Disconnection
```
BEGIN/OFF/SERVIDOR/{IP}/{PORT}/END
```

### TCP Object Requests
```
BEGIN/OBJECTS/END
Response: BEGIN/OK/OBJECT1\nOBJECT2/END
```

### Client Resource Requests
```
BEGIN/GET/{DRAWING_NAME}/END
Response: BEGIN/OK/{CONTENT}/END
```

### Error Messages
```
BEGIN/ERROR/{CODE}/{MESSAGE}/END
```

## Error Codes

- 100: Connection cannot be established
- 201: Drawing not found
- 3xx: Protocol errors (malformed messages)

## Thread Safety

All operations in the system are thread-safe using mutexes. The following components are designed for concurrent access:

- ResourceManager: Thread-safe resource access
- StateManager: Thread-safe state management
- NetworkManager: Thread-safe message handling

## Timeout and Retry

The NetworkManager implements:
- 5-second timeout for TCP connections
- 3 retry attempts for failed operations

## Contributing

1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create a new Pull Request 