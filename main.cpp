#include <string.h>
#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/types.h>
#include <iostream>
#include "parser.hpp"

void ServerTCP(int port, std::string crt, std::string key);
void ClientTCP(int port);

int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "Usage: " << argv[0] << " server or client\n\n";
        return 0;
    }

    OPENSSL_init_ssl(0, nullptr);
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ERR_load_crypto_strings();

    if (strcmp(argv[1], "server") == 0) {
        ServerConfig config;
        if (!ServerCommandLineParse(config, argc, argv))
            return 0;

        ServerTCP(config.port, config.crt, config.key);
    }
    else if (strcmp(argv[1], "client") == 0) {
        ClientConfig config;
        if (!ClientCommandLineParse(config, argc, argv))
            return 0;

        ClientTCP(config.port);
    }
    else {
        std::cout << "Usage: " << argv[0] << " server or client\n\n";
        return 0;
    }
}