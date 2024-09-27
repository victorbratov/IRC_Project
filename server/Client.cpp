#include "headers/Client.hpp"
#include <sys/socket.h>

Client::Client(int fd){
    this->isRegistered = false;
    this->isAuth = false;
    this->fd = fd;
    this->username = "";
    this->realName = "";
    this->nickname = "";
    this->addr = sockaddr_storage();
    this->addrLen = sizeof(this->addr);
    this->mode = Mode{0, 0, 0, 0, 0, 0, 0};
}

Client::~Client(){
    close(this->fd);
}

int Client::GetFd(){
    return this->fd;
}

bool Client::IsRegistered(){
    return this->isRegistered;
}

void Client::SetRegistered(bool isRegistered){
    this->isRegistered = isRegistered;
}

void Client::SetUsername(std::string username){
    this->username = username;
}

std::string Client::GetUsername(){
    return this->username;
}

bool Client::IsAuth(){
    return this->isAuth;
}

void Client::SetAuth(bool isAuth){
    this->isAuth = isAuth;
}

void Client::SetRealName(std::string realName){
    this->realName = realName;
}

std::string Client::GetRealName(){
    return this->realName;
}

void Client::SetNickname(std::string nick){
    this->nickname = nick;
}

std::string Client::GetNickname(){
    return this->nickname;
}
