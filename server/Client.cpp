#include "headers/Client.hpp"
#include <cstdio>
#include <sys/socket.h>

// ckass constructor
Client::Client(int fd) {
  this->isRegistered = false;
  this->fd = fd;
  this->username = "";
  this->realName = "";
  this->nickname = "";
  this->addr = sockaddr_storage();
  this->addrLen = sizeof(this->addr);
}

Client::~Client() { close(this->fd); }

int Client::GetFd() { return this->fd; }

bool Client::IsRegistered() { return this->isRegistered; }

void Client::SetRegistered(bool isRegistered) {
  this->isRegistered = isRegistered;
}

void Client::SetUsername(std::string username) { this->username = username; }

std::string Client::GetUsername() { return this->username; }

void Client::SetRealName(std::string realName) { this->realName = realName; }

std::string Client::GetRealName() { return this->realName; }

void Client::SetNickname(std::string nick) { this->nickname = nick; }

std::string Client::GetNickname() { return this->nickname; }

bool Client::getOnline() { return this->online; }

void Client::setOnline(bool val) { this->online = val; }
