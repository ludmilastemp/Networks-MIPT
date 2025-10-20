#ifndef NETWORK_H
#define NETWORK_H

#include <string>

ssize_t longSendTCP(int sockfd, std::string& buf);
ssize_t longRecvTCP(int sockfd, std::string& buf);
 
int createSocketTCP();
int  addClientTCP(int socketfd);
void closeSocketTCP(int socketfd);
bool startServerTCP(int server_port, int socketfd, int nClient = 0);
bool startClientTCP(int server_port, int socketfd);

#endif /* NETWORK_H */
