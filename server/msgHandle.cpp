#include "headers/Client.hpp"
#include "headers/Message.hpp"
#include "headers/Server.hpp"
#include "headers/Utils.hpp"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>

std::string Server::respond(Message message, int ind) {
  if (message.isInvalid())
    return "Invalid message!!!\r\n";
  else if (message.getCommand() == "CAP")
    return "";
  else if (message.getCommand() == "NICK")
    return nick(message, ind);
  else if (message.getCommand() == "USER")
    return user(message, ind);
  else if (message.getCommand() == "JOIN")
    return join(message, ind);
  return msgTransform(":Unknown command",
                      this->clients[this->fds[ind].fd]->GetNickname(), 421);
}

std::string Server::nick(Message message, int ind) {
  int fd = this->fds[ind].fd;
  Client *client = this->clients[fd];
  if (message.getArguments().size() < 1)
    return msgTransform(":No nickname given", client->GetNickname(), 431);
  if (!isValidNick(message.getArguments()[0]))
    return msgTransform(message.getArguments()[0] + " :Erroneous nickname",
                        client->GetNickname(), 432);
  if (std::find(this->nicks.begin(), this->nicks.end(),
                message.getArguments()[0]) != this->nicks.end())
    return msgTransform(message.getArguments()[0] +
                            " :Nickname is already in use",
                        client->GetNickname(), 433);
  this->clients[fd]->SetNickname(message.getArguments()[0]);
  this->nicks.push_back(message.getArguments()[0]);

  return msgTransform(":Welcome to the Internet Relay Network," +
                          client->GetNickname(),
                      client->GetNickname(), 001);
}

std::string Server::user(Message message, int ind) {
  int fd = this->fds[ind].fd;
  Client *client = this->clients[fd];
  if (this->clients[fd]->IsRegistered())
    return msgTransform(":You are already registered", client->GetNickname(),
                        462);
  if (message.getArguments().size() < 4)
    return msgTransform("USER :Not enough parameters", client->GetNickname(),
                        461);
  this->clients[fd]->SetUsername(message.getArguments()[0]);
  this->clients[fd]->SetRealName(message.getArguments()[3]);
  this->clients[fd]->SetRegistered(true);
  return msgTransform(":Welcome to the Internet Relay Network",
                      client->GetNickname(), 001);
}

std::string Server::join(Message msg, int ind) {
  Client *client = this->clients[this->fds[ind].fd];
  if (!client->IsRegistered()) {
    return msgTransform(":You are not registered", client->GetNickname(), 451);
  } else if (msg.getArguments().size() == 0) {
    return msgTransform(":Not enough parameters", client->GetNickname(), 461);
  }
  bool validChanNames = true;
  std::string badChan = "";
  for (size_t i = 0; i < msg.getArguments().size(); i++) {
    validChanNames = validChanNames && validChanName(msg.getArguments()[i]);
    badChan = msg.getArguments()[i];
    if (!validChanNames)
      break;
  }
  if (!validChanNames) {
    return msgTransform(badChan + ":Invalid channel name",
                        client->GetNickname(), 476);
  }
  std::string response = "";
  for (size_t i = 0; i < msg.getArguments().size(); i++) {
    std::string chanName = msg.getArguments()[i];
    Channel *chanPtr = findChannelByName(this->channels, chanName);
    if (chanPtr != NULL) {
      chanPtr->addMember(client);
    } else {
      std::cout << "chan name:" << chanName << std::endl;
      addChannel(chanName, client);
      chanPtr = findChannelByName(this->channels, chanName);
    }
    response += msgTransform(chanPtr->getName() + " :" + chanPtr->getTopic(),
                             client->GetNickname(), 332);
  }
  return response;
}
