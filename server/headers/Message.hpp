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
};

#endif // MESSAGE_HPP
