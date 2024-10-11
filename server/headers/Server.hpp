#ifndef SERVER_HPP
#define SERVER_HPP

#include "Channel.hpp"
#include "Client.hpp"
#include "Message.hpp"
#include <arpa/inet.h>
#include <map>
#include <netdb.h>
#include <poll.h>
#include <string>
#include <unistd.h>
#include <vector>

class Server {
private:
  int socket_fd;
  struct pollfd *fds;
  int clients_online;
  int max_clients;
  std::string name;
  std::map<int, Client *> clients;
  std::vector<std::string> nicks;
  std::vector<Channel *> channels;

public:
  Server(std::string name, int max_clients, std::string port);
  ~Server();
  void start();

private:
  void setSocket(std::string port);
  void addPoll(int newfd);
  void removePoll(int i);
  std::string msgTransform(std::string msg, std::string nick, int code);

public:
  void add_client();
  void remove_client(Client *client);
  void send_message(std::string message, Client *client);
  void client_message(int index);
  std::string respond(Message msg, int ind);
  std::string nick(Message msg, int ind);
  std::string user(Message msg, int ind);
  std::string join(Message msg, int ind);
  void addChannel(std::string chanName, Client *creator);
  std::string privmsg(Message msg, int ind);
  std::string part(Message msg, int ind);
  std::string quit(Message msg, int ind);
};

#endif // SERVER_HPP
