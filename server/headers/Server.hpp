#ifndef SERVER_HPP
#define SERVER_HPP

#include "Client.hpp"
#include "Message.hpp"
#include <string>
#include <poll.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <map>
#include <vector>

class Server{
    private:
        int socket_fd;
        struct pollfd *fds;
        int clients_online;
        int max_clients;
        std::string name;
        std::map<int, Client *> clients;
        std::string password;
        std::vector<std::string> nicks;

    public:
        Server(std::string name, int max_clients, std::string port, std::string password);
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
        std::string pass(Message msg, int ind);
        std::string nick(Message msg, int ind);
        std::string user(Message msg, int ind);
};

inline bool isValidNick(std::string nick){
    if (nick.size() > 9) return false;
    for (std::string::size_type i = 0; i < nick.length(); i++){
        if (!isalnum(nick[i])) return false;
        if (nick[i] == '-') return false;
        if (nick[i] == '\r') return false;
    }
    return true;
}

#endif // SERVER_HPP
