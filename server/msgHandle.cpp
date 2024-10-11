#include "headers/Channel.hpp"
#include "headers/Client.hpp"
#include "headers/Message.hpp"
#include "headers/Server.hpp"
#include "headers/Utils.hpp"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <vector>

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
  else if (message.getCommand() == "PRIVMSG")
    return privmsg(message, ind);
  else if (message.getCommand() == "PART")
    return part(message, ind);
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
  std::vector<std::string> chanNameList = splitRm(msg.getArguments()[0], ",");
  bool validChanNames = true;
  std::string badChan = "";
  for (size_t i = 0; i < chanNameList.size(); i++) {
    validChanNames = validChanNames && validChanName(chanNameList[i]);
    badChan = chanNameList[i];
    if (!validChanNames)
      break;
  }
  if (!validChanNames) {
    return msgTransform(badChan + ":Invalid channel name",
                        client->GetNickname(), 476);
  }
  std::string response = "";
  for (size_t i = 0; i < chanNameList.size(); i++) {
    std::string chanName = chanNameList[i];
    Channel *chanPtr = findChannelByName(this->channels, chanName);
    if (chanPtr != NULL) {
      chanPtr->addMember(client);
    } else {
      std::cout << "chan name:" << chanName << std::endl;
      addChannel(chanName, client);
      chanPtr = findChannelByName(this->channels, chanName);
    }
    response += msgTransform("= " + chanName + " :" + chanPtr->getMemberNicks(),
                             client->GetNickname(), 353);
    response += msgTransform(chanName + " :End of /NAMES list.",
                             client->GetNickname(), 366);
  }
  return response;
}

std::string Server::part(Message msg, int ind) {
  Client *client = this->clients[this->fds[ind].fd];
  if (!client->IsRegistered()) {
    return msgTransform(":You are not registered", client->GetNickname(), 451);
  } else if (msg.getArguments().size() == 0) {
    return msgTransform(":Not enough parameters", client->GetNickname(), 461);
  }
  std::vector<std::string> chanNameList = splitRm(msg.getArguments()[0], ",");
  bool validChanNames = true;
  std::string badChan = "";
  for (size_t i = 0; i < chanNameList.size(); i++) {
    validChanNames = validChanNames && validChanName(chanNameList[i]);
    badChan = chanNameList[i];
    if (!validChanNames)
      break;
  }
  if (!validChanNames) {
    return msgTransform(badChan + ":Invalid channel name",
                        client->GetNickname(), 476);
  }
  std::string response = "";
  std::cout << chanNameList[0] << "\n";
  for (size_t i = 0; i < chanNameList.size(); i++) {
    std::string chanName = chanNameList[i];
    std::cout << chanNameList[i] << "\n";
    Channel *chanPtr = findChannelByName(this->channels, chanName);
    if (chanPtr != NULL) {
      chanPtr->removeMember(
          client, msg.getArguments().size() == 2 ? msg.getArguments()[1] : " ");
    } else {
      response += msgTransform(chanName + " :No such channel",
                               client->GetNickname(), 403);
    }
  }
  return response;
}

std::string Server::privmsg(Message message, int ind) {
  int fd = this->fds[ind].fd;
  Client *client = this->clients[fd];
  // check if client is authorised and registered
  if (!client->IsRegistered())
    return msgTransform(":You are not registered", client->GetNickname(), 451);
  // check the right number of parametrs
  if (message.getArguments().size() < 2)
    return msgTransform("PRVIMSG: Not enough parameters", client->GetNickname(),
                        461);
  if (message.getArguments().size() > 2)
    return msgTransform(":Too many parameters", client->GetNickname(), 461);
  std::string recipients = message.getArguments()[0];
  std::string msgContent = message.getArguments()[1];
  // check if recipient is valid
  if (recipients.empty())
    return msgTransform(":No recipient or message provided",
                        client->GetNickname(), 411);
  if (msgContent.empty()) {
    return msgTransform(":No message provided", client->GetNickname(), 412);
  }

  std::vector<std::string> recipientList = splitRm(recipients, ",");
  std::string response = "";
  for (const std::string &recpName : recipientList) {
    if (recpName.empty())
      return msgTransform("No recipient provided", client->GetNickname(), 411);
    if (recpName[0] == '#' || recpName[0] == '&') {
      Channel *chan = findChannelByName(this->channels, recpName);
      if (chan == NULL) {
        response += msgTransform(recpName + " :No such nck or channel",
                                 client->GetNickname(), 401);
        continue;
      } else {
        chan->sendMsg(":" + client->GetNickname() + " PRIVMSG " +
                          chan->getName() + " :" + msgContent + "\r\n",
                      client);
      }

    } else {
      // find recipient in the connected clients
      auto it = std::find_if(clients.begin(), clients.end(),
                             [recpName](const std::pair<int, Client *> &pair) {
                               return pair.second->GetNickname() == recpName;
                             });

      // if recipient doesn't exist return error
      if (it == clients.end()) {
        response += msgTransform(recpName + " :No such nick or chanel",
                                 client->GetNickname(), 401);
        continue;
      }

      // if recipient is found - send message
      Client *recipientClient = it->second;

      // check if the recipient is registered to receive a message
      if (!recipientClient->IsRegistered()) {
        return msgTransform(":Recipient isn't registered",
                            client->GetNickname(), 401);
        continue;
      }

      // deliver a message
      std::string fullMessage = ":" + client->GetNickname() + " PRIVMSG " +
                                recpName + " :" + msgContent + "\r\n";

      send(recipientClient->GetFd(), fullMessage.c_str(), fullMessage.length(),
           0);
    }
  }

  return response;
}
