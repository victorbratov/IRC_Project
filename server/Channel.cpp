#include "headers/Channel.hpp"
#include "headers/Client.hpp"
#include <iostream>
#include <map>
#include <string>
#include <sys/socket.h>
#include <utility>

Channel::Channel(std::string name, Client *creator) {
  this->name = name;
  this->creator = creator;
  this->topic = "";
  this->prefix = "";
  this->members = std::map<int, Client *>();
  addMember(creator);
}

Channel::~Channel() {}

std::string Channel::getName() { return this->name; }

std::string Channel::getTopic() {
  return topic.empty() ? "no topic set" : topic;
}

std::string Channel::getPrefix() { return this->prefix; }

Client *Channel::createdBy() { return this->creator; }

void Channel::setName(std::string name) { this->name = name; }

void Channel::setTopic(std::string topic) { this->topic = topic; }

void Channel::setPrefix(std::string prefix) { this->prefix = prefix; }

void Channel::addMember(Client *member) {
  this->members.insert({member->GetFd(), member});
  std::string join_msg = ":" + member->GetNickname() + "!" +
                         member->GetUsername() + "@localhost JOIN " + name +
                         "\r\n";
  for (const std::pair<int, Client *> ptr : members) {
    std::cout << ptr.first << " " << join_msg << std::endl;
    if (send(ptr.first, join_msg.c_str(), join_msg.length(), 0) == -1) {
      std::cerr << "couldnt send join notification" << std::endl;
    }
  }
}

void Channel::removeMember(int fd) { this->members.erase(fd); }

void Channel::removeMember(Client *member) {
  this->members.erase(member->GetFd());
}
