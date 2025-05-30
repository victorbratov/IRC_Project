#include "headers/Message.hpp"
#include <sstream>
#include <string>
#include <vector>

// class constructor: acts as a parser for raw incoming strings of text
// brakes everything down into a prefix, command and arguments
Message::Message(std::string message) {

  this->arguments = std::vector<std::string>();
  this->command = "";
  this->prefix = "";
  std::istringstream iss(message);
  std::string token;

  if (message[0] == ':') {
    iss >> this->prefix;
    this->prefix = this->prefix.substr(1);
  }

  iss >> this->command;

  while (iss >> token) {
    // special case for trailing arguments
    if (token[0] == ':') {
      std::string trailing = token.substr(1);
      std::string tk;
      while (iss >> tk) {
        trailing += " " + tk;
      }
      this->arguments.push_back(trailing);
      break;
    }
    this->arguments.push_back(token);
  }

  this->invalid = this->command.empty();
}

Message::~Message() {}

std::string Message::getCommand() { return this->command; }

std::vector<std::string> Message::getArguments() { return this->arguments; }

std::string Message::getPrefix() { return this->prefix; }

bool Message::isInvalid() { return this->invalid; }
