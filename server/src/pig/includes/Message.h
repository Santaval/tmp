#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <vector>

enum class MessageType {
    ANNOUNCEMENT,    // BEGIN/ON/SERVIDOR|TENEDOR/{IP}/{PORT}/END
    DISCONNECTION,   // BEGIN/OFF/SERVIDOR|TENEDOR/{IP}/{PORT}/END
    OBJECT_REQUEST,  // BEGIN/OBJECTS/END
    RESOURCE_REQUEST, // BEGIN/GET/{DRAWING_NAME}/END
    SUCCESS,        // BEGIN/OK/{CONTENT}/END
    ERROR,          // BEGIN/ERROR/{CODE}/{MESSAGE}/END
    UNKNOWN
};

enum class EntityType {
    SERVIDOR,
    TENEDOR,
    UNKNOWN
};

class Message {
public:
    // Constructor for empty message
    Message() : type_(MessageType::UNKNOWN), entityType_(EntityType::UNKNOWN), 
                port_(0), errorCode_(0) {}

    // Getters
    MessageType getType() const { return type_; }
    EntityType getEntityType() const { return entityType_; }
    std::string getIP() const { return ip_; }
    int getPort() const { return port_; }
    std::string getDrawingName() const { return drawingName_; }
    std::string getContent() const { return content_; }
    int getErrorCode() const { return errorCode_; }
    std::string getErrorMessage() const { return errorMessage_; }
    std::vector<std::string> getObjects() const { return objects_; }

    // Setters
    void setType(MessageType type) { type_ = type; }
    void setEntityType(EntityType type) { entityType_ = type; }
    void setIP(const std::string& ip) { ip_ = ip; }
    void setPort(int port) { port_ = port; }
    void setDrawingName(const std::string& name) { drawingName_ = name; }
    void setContent(const std::string& content) { content_ = content; }
    void setErrorCode(int code) { errorCode_ = code; }
    void setErrorMessage(const std::string& message) { errorMessage_ = message; }
    void setObjects(const std::vector<std::string>& objects) { objects_ = objects; }

    // Helper methods
    bool isValid() const;
    std::string toString() const ;

private:
    MessageType type_;
    EntityType entityType_;
    std::string ip_;
    int port_;
    std::string drawingName_;
    std::string content_;
    int errorCode_;
    std::string errorMessage_;
    std::vector<std::string> objects_;
};

#endif // MESSAGE_H 