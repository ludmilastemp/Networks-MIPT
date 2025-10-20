#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>

#include "network_tcp.hpp"

const int BACKLOG = 10;

class ClientTCP
{
    using HashMap = std::unordered_map<std::string, std::string>;
    
public:
    ClientTCP(int fd,
           HashMap* map)
        : fd(fd),
          map(map)
    {}

    ~ClientTCP()
    {}

    bool handle()
    {
        int check = longRecvTCP(fd, buf);
        if (check == -1) {
            std::cout << "Recv error! Client fd = " << fd << std::endl;
            return false;
        }
        if (check == 0) {
            return false;
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
                if (longSendTCP(fd, value) == -1) {
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
 
void ServerTCP(int port)
{
    int socketfd = createSocketTCP();
    if (socketfd == -1) {
        std::cout << "Failed to create socket!" << std::endl;
        return;
    }

    if (!startServerTCP(port, socketfd, BACKLOG)) {
        std::cout << "Failed to start server!" << std::endl;
        return;
    }

    fd_set fd_list;   
    FD_ZERO(&fd_list);
    FD_SET(socketfd, &fd_list);
    int fd_max = socketfd;

    std::unordered_map<std::string, std::string> map;
    std::unordered_map<int, ClientTCP> clients;

    while (true) {
        fd_set read_fds = fd_list;
        if (select(fd_max + 1, &read_fds, NULL, NULL, NULL) == -1) {
			std::cout << "Failed to select()!" << std::endl;
			return;
		}

        for (auto& [key, client] : clients) {
            if (FD_ISSET(key, &read_fds)) {
                if (!client.handle()) {
                    std::cout << "Client detached! fd = " << key << std::endl;
                    closeSocketTCP(key);
                    FD_CLR(key, &fd_list);
                }
            }
        }

        if (FD_ISSET(socketfd, &read_fds)) {
            int new_fd = addClientTCP(socketfd);
            if (new_fd == -1) {
                std::cout << "Failed to attach Client!" << std::endl;
                continue;
            }
            std::cout << "Client attached! fd = " << new_fd << std::endl;
    
            clients.emplace(new_fd, ClientTCP(new_fd, &map));
            
            FD_SET(new_fd, &fd_list);
            if (new_fd > fd_max) fd_max = new_fd;
        }
    }
}
