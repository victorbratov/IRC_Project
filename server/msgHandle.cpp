#include "headers/Message.hpp"
#include "headers/Server.hpp"
#include <algorithm>
#include <iostream>
#include <string>

std::string Server::respond(Message message, int ind){
    if (message.isInvalid()) return "400 :Invalid message";
    if (message.getCommand() == "PASS") return pass(message, ind);
    else if (message.getCommand() == "NICK") return nick(message, ind);
    else if (message.getCommand() == "USER") return user(message, ind);
    return "UNKNOWN COMMAND";
}

std::string Server::pass(Message message, int ind){
    std::cout << "1";
    int fd = this->fds[ind].fd;
    if (this->clients[fd]->IsRegistered()) return "462 :You may not reregister";
    std::cout << "2";
    if (message.getArguments().size() < 1) return "461 :Not enough parameters";
    std::cout << "3";
    if (message.getArguments()[0] != this->password) return "464 :Password incorrect";
    std::cout << "4";
    this->clients[fd]->SetAuth(true);
    return "001 :Welcome to the Internet Relay Network";
}

std::string Server::nick(Message message, int ind){
    int fd = this->fds[ind].fd;
    if (!this->clients[fd]->IsAuth()) return "451 :You are not authorized";
    if (message.getArguments().size() < 1) return "431 :No nickname given";
    if (!isValidNick(message.getArguments()[0])) return "432 :Erroneous nickname";
    if (std::find(this->nicks.begin(), this->nicks.end(), message.getArguments()[0]) != this->nicks.end()) return "433 :Nickname is already in use";
    this->clients[fd]->SetNickname(message.getArguments()[0]);
    this->nicks.push_back(message.getArguments()[0]);

    return "001 :Welcome to the Internet Relay Network";
}

std::string Server::user(Message message, int ind){
    int fd = this->fds[ind].fd;
    if (!this->clients[fd]->IsAuth()) return "451 :You are not authorized";
    if(this->clients[fd]->IsRegistered()) return "462 :You are already registered";
    if (message.getArguments().size() < 4) return "461 :Not enough parameters";
    this->clients[fd]->SetUsername(message.getArguments()[0]);
    this->clients[fd]->SetRealName(message.getArguments()[3]);
    this->clients[fd]->SetRegistered(true);
    return "001 :Welcome to the Internet Relay Network";
}
