#include <cstdint>
#include <netdb.h>
#include <sys/user.h>
#include <unistd.h>
#include <climits>

#include "network_udp.hpp"

const int MAX_BUF_SIZE = INT16_MAX;

ssize_t longSendUDP(int sockfd, std::string& buf, struct sockaddr_in& to) {
	buf += '\n';
    size_t sendSize = 0;
    while (sendSize < buf.size()) {
		ssize_t curSize = sendto(sockfd, buf.data() + sendSize, buf.size() - sendSize, 0,
			   (sockaddr*)&to, sizeof(sockaddr_in));

        if (curSize == -1)
            return -1;
        sendSize += curSize;
    }
    return sendSize;
}

ssize_t longRecvUDP(int sockfd, std::string& buf, struct sockaddr_in& from) {
    const int kChunkSize = MAX_BUF_SIZE;
    
    std::string chunk(kChunkSize, '\0');
    socklen_t sizeof_struct = sizeof(sockaddr_in);
    int check = recvfrom(sockfd, (void*)chunk.data(), chunk.size(), 0,
                (struct sockaddr*)&from, &sizeof_struct);
    if (check == -1)
        return -1;    
        
    chunk.resize(check);
    buf += chunk;

    return buf.size();
}

int createSocketUDP() {
	int socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketfd == -1) 
        return -1;
    
    return socketfd;
}

void closeSocketUDP(int socketfd) {
    close(socketfd);
}

bool startServerUDP(int server_port, int socketfd) {
    struct in_addr sin_addr;
    sin_addr.s_addr = INADDR_ANY;

    // struct sockaddr_in {
    //     sa_family_t    sin_family; /* address family: AF_INET */
    //     in_port_t      sin_port;   /* port in network byte order */
    //     struct in_addr sin_addr;   /* internet address */
    // };

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);
    server_addr.sin_addr = sin_addr;
    
	if (bind(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1)
        return false;

    return true;
}

bool startClientUDP(int server_port, int socketfd) {
    struct in_addr sin_addr;
    sin_addr.s_addr = INADDR_ANY;

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
