#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include <string>
#include <poll.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <map>

class Server{
    private:
        int socket_fd;
        struct pollfd *fds;
        int clients_online;
        int max_clients;
        std::string name;
        std::map<int, Client *> clients;

    public:
        Server(std::string name, int max_clients, std::string port);
        ~Server();
        void start();

    private:
        void setSocket(std::string port);
        void addPoll(int newfd);
        void removePoll(int i);

    public:
        void add_client();
        void remove_client(Client *client);
        void send_message(std::string message, Client *client);
        void client_message(int index);
};

#endif // SERVER_HPP
