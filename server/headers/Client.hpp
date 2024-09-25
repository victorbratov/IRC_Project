#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <sys/_types/_socklen_t.h>
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
        bool isRegistered;
        int fd;
        std::string nickname;
        std::string realName;
        sockaddr_storage addr;
        socklen_t addrLen;
        Mode mode;

    public:
        Client(int client_fd);
        ~Client();

    public:
        bool IsRegistered();
        int GetFd();
        std::string GetNickname();
        std::string GetRealName();
        std::string GetHost();
        sockaddr_storage GetAddr();
        socklen_t GetAddrLen();
        Mode GetMode();

        void SetNickname(std::string nickname);
        void SetRealName(std::string realName);
        void SetMode(Mode mode);
        void SetRegistered(bool isRegistered);

};

#endif // CLIENT_HPP
