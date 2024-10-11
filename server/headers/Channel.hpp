#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include "Client.hpp"
#include <map>
#include <string>

class Channel {
private:
  std::string prefix;
  Client *creator;
  std::map<int, Client *> members;
  std::string name;
  std::string topic;

public:
  Channel(std::string name, Client *creator);
  ~Channel();
  Client *createdBy();
  std::string getPrefix();
  std::string getName();
  std::string getTopic();
  std::map<int, Client *> getMembers();

  void setPrefix(std::string);
  void setName(std::string);
  void setTopic(std::string);
  void addMember(Client *);
  void removeMember(Client *, std::string reason);
  void removeMember(int);
  std::string getMemberNicks();
  void sendMsg(std::string, Client *client);
};

#endif // CHANNEL_HPP
