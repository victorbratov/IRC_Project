#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <sys/socket.h>
#include <unistd.h>

class Client {
private:
  bool isRegistered, online;
  int fd;
  std::string nickname, username;
  std::string realName;
  sockaddr_storage addr;
  socklen_t addrLen;

public:
  Client(int client_fd);
  ~Client();

public:
  bool IsRegistered();
  int GetFd();
  std::string GetNickname();
  std::string GetRealName();
  std::string GetHost();
  std::string GetUsername();
  sockaddr_storage GetAddr();
  socklen_t GetAddrLen();

  void SetNickname(std::string nickname);
  void SetRealName(std::string realName);
  void SetUsername(std::string username);
  void SetRegistered(bool isRegistered);
  bool getOnline();
  void setOnline(bool val);
};

#endif // CLIENT_HPP
