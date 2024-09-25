#include "headers/Client.hpp"

Client::Client(int fd){
    this->isRegistered = false;
    this->fd = fd;
}

Client::~Client(){
    close(this->fd);
}
