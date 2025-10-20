#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>
#include <sys/socket.h>
#include <netdb.h>

#include "network_udp.hpp"

class ClientUPD
{
    using HashMap = std::unordered_map<std::string, std::string>;
    
public:
    ClientUPD(int fd,
           HashMap* map)
        : fd(fd),
          map(map)
    {}

    ~ClientUPD()
    {}

    bool handle()
    {
		struct sockaddr_in client_address;

        buf.clear();
        int check = longRecvUDP(fd, buf, client_address);
        if (check == -1) {
            std::cout << "Recv error! Client fd = " << fd << std::endl;
            return false;
        }
        if (check == 0) {
            return false;
        }

        if (buf[buf.size() - 1] != '\n') {
            std::cout << "Command len > max UDP payload size!" << std::endl;
            buf.clear();
            return true;
        }


        while (true) {
            size_t commandEnd = buf.find('\n');
            if (commandEnd == std::string::npos) {
                return true;
            }

            std::string curCommand = buf.substr(0, commandEnd);
            buf.erase(0, commandEnd + 1);

            size_t firstSpace = curCommand.find(' ');
            if (firstSpace == std::string::npos) {
                std::cout << "Command error! Client fd = " << fd << std::endl;
                continue;
            }

            std::string cmd = curCommand.substr(0, firstSpace);

            if (cmd == "get") {
                std::string key = curCommand.substr(firstSpace + 1, commandEnd - firstSpace - 1);
                std::string value = (*map)[key];
                if (longSendUDP(fd, value, client_address) == -1) {
                    std::cout << "Send error! Client fd = " << fd << std::endl;
                    return false;
                }
            }

            if (cmd == "set") {
                size_t secondSpace = curCommand.find(' ', firstSpace + 1);
                if (secondSpace == std::string::npos) {
                    std::cout << "Command error! Client fd = " << fd << std::endl;
                    continue;
                }

                std::string key = curCommand.substr(firstSpace + 1, secondSpace - firstSpace - 1);
                std::string value = curCommand.substr(secondSpace + 1, commandEnd - secondSpace - 1);
                
                (*map)[key] = value;
            }
        }
        return true;
    }

private:
    int fd = 0;
    std::string buf{};
    HashMap* map;
};

void ServerUDP(int port)
{
    int socketfd = createSocketUDP();
    if (socketfd == -1)
    {
        std::cout << "Failed to create socket!" << std::endl;
        return;
    }

    if (!startServerUDP(port, socketfd))
    {
        std::cout << "Failed to start server!" << std::endl;
        return;
    }

    std::unordered_map<std::string, std::string> map;
    ClientUPD client(socketfd, &map);

    while (true)
    {
        client.handle();
    }
}
