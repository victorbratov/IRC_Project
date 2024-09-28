#include "headers/Client.hpp"
#include "headers/Message.hpp"
#include "headers/Server.hpp"
#include <algorithm>
#include <iostream>
#include <string>

std::string Server::respond(Message message, int ind){
    if (message.isInvalid()) return "Invalid message!!!\r\n";
    if (message.getCommand() == "PASS") return pass(message, ind);
    else if (message.getCommand() == "NICK") return nick(message, ind);
    else if (message.getCommand() == "USER") return user(message, ind);
    return "UNKNOWN COMMAND\r\n";
}

std::string Server::pass(Message message, int ind){
    std::cout << "1";
    int fd = this->fds[ind].fd;
    Client *client = this->clients[fd];
    if (client->IsRegistered()) return msgTransform(":Unauthorized command (already registered)", client->GetNickname(), 462);
    std::cout << "2";
    if (message.getArguments().size() < 1) return msgTransform("PASS :Not enough parameters", client->GetNickname(), 461);
    std::cout << "3";
    if (message.getArguments()[0] != this->password) return msgTransform(":Password incorrect", client->GetNickname(), 464);
    std::cout << "4";
    this->clients[fd]->SetAuth(true);
    return "";
}

std::string Server::nick(Message message, int ind){
    int fd = this->fds[ind].fd;
    Client *client = this->clients[fd];
    if (!this->clients[fd]->IsAuth()) return msgTransform(":You have not registered", client->GetNickname(), 451);
    if (message.getArguments().size() < 1) return msgTransform(":No nickname given", client->GetNickname(), 431);
    if (!isValidNick(message.getArguments()[0])) return msgTransform(message.getArguments()[0] + " :Erroneous nickname", client->GetNickname(), 432);
    if (std::find(this->nicks.begin(), this->nicks.end(), message.getArguments()[0]) != this->nicks.end()) return msgTransform(message.getArguments()[0] + " :Nickname is already in use", client->GetNickname(), 433);
    this->clients[fd]->SetNickname(message.getArguments()[0]);
    this->nicks.push_back(message.getArguments()[0]);

    return msgTransform(":Welcome to the Internet Relay Network", client->GetNickname(), 001);
}

std::string Server::user(Message message, int ind){
    int fd = this->fds[ind].fd;
    Client *client = this->clients[fd];
    if (!this->clients[fd]->IsAuth()) return msgTransform(":You are not authorized", client->GetNickname(), 451);
    if(this->clients[fd]->IsRegistered()) return msgTransform(":You are already registered", client->GetNickname(), 462);
    if (message.getArguments().size() < 4) return msgTransform("USER :Not enough parameters", client->GetNickname(), 461);
    this->clients[fd]->SetUsername(message.getArguments()[0]);
    this->clients[fd]->SetRealName(message.getArguments()[3]);
    this->clients[fd]->SetRegistered(true);
    return msgTransform(":Welcome to the Internet Relay Network", client->GetNickname(), 001);
}
