#include "headers/Message.hpp"
#include "headers/Server.hpp"
#include <algorithm>
#include <string>

std::string Server::respond(Message message, int ind){
    if (message.getCommand() == "PASS") return pass(message, ind);
    else if (message.getCommand() == "NICK") return nick(message, ind);
    else if (message.getCommand() == "USER") return user(message, ind);
    return "UNKNOWN COMMAND";
}

std::string Server::pass(Message message, int ind){
    if (this->clients[ind]->IsRegistered()) return "462 :You may not reregister";
    if (message.getArguments().size() < 1) return "461 :Not enough parameters";
    if (message.getArguments()[0] != this->password) return "464 :Password incorrect";
    this->clients[ind]->SetAuth(true);
    return "001 :Welcome to the Internet Relay Network";
}

std::string Server::nick(Message message, int ind){
    if (!this->clients[ind]->IsAuth()) return "451 :You are not authorized";
    if (message.getArguments().size() < 1) return "431 :No nickname given";
    if (!isValidNick(message.getArguments()[0])) return "432 :Erroneous nickname";
    if (std::find(this->nicks.begin(), this->nicks.end(), message.getArguments()[0]) != this->nicks.end()) return "433 :Nickname is already in use";
    this->clients[ind]->SetNickname(message.getArguments()[0]);
    this->nicks.push_back(message.getArguments()[0]);

    return "001 :Welcome to the Internet Relay Network";
}

std::string Server::user(Message message, int ind){
    if (!this->clients[ind]->IsAuth()) return "451 :You are not authorized";
    if(this->clients[ind]->IsRegistered()) return "462 :You are already registered";
    if (message.getArguments().size() < 4) return "461 :Not enough parameters";
    this->clients[ind]->SetUsername(message.getArguments()[0]);
    this->clients[ind]->SetRealName(message.getArguments()[3]);
    this->clients[ind]->SetRegistered(true);
    return "001 :Welcome to the Internet Relay Network";
}
