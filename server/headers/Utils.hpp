#ifndef UTILS
#define UTILS

#include "Channel.hpp"
#include <cstddef>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

inline bool validChanName(std::string chanName) {
  if (chanName.size() == 0)
    return false;
  if (chanName[0] != '#' && chanName[0] != '&')
    return false;
  for (size_t i = 0; i < chanName.size(); i++) {
    // ASCII 32: space, 7: ctrl-G, 44: comma
    if (chanName[i] == 32 || chanName[i] == 7 || chanName[i] == 44)
      return false;
  }
  return true;
}

inline bool isValidNick(std::string nick) {
  if (nick.size() > 9)
    return false;
  for (std::string::size_type i = 0; i < nick.length(); i++) {
    if (!isalnum(nick[i]))
      return false;
    if (nick[i] == '-')
      return false;
    if (nick[i] == '\r')
      return false;
  }
  return true;
}

inline Channel *findChannelByName(std::vector<Channel *> chanList,
                                  std::string chanName) {
  if (chanList.empty()) {
    std::cout << "channel list empty\n";
    return NULL;
  }
  for (size_t i = 0; i < chanList.size(); i++) {
    if (chanList[i]->getName() == chanName) {
      return chanList[i];
    }
  }
  std::cout << "couldnt find channel\n";
  return NULL;
}

inline std::string serverCodeResp(int code, std::string resp, std::string nick,
                                  std::string serverName) {
  std::string prefix = ":" + serverName;
  return prefix + " " + std::to_string(code) + " " + nick + " :" + resp +
         "\r\n";
}

inline std::vector<std::string> split(std::string str, std::string delimiter) {
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> res;

  while ((pos_end = str.find(delimiter, pos_start)) != std::string::npos) {
    token = str.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }

  res.push_back(str.substr(pos_start));
  return res;
}

inline std::vector<std::string> splitRm(std::string s, std::string delimiter) {
  std::vector<std::string> tokens;
  size_t pos = 0;
  std::string token;
  while ((pos = s.find(delimiter)) != std::string::npos) {
    token = s.substr(0, pos);
    tokens.push_back(token);
    s.erase(0, pos + delimiter.length());
  }
  tokens.push_back(s);

  return tokens;
}

#endif // UTILS
