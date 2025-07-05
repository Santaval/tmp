#include "./includes/ProtocolParser.h"
#include <sstream>
#include <regex>

ProtocolParser::ProtocolParser() {
    // Initialize any member variables if needed
}

ProtocolParser::~ProtocolParser() {
    // Clean up any resources if needed
}

std::shared_ptr<Message> ProtocolParser::parseMessage(const std::string& rawMessage) {
    auto message = std::make_shared<Message>();
    std::vector<std::string> parts = splitMessage(rawMessage, '/');
    
    // Basic validation
    if (parts.size() < 3 || parts[0] != BEGIN_MARKER || parts.back() != END_MARKER) {
        message->setType(MessageType::UNKNOWN);
        return message;
    }

    // Parse message type
    if (parts[1] == ON_MARKER) {
        message->setType(MessageType::ANNOUNCEMENT);
        if (parts.size() != 6) return message;
        
        // Parse entity type
        if (parts[2] == SERVIDOR_MARKER) message->setEntityType(EntityType::SERVIDOR);
        else if (parts[2] == TENEDOR_MARKER) message->setEntityType(EntityType::TENEDOR);
        else return message;
        
        message->setIP(parts[3]);
        try {
            message->setPort(std::stoi(parts[4]));
        } catch (...) {
            return message;
        }
    }
    else if (parts[1] == OFF_MARKER) {
        message->setType(MessageType::DISCONNECTION);
        if (parts.size() != 6) return message;
        
        if (parts[2] == SERVIDOR_MARKER) message->setEntityType(EntityType::SERVIDOR);
        else if (parts[2] == TENEDOR_MARKER) message->setEntityType(EntityType::TENEDOR);
        else return message;
        
        message->setIP(parts[3]);
        try {
            message->setPort(std::stoi(parts[4]));
        } catch (...) {
            return message;
        }
    }
    else if (parts[1] == OBJECTS_MARKER) {
        message->setType(MessageType::OBJECT_REQUEST);
    }
    else if (parts[1] == GET_MARKER) {
        message->setType(MessageType::RESOURCE_REQUEST);
        if (parts.size() != 4) return message;
        message->setDrawingName(parts[2]);
    }
    else if (parts[1] == OK_MARKER) {
        message->setType(MessageType::SUCCESS);
        if (parts.size() != 4) return message;
        message->setContent(parts[2]);
    }
    else if (parts[1] == ERROR_MARKER) {
        message->setType(MessageType::ERROR);
        if (parts.size() != 5) return message;
        try {
            message->setErrorCode(std::stoi(parts[2]));
        } catch (...) {
            return message;
        }
        message->setErrorMessage(parts[3]);
    }

    return message;
}

std::string ProtocolParser::generateMessage(const std::shared_ptr<Message>& message) {
    if (!message) return "";
    
    std::stringstream ss;
    ss << BEGIN_MARKER << "/";

    switch (message->getType()) {
        case MessageType::ANNOUNCEMENT:
            ss << ON_MARKER << "/";
            ss << (message->getEntityType() == EntityType::SERVIDOR ? SERVIDOR_MARKER : TENEDOR_MARKER) << "/";
            ss << message->getIP() << "/" << message->getPort();
            break;
            
        case MessageType::DISCONNECTION:
            ss << OFF_MARKER << "/";
            ss << (message->getEntityType() == EntityType::SERVIDOR ? SERVIDOR_MARKER : TENEDOR_MARKER) << "/";
            ss << message->getIP() << "/" << message->getPort();
            break;
            
        case MessageType::OBJECT_REQUEST:
            ss << OBJECTS_MARKER;
            break;
            
        case MessageType::RESOURCE_REQUEST:
            ss << GET_MARKER << "/" << message->getDrawingName();
            break;
            
        case MessageType::SUCCESS:
            ss << OK_MARKER << "/" << message->getContent();
            break;
            
        case MessageType::ERROR:
            ss << ERROR_MARKER << "/" << message->getErrorCode() << "/" << message->getErrorMessage();
            break;
            
        default:
            return "";
    }

    ss << "/" << END_MARKER;
    return ss.str();
}

std::vector<std::string> ProtocolParser::splitMessage(const std::string& message, char delimiter) {
    std::vector<std::string> parts;
    std::stringstream ss(message);
    std::string part;
    
    while (std::getline(ss, part, delimiter)) {
        parts.push_back(part);
    }
    
    return parts;
}

bool ProtocolParser::validateIP(const std::string& ip) {
    std::regex ipPattern("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    return std::regex_match(ip, ipPattern);
}

bool ProtocolParser::validatePort(int port) {
    return port > 0 && port < 65536;
}

bool ProtocolParser::validateMessage(const std::string& rawMessage) {
    auto message = parseMessage(rawMessage);
    return message->getType() != MessageType::UNKNOWN;
}

bool ProtocolParser::validateMessageFormat(const std::vector<std::string>& parts) {
    if (parts.size() < 3) return false;
    if (parts[0] != BEGIN_MARKER || parts.back() != END_MARKER) return false;
    return true;
}

std::string ProtocolParser::generateAnnouncementMessage(const std::string& type, const std::string& ip, int port) {
    std::stringstream ss;
    ss << BEGIN_MARKER << "/" << ON_MARKER << "/" << type << "/" << ip << "/" << port << "/" << END_MARKER;
    return ss.str();
}

std::string ProtocolParser::generateDisconnectionMessage(const std::string& type, const std::string& ip, int port) {
    std::stringstream ss;
    ss << BEGIN_MARKER << "/" << OFF_MARKER << "/" << type << "/" << ip << "/" << port << "/" << END_MARKER;
    return ss.str();
}

std::string ProtocolParser::generateObjectRequestMessage() {
    std::stringstream ss;
    ss << BEGIN_MARKER << "/" << OBJECTS_MARKER << "/" << END_MARKER;
    return ss.str();
}

std::string ProtocolParser::generateResourceRequestMessage(const std::string& drawingName) {
    std::stringstream ss;
    ss << BEGIN_MARKER << "/" << GET_MARKER << "/" << drawingName << "/" << END_MARKER;
    return ss.str();
}

std::string ProtocolParser::generateErrorMessage(int code, const std::string& message) {
    std::stringstream ss;
    ss << BEGIN_MARKER << "/" << ERROR_MARKER << "/" << code << "/" << message << "/" << END_MARKER;
    return ss.str();
}

std::string ProtocolParser::generateSuccessMessage(const std::string& content) {
    std::stringstream ss;
    ss << BEGIN_MARKER << "/" << OK_MARKER << "/" << content << "/" << END_MARKER;
    return ss.str();
} 