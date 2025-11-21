#include <iostream>
#include <vector>
#include <unordered_map>
#include <memory>

#include "network.hpp"

const int BACKLOG = 10;

class ClientTCP
{    
public:
    ClientTCP(int fd, SSL* ssl)
        : fd(fd), ssl(ssl)
    {}

    bool handle()
    {
        char symbol = gameRecv(ssl);
        gameSend(ssl, symbol);
        return true;
    }

private:
    int fd = 0;
    SSL* ssl = nullptr;
};
 
void ServerTCP(int port, std::string crt, std::string key)
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

    SSL_CTX* ctx = startServerTLS(crt.c_str(), key.c_str());

    fd_set fd_list;   
    FD_ZERO(&fd_list);
    FD_SET(socketfd, &fd_list);
    int fd_max = socketfd;

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
    
            SSL* ssl = SSL_new(ctx);
            SSL_set_fd(ssl, new_fd);
            if (SSL_accept(ssl) <= 0) {
                continue;
            }

            clients.emplace(new_fd, ClientTCP(new_fd, ssl));
            FD_SET(new_fd, &fd_list);
            if (new_fd > fd_max) fd_max = new_fd;
        }
    }
}
