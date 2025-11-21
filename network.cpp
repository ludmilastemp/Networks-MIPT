#include <netdb.h>
#include <unistd.h>
#include <assert.h>
#include <iostream>
#include <string.h>

#include "network.hpp"

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

ssize_t longSendTLS(SSL* ssl, std::string& buf) {
	buf += '\n';
    size_t sendSize = 0;
    while (sendSize < buf.size()) {
        ssize_t curSize = SSL_write(ssl, buf.data() + sendSize, buf.size() - sendSize);
        if (curSize == -1)
            return -1;
        sendSize += curSize;
    }
    return sendSize;
}

ssize_t longRecvTLS(SSL* ssl, std::string& buf) {
    const int kChunkSize = MAX_BUF_SIZE;
    std::string chunk(kChunkSize, '\0');
    
    int check = SSL_read(ssl, (void*)chunk.data(), chunk.size());
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

void SLLKeyLogCallback(const SSL* sll, const char* line) {
    char* env_p = getenv("SSLKEYLOGFILE");
    if (env_p) {
        FILE* fp = fopen(env_p, "a+");
        if (fp) {
            fprintf(fp, "%s\n", line);
        }
        fclose(fp);
    }
}

SSL_CTX* startServerTLS(const char* crt_path, const char* key_path) {
    if (crt_path == nullptr || key_path == nullptr) {
        return nullptr;
    }

    SSL_CTX* ssl_ctx = SSL_CTX_new(TLS_method());
    if (!ssl_ctx) {
        return nullptr;
    }

    if (SSL_CTX_use_certificate_file(ssl_ctx, crt_path, SSL_FILETYPE_PEM) != 1) {
        return nullptr;
    }
    if (SSL_CTX_use_PrivateKey_file(ssl_ctx, key_path, SSL_FILETYPE_PEM) != 1) {
        return nullptr;
    } 

    if (SSL_CTX_check_private_key(ssl_ctx) != 1) {
        return nullptr;
    }

    SSL_CTX_set_options(ssl_ctx, SSL_OP_ALL|SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3);

    SSL_CTX_set_keylog_callback(ssl_ctx, SLLKeyLogCallback);

    return ssl_ctx;
}

SSL_CTX* startClientTLS() {
    SSL_CTX* ssl_ctx = SSL_CTX_new(TLS_method());
    if (!ssl_ctx) {
        return nullptr;
    }

    SSL_CTX_set_options(ssl_ctx, SSL_OP_ALL|SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3);

    return ssl_ctx;
}

const size_t kMaxMessageLen = 50;

void gameSend(SSL* ssl, char symbol) {
    std::cout << "You: " << symbol;
    char word[kMaxMessageLen] = {};
    size_t len = 0;
    word[0] = symbol;

    while(true) {
        std::cin >> (word + 1);

        len = strlen(word);
        if (len < kMaxMessageLen) break;
    }

    std::string buffer = word;
    if (longSendTLS(ssl, buffer) == -1)
        return;
}

char gameRecv(SSL* ssl) {
    std::string buffer = "";
    int check = longRecvTLS(ssl, buffer);
    if (check == -1)
        return false;
                
    std::cout << "Opponent: " << buffer;
    return buffer[check - 2];
}