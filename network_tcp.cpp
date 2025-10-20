#include <netdb.h>
#include <unistd.h>
#include <assert.h>

#include "network_tcp.hpp"

const int MAX_BUF_SIZE = 1024;

ssize_t longSendTCP(int sockfd, std::string& buf) {
	buf += '\n';
    size_t sendSize = 0;
    while (sendSize < buf.size()) {
        ssize_t curSize = send (sockfd, buf.data() + sendSize, buf.size() - sendSize, 0);
        if (curSize == -1)
            return -1;
        sendSize += curSize;
    }
    return sendSize;
}

ssize_t longRecvTCP(int sockfd, std::string& buf) {
    const int kChunkSize = MAX_BUF_SIZE;
    std::string chunk(kChunkSize, '\0');
    
    int check = recv (sockfd, (void*)chunk.data(), chunk.size(), 0);
    if (check == -1)
        return -1;
    
    chunk.resize(check);
    buf += chunk;
    return check;
}

int createSocketTCP() {
    int socketfd = socket (PF_INET, SOCK_STREAM, 0);
    if (socketfd == -1) 
        return -1;
    
    int yes = 1;
    setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

    return socketfd;
}

int addClientTCP(int socketfd) {
    int new_fd = accept(socketfd, (struct sockaddr*)NULL, NULL);
    if (new_fd == -1)
        return -1;
    
    return new_fd;
}

void closeSocketTCP(int socketfd) {
    close(socketfd);
}

bool startServerTCP(int server_port, int socketfd, int backlog) {
    struct in_addr sin_addr;
    sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // struct sockaddr_in {
    //     sa_family_t    sin_family; /* address family: AF_INET */
    //     in_port_t      sin_port;   /* port in network byte order */
    //     struct in_addr sin_addr;   /* internet address */
    // };

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr = sin_addr;
    
    if (bind (socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        return false;

    if (listen(socketfd, backlog) == -1)
        return false;

    return true;
}

bool startClientTCP(int server_port, int socketfd) {
    struct in_addr sin_addr;
    sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // struct sockaddr_in {
    //     sa_family_t    sin_family; /* address family: AF_INET */
    //     in_port_t      sin_port;   /* port in network byte order */
    //     struct in_addr sin_addr;   /* internet address */
    // };

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr = sin_addr;
    
    if (connect (socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        return false;

    return true;
}
