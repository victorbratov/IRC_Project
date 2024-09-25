#include "headers/Message.hpp"
#include <sstream>
#include <string>
#include <vector>

Message::Message(std::string message) {
    std::stringstream ss(message);
    std::string token;
    if (message[0] == ':') {
        std::getline(ss, token, ' ');
        this->prefix = token;
    }
    std::getline(ss, token, ' ');
    this->command = token;
    while (std::getline(ss, token, ' ')) {
        this->arguments.push_back(token);
    }

}

std::string Message::getCommand() {
    return this->command;
}

std::vector<std::string> Message::getArguments() {
    return this->arguments;
}

std::string Message::getPrefix() {
    return this->prefix;
}
