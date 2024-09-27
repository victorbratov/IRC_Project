#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <sys/socket.h>
#include <unistd.h>

struct Mode{
    bool	away;
	bool	invisible;
	bool	wallops;
	bool	restricted;
	bool	op;
	bool	localOp;
	bool	server;
};

class Client{
    private:
        bool isRegistered, isAuth;
        int fd;
        std::string nickname, username;
        std::string realName;
        sockaddr_storage addr;
        socklen_t addrLen;
        Mode mode;

    public:
        Client(int client_fd);
        ~Client();

    public:
        bool IsRegistered();
        bool IsAuth();
        int GetFd();
        std::string GetNickname();
        std::string GetRealName();
        std::string GetHost();
        std::string GetUsername();
        sockaddr_storage GetAddr();
        socklen_t GetAddrLen();
        Mode GetMode();

        void SetNickname(std::string nickname);
        void SetRealName(std::string realName);
        void SetUsername(std::string username);
        void SetMode(Mode mode);
        void SetRegistered(bool isRegistered);
        void SetAuth(bool isAuth);

};

#endif // CLIENT_HPP
