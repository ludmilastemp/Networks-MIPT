#include <assert.h>
#include <iostream>
#include <memory>
#include <netdb.h>

#include "network_udp.hpp"

bool TestGetSetGetUDP(int port) {
    struct sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY; 

    int socketfd = createSocketUDP();
    if (socketfd == -1) {
        std::cout << "Failed to create socket!" << std::endl;
        return false;
    }

    if (!startClientUDP(port, socketfd)) {
        std::cout << "Failed to start server!" << std::endl;
        return false;
    }

    std::string buffer;
    int check = 0;

    size_t value_len = 5 * 1024;
    std::string value(value_len, 'X');

    buffer = "get foo";
    if (longSendUDP(socketfd, buffer, server_addr) == -1)
        return false;

    buffer = "";
    check = longRecvUDP(socketfd, buffer, server_addr);
    if (check == -1)
        return false;
                
    assert(check != 0);
    assert(buffer == "\n");

    buffer = "set foo " + value;
    if (longSendUDP(socketfd, buffer, server_addr) == -1)
        return false;

    buffer = "get foo";
    if (longSendUDP(socketfd, buffer, server_addr) == -1)
        return false;

    buffer = "";
    while (buffer[buffer.size() - 1] != '\n') {
        check = longRecvUDP(socketfd, buffer, server_addr);
        if (check == -1)
            return false;
    }

    assert(check != 0);
    assert(buffer.size() == value_len + 1);
    assert(buffer == value + '\n');

    closeSocketUDP(socketfd);
    return true;
}

bool TestTwoClientUDP(int port) {    
    struct sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY; 

    int socketfd1 = createSocketUDP();
    if (socketfd1 == -1) {
        std::cout << "Failed to create socket!" << std::endl;
        return false;
    }

    if (!startClientUDP(port, socketfd1)) {
        std::cout << "Failed to start server!" << std::endl;
        return false;
    }

    int socketfd2 = createSocketUDP();
    if (socketfd2 == -1) {
        std::cout << "Failed to create socket!" << std::endl;
        return false;
    }

    if (!startClientUDP(port, socketfd2)) {
        std::cout << "Failed to start server!" << std::endl;
        return false;
    }

    std::string buffer;
    int check = 0;

    size_t value_len = 50;
    std::string value(value_len, 'X');
    value[value_len - 1] = '\0';

    buffer = "set foo " + value;
    if (longSendUDP(socketfd1, buffer, server_addr) == -1)
        return false;

    buffer = "get foo";
    if (longSendUDP(socketfd2, buffer, server_addr) == -1)
        return false;

    buffer = "";
    while (buffer[buffer.size() - 1] != '\n') {
        check = longRecvUDP(socketfd2, buffer, server_addr);
        if (check == -1)
            return false;
    }

    assert(check != 0);
    assert(buffer.size() == value_len + 1);
    assert(buffer == value + '\n');

    closeSocketUDP(socketfd1);
    closeSocketUDP(socketfd2);

    return true;
}

void ClientUDP(int port)
{
    bool check = true;
    check &= TestGetSetGetUDP(port);
    check &= TestTwoClientUDP(port);

    if (check) std::cout << "OK\n";
    else std::cout << "ERROR\n";
}
