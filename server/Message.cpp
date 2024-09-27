#include "headers/Message.hpp"
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

// Constructor for Message class
Message::Message(std::string message) {
    // if message is valid
    this->invalid = true;

    // commands for the IRC protocol
    std::vector <std::string> commands = { "PASS", "NICK", "USER", "PRIVMSG", "MODE", "PING", "PONG"};

    // check total characters in the message
    int characters = 0;

    // using stringstream to tokenize the input message
    std::stringstream ss(message);
    std::string token;

    // if the message starts with a colon, it contains a prefix
    if (message[0] == ':') {
        // prefix, which ends at the first space
        std::getline(ss, token, ' ');

        // checking invalid prefix, only colon
        if (token.length() == 1) {
            std::cout << "Message not valid" << std::endl;
            this->invalid = false;
            return;
        }

        // store the prefix
        this->prefix = token;
        characters += token.length();//update the character count
    }
    //extract command part of the message
    std::getline(ss, token, ' ');

    // check the command is either a known command or a valid numeric reply code
    try {
        // if command NOT in command list
        if (std::find(commands.begin(), commands.end(), token) == commands.end()) {
            // convert token to an integer, check if it's a valid numeric reply code
            int numericCommand = std::stoi(token);
            if (numericCommand < 100 || numericCommand > 127) {
                std::cout << "Message not valid" << std::endl;
                this->invalid = false;
                return;
            }
        }
    }
    // processing exception for NOT valid number
    catch (std::invalid_argument& e) {
        std::cout << "Message not valid: Invalid command" << std::endl;
        this->invalid = false;
        return;
    }
    // processing out the range exception when parsing the token
    catch (std::out_of_range& e) {
        std::cout << "Message not valid: Command out of range" << std::endl;
        this->invalid = false;
        return;
    }

    // store the command
    this->command = token;
    characters += token.length();// update the character count

    // loop rest of the tokens arguments
    while (std::getline(ss, token, ' ')) {
        // if token starts with a colon, everything that follows treat as one argument
        if (token[0] == ':') {
            // collect as a single trailing argument
            std::string trailing;
            std::getline(ss, trailing);

            // push the trailing argument with removing the initial colon and break
            this->arguments.push_back(token.substr(1) + " " + trailing);
            break;
        } else {
            // treat the token as a normal argument
            this->arguments.push_back(token);
        }

        // update the character count
        characters += token.length();
    }

    // if the total character count overrun 512 characters (from the protocol limit)
    if (characters > 512) {
        std::cout << "Message is not valid" << std::endl;
        this->invalid = false;
    }
}

// method to return command in uppercase
std::string Message::getCommand() {
    // convert command to uppercase
    std::transform(this->command.begin(), this->command.end(), this->command.begin(), ::toupper);
    return this->command;
}

// method to return the list of arguments
std::vector<std::string> Message::getArguments() {
    return this->arguments;
}

// method to return the prefix
std::string Message::getPrefix() {
    return this->prefix;
}

// method to reformat the message back to its original string form
std::string Message::toString() {
    std::string formattedMsg;

    // if it is exist add prefix followed by a space
    if (!this->prefix.empty()) {
        formattedMsg += this->prefix + " ";
    }

    // add the command
    formattedMsg += this->command;

    // add argument, prefixed by a space
    for (const auto& arg : this->arguments) {
        formattedMsg += " " + arg;
    }
    // return the reconstructed message as a string
    return formattedMsg;
}
