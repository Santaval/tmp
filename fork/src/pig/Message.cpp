#include "./includes/Message.h"
#include <sstream>


std::string Message::toString() const {
    std::stringstream ss;
    ss << "Message Type: " << static_cast<int>(type_) << "\n"
       << "Entity Type: " << static_cast<int>(entityType_) << "\n"
       << "IP: " << ip_ << "\n"
       << "Port: " << port_ << "\n"
       << "Drawing Name: " << drawingName_ << "\n"
       << "Content: " << content_ << "\n"
       << "Error Code: " << errorCode_ << "\n"
       << "Error Message: " << errorMessage_ << "\n"
       << "Objects: ";
    for (const auto& obj : objects_) {
        ss << obj << ", ";
    }
    return ss.str();
}