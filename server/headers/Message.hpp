#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <string>
#include <vector>
class Message {

private:
  std::vector<std::string> arguments;
  std::string command, prefix;
  bool invalid;

public:
  Message(std::string message);
  ~Message();
  std::string getCommand();
  std::vector<std::string> getArguments();
  std::string getPrefix();
  bool isInvalid();
  void setCommand(std::string command);
  void setArguments(std::vector<std::string> arguments);
  void setPrefix(std::string prefix);
  void setInvalid(bool invalid);
};

#endif // MESSAGE_HPP
