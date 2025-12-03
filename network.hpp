#ifndef NETWORK_H
#define NETWORK_H

#include <string>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/types.h>

ssize_t longSendTCP(int sockfd, std::string& buf);
ssize_t longRecvTCP(int sockfd, std::string& buf);
ssize_t longSendTLS(SSL* ssl, std::string& buf);
ssize_t longRecvTLS(SSL* ssl, std::string& buf);
 
int createSocketTCP();
int  addClientTCP(int socketfd);
void closeSocketTCP(int socketfd);
bool startServerTCP(int server_port, int socketfd, int nClient = 0);
bool startClientTCP(int server_port, int socketfd);

SSL_CTX* startServerTLS(const char* crt_path, const char* key_path);
SSL_CTX* startClientTLS(const char* crt_path);

void gameSend(SSL* ssl, char symbol);
char gameRecv(SSL* ssl);

#endif /* NETWORK_H */
