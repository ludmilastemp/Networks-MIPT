#include <assert.h>
#include <iostream>
#include <memory>
#include <cassert>

#include "network.hpp"

void ClientTCP(int port)
{
    int socketfd = createSocketTCP();
    if (socketfd == -1) {
        std::cout << "Failed to create socket!" << std::endl;
        return;
    }

    if (!startClientTCP(port, socketfd)) {
        std::cout << "Failed to start server!" << std::endl;
        return;
    }

    SSL_CTX* ctx = startClientTLS();
    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, socketfd);
    int check = SSL_connect(ssl);
    assert(check > 0);

    char symbol = 'a';
    while(true) {
        gameSend(ssl, symbol);
        symbol = gameRecv(ssl);
    }

    SSL_free(ssl);
    SSL_CTX_free(ctx);
}
