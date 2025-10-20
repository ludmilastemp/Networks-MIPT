#ifndef NETWORK_H
#define NETWORK_H

#include <string>

ssize_t longSendUDP(int sockfd, std::string& buf, struct sockaddr_in& to);
ssize_t longRecvUDP(int sockfd, std::string& buf, struct sockaddr_in& from);
 
int createSocketUDP();
void closeSocketUDP(int socketfd);
bool startServerUDP(int server_port, int socketfd);
bool startClientUDP(int server_port, int socketfd);

#endif /* NETWORK_H */
