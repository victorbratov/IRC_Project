#include "headers/Message.hpp"
#include "headers/Server.hpp"
#include <algorithm>
#include <iostream>
#include <string>

std::string Server::respond(Message message, int ind){
    if (message.isInvalid()) return msgTransform("Invalid message!!!");
    if (message.getCommand() == "PASS") return pass(message, ind);
    else if (message.getCommand() == "NICK") return nick(message, ind);
    else if (message.getCommand() == "USER") return user(message, ind);
    return msgTransform("UNKNOWN COMMAND");
}

std::string Server::pass(Message message, int ind){
    std::cout << "1";
    int fd = this->fds[ind].fd;
    if (this->clients[fd]->IsRegistered()) return msgTransform("462 :Unauthorized command (already registered)");
    std::cout << "2";
    if (message.getArguments().size() < 1) return msgTransform("461 PASS :Not enough parameters");
    std::cout << "3";
    if (message.getArguments()[0] != this->password) return msgTransform("464 :Password incorrect");
    std::cout << "4";
    this->clients[fd]->SetAuth(true);
    return "";
}

std::string Server::nick(Message message, int ind){
    int fd = this->fds[ind].fd;
    if (!this->clients[fd]->IsAuth()) return msgTransform("451 :You have not registered");
    if (message.getArguments().size() < 1) return msgTransform("431 :No nickname given");
    if (!isValidNick(message.getArguments()[0])) return msgTransform("432 " + message.getArguments()[0] + " :Erroneous nickname");
    if (std::find(this->nicks.begin(), this->nicks.end(), message.getArguments()[0]) != this->nicks.end()) return msgTransform("433 " + message.getArguments()[0] + " :Nickname is already in use");
    this->clients[fd]->SetNickname(message.getArguments()[0]);
    this->nicks.push_back(message.getArguments()[0]);

    return msgTransform("001 :Welcome to the Internet Relay Network");
}

std::string Server::user(Message message, int ind){
    int fd = this->fds[ind].fd;
    if (!this->clients[fd]->IsAuth()) return msgTransform("451 :You are not authorized");
    if(this->clients[fd]->IsRegistered()) return msgTransform("462 :You are already registered");
    if (message.getArguments().size() < 4) return msgTransform("461 USER :Not enough parameters");
    this->clients[fd]->SetUsername(message.getArguments()[0]);
    this->clients[fd]->SetRealName(message.getArguments()[3]);
    this->clients[fd]->SetRegistered(true);
    return msgTransform("001 :Welcome to the Internet Relay Network");
}
