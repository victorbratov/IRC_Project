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

std::string Server::notice(Message message, int ind){
    int fd = this->fds[ind].fd;
    Client *client = this->clients[fd];
    if (message.getArguments().size() != 2) return "Wrong number of parameters"
    std::string recipient = message.getArguments()[0];
    if (message.getArguments()[1][0] !=":") return "Too many targets"
    std::string msgContent = message.getArguments()[1].substr(1);
    for (int i=3; i<message.getArguments().length(); i++){
        msgContent+=message.getArguments()[i]+" ";
    }

    //find recipient in the connected clients
    auto it = std::find_if(clients.begin(), clients.end(),
                               [&recipient](const std::pair<int, Client *> &pair) {
                                   return pair.second->GetNickname() == recipient;
                               });

    if (it == clients.end()) return "No such nick or chanel"
    //if recipient is found - send message
    Client *recipientClient = it->second;

    //deliver a message
    std::string fullMessage = ":" + client->GetNickname() + " PRIVMSG " + recipient + " :" + msgContent + "\r\n";
    send(recipientClient->GetFd(), fullMessage.c_str(), fullMessage.length(), 0);

    return ""



}




std::string Server::privmsg(Message message, int ind) {
    int fd = this->fds[ind].fd;
    Client *client = this->clients[fd];
    //check if client is authorised and registered
    if (!this->clients[fd]->IsAuth()) return msgTransform(":You are not authorized", client->GetNickname(), 451);

    //check the right number of parameters
    if (message.getArguments().size() < 2) return msgTransform("PRVIMSG: Not enough parameters", client->GetNickname(), 461);
    std::string recipient = message.getArguments()[0];
    if (message.getArguments()[1][0] !=":") return msgTransform("PRVIMSG: Too many targets", client->GetNickname(), 407);
    std::string msgContent = message.getArguments()[1].substr(1);
    for (int i=3; i<message.getArguments().length(); i++){
        msgContent+=message.getArguments()[i]+" ";
    }

    //check if recipient is valid
    if (recipient.empty() || msgContent.empty()) return msgTransform(":No recipient or message provided", client->GetNickname(), 411);
    if (msgContent.empty()) {
        return msgTransform(":No message provided", client->GetNickname(), 412);
    }

    //find recipient in the connected clients
    auto it = std::find_if(clients.begin(), clients.end(),
                           [&recipient](const std::pair<int, Client*>& pair) {
                               return pair.second->GetNickname() == recipient;
                           });

    // if recipient doesn't exist return error
    if (it == clients.end()) return msgTransform(recipient + " :No such nick or chanel", client->GetNickname(), 401);

    //if recipient is found - send message
    Client *recipientClient = it->second;

    //check if the recipient is registered to receive a message
    if (!recipientClient->IsRegistered()) return msgTransform(":Recipient isn't registered", client->GetNickname(), 401);

    //deliver a message
    std::string fullMessage = ":" + client->GetNickname() + " PRIVMSG " + recipient + " :" + msgContent + "\r\n";
    send(recipientClient->GetFd(), fullMessage.c_str(), fullMessage.length(), 0);

    return ""
}
