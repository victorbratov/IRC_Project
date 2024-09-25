#include "headers/Server.hpp"
#include "headers/Client.hpp"
#include <cstring>
#include <iostream>
#include <map>
#include <ostream>
#include <string>
#include <sys/socket.h>


Server::Server(std::string name, int max_clients, std::string port){
    this->name = name;
    this->max_clients = max_clients;
    this->fds = new struct pollfd[max_clients];
    setSocket(port);
    this->fds[0].fd = this->socket_fd;
    this->fds[0].events = POLLIN;
    this->clients_online = 1;
}

Server::~Server(){
    if (this->fds != NULL){
        delete[] this->fds;
    }
    close(this->socket_fd);
    std::map<int, Client *>::iterator iterator = this->clients.begin();
    while (iterator != this->clients.end()){
        delete iterator->second;
        iterator++;
    }
    this->clients.clear();
}

void Server::add_client(){
    struct sockaddr_storage client_addr;
    socklen_t addr_size = sizeof client_addr;
    int newfd = accept(this->socket_fd, (struct sockaddr *)&client_addr, &addr_size);

    if (newfd == -1) {
        std::cerr << "accept" << std::endl;
        return;
    }
    else{
        addPoll(newfd);
        std::string msg = "Welcome to " + this->name + "!";
        if (send(newfd, msg.c_str(), msg.length(), 0) == -1) {
            std::cerr << "send" << std::endl;
        }
        std::cout << "server: new connection from " << newfd << std::endl;
    }
}

void Server::start(){
    while (1) {
        std::cout << this->fds[0].fd <<std::endl;
        int poll_c = poll(this->fds, this->clients_online, -1);
        if (poll_c == -1) {
            std::cerr << "poll" << std::endl;
            exit(1);
        }

        for (int i = 0; i < this->clients_online; i++){
            std::cout << "tst" << std::endl;
            if (this->fds[i].revents & POLLIN){
                if (this->fds[i].fd == this->socket_fd){
                    add_client();
                } else {
                    client_message(i);
                }
            }
        }
    }
}

void Server::addPoll(int newfd){
    if (this->clients_online == this->max_clients){
        std::cerr << "server: max clients reached" << std::endl;
        close(newfd);
        return;
    }

    this->fds[this->clients_online].fd = newfd;
    this->fds[this->clients_online].events = POLLIN;
    this->clients.insert(std::pair<int, Client*>(newfd, new Client(newfd)));
    this->clients_online++;
}

void Server::removePoll(int i){
    close(this->fds[i].fd);
    this->fds[i] = this->fds[this->clients_online - 1];
    this->clients.erase(this->fds[i].fd);
    this->clients_online--;
}

void Server::setSocket(std::string port){
    int yes = 1;
    int status;
    struct addrinfo hints, *server_info, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    status = getaddrinfo(NULL, port.c_str(), &hints, &server_info);
    if(status != 0){
        std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
        exit(1);
    }

    for(p = server_info; p != NULL; p = p->ai_next){
        this->socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if(this->socket_fd == -1){
            std::cerr << "server: socket" << std::endl;
            continue;
        }

        status = setsockopt(this->socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
        if(status == -1){
            std::cerr << "setsockopt" << std::endl;
            exit(1);
        }

        status = bind(this->socket_fd, p->ai_addr, p->ai_addrlen);
        if(status == -1){
            close(this->socket_fd);
            std::cerr << "server: bind" << std::endl;
            continue;
        }
        break;
    }

    freeaddrinfo(server_info);

    if(p == NULL){
        std::cerr << "server: failed to bind" << std::endl;
        exit(1);
    }

    status = listen(this->socket_fd, this->max_clients);
    if(status == -1){
        std::cerr << "listen" << std::endl;
        exit(1);
    }

    std::cout << "server: waiting for connections on port: " << port << std::endl;
}

void Server::client_message(int index){
    char buf[1024];
    int nbytes = recv(this->fds[index].fd, buf, sizeof buf, 0);
    if(nbytes <= 0){
        if(nbytes == 0){
            std::cout << "server: socket " << this->fds[index].fd << " hung up" << std::endl;
        } else {
            std::cerr << "recv" << std::endl;
        }
        close(fds[index].fd);
        removePoll(index);
    } else {
        std::string msg = std::string(buf, nbytes);
        std::string response = "server: " + msg;
        std::cout << response << std::endl;
        if (send(this->fds[index].fd, response.c_str(), response.length(), 0) == -1) {
            std::cerr << "send" << std::endl;
        }
    }

    memset(buf, 0, 1024);
}
