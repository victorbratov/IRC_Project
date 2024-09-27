#include "headers/Message.hpp"
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

Message::Message(std::string message) {
    // std::stringstream ss(message);
    // std::string token;
    // if (message[0] == ':') {
    //     std::getline(ss, token, ' ');
    //     this->prefix = token.substr(1);
    // }
    // std::getline(ss, token, ' ');
    // this->command = token;
    // while (std::getline(ss, token, ' ')) {
    //     if (!token.empty() && token[0] == ':') {
    //         std::string remainingArgs;
    //         std::getline(ss, remainingArgs);
    //         token = token.substr(1) + " " + remainingArgs;
    //         this->arguments.push_back(token);
    //         break;
    //     }
    //     this->arguments.push_back(token);
    // }
    std::vector <std::string> commands = { "PASS", "NICK", "USER", "PRIVMSG", "MODE", "PING", "PONG"};
    int characters = 0;
    std::stringstream ss(message);
    std::string token;
    if (message[0] == ':') {
        std::getline(ss, token, ' ');
        if (token.length()==1){
            std::cout << "Message not valid"<< std::endl;
            return;
        }
        this->prefix = token;
        characters+=token.length();

    }
    std::getline(ss, token, ' ');
    if (count(commands.begin(), commands.end(), token) <= 0){
        if (stoi(token) < 100 && stoi(token) > 127){
            std::cout << "Message not valid"<< std::endl;
            return;
        }
    }
    this->command = token;
    characters+=token.length();
    while (std::getline(ss, token, ' ')) {
        this->arguments.push_back(token);
        characters+=token.length();
    }
    if (characters>512){
        std::cout << "Message is not valid"<< std::endl;
    }
}



     std::string Message::getCommand() {
     std::transform(this->command.begin(), this->command.end(), this->command.begin(), ::toupper);
     return this->command;
}

std::vector<std::string> Message::getArguments() {
     return this->arguments;
 }

std::string Message::getPrefix() {
    return this->prefix;
}
std::string Message::toString() {
    std::string formattedMsg;


}
