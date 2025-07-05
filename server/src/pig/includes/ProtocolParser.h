#ifndef PROTOCOL_PARSER_H
#define PROTOCOL_PARSER_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Message.h"

class ProtocolParser {
public:
    ProtocolParser();
    ~ProtocolParser();

    // Protocol markers
    static constexpr const char* BEGIN_MARKER = "BEGIN";
    static constexpr const char* END_MARKER = "END";
    static constexpr const char* ON_MARKER = "ON";
    static constexpr const char* OFF_MARKER = "OFF";
    static constexpr const char* SERVIDOR_MARKER = "SERVIDOR";
    static constexpr const char* TENEDOR_MARKER = "TENEDOR";
    static constexpr const char* OBJECTS_MARKER = "OBJECTS";
    static constexpr const char* GET_MARKER = "GET";
    static constexpr const char* OK_MARKER = "OK";
    static constexpr const char* ERROR_MARKER = "ERROR";

    // Message parsing methods
    std::shared_ptr<Message> parseMessage(const std::string& rawMessage);
    bool validateMessage(const std::string& rawMessage);
    
    // Message generation methods
    std::string generateAnnouncementMessage(const std::string& type, const std::string& ip, int port);
    std::string generateDisconnectionMessage(const std::string& type, const std::string& ip, int port);
    std::string generateObjectRequestMessage();
    std::string generateResourceRequestMessage(const std::string& drawingName);
    std::string generateErrorMessage(int code, const std::string& message);
    std::string generateSuccessMessage(const std::string& content);

    // Generate a raw message string from a Message object
    std::string generateMessage(const std::shared_ptr<Message>& message);

private:
    // Helper methods
    std::vector<std::string> splitMessage(const std::string& message, char delimiter);
    bool validateIP(const std::string& ip);
    bool validatePort(int port);
    bool validateMessageFormat(const std::vector<std::string>& parts);
};

#endif // PROTOCOL_PARSER_H 