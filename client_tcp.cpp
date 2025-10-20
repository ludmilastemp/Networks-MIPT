#include <assert.h>
#include <iostream>
#include <memory>

#include "network_tcp.hpp"

bool TestGetSetGetTCP(int port) {
    int socketfd = createSocketTCP();
    if (socketfd == -1) {
        std::cout << "Failed to create socket!" << std::endl;
        return false;
    }

    if (!startClientTCP(port, socketfd)) {
        std::cout << "Failed to start server!" << std::endl;
        return false;
    }

    std::string buffer;
    int check = 0;

    size_t value_len = 30 * 1024;
    std::string value(value_len, 'X');

    buffer = "get foo";
    if (longSendTCP(socketfd, buffer) == -1)
        return false;

    buffer = "";
    check = longRecvTCP(socketfd, buffer);
    if (check == -1)
        return false;
                
    assert(check != 0);
    assert(buffer == "\n");

    buffer = "set foo " + value;
    if (longSendTCP(socketfd, buffer) == -1)
        return false;

    buffer = "get foo";
    if (longSendTCP(socketfd, buffer) == -1)
        return false;

    buffer = "";
    while (buffer[buffer.size() - 1] != '\n') {
        check = longRecvTCP(socketfd, buffer);
        if (check == -1)
            return false;
    }

    assert(check != 0);
    assert(buffer.size() == value_len + 1);
    assert(buffer == value + '\n');

    closeSocketTCP(socketfd);
    return true;
}

bool TestTwoClientTCP(int port) {    
    
    int socketfd1 = createSocketTCP();
    if (socketfd1 == -1) {
        std::cout << "Failed to create socket!" << std::endl;
        return false;
    }

    if (!startClientTCP(port, socketfd1)) {
        std::cout << "Failed to start server!" << std::endl;
        return false;
    }

    int socketfd2 = createSocketTCP();
    if (socketfd2 == -1) {
        std::cout << "Failed to create socket!" << std::endl;
        return false;
    }

    if (!startClientTCP(port, socketfd2)) {
        std::cout << "Failed to start server!" << std::endl;
        return false;
    }

    std::string buffer;
    int check = 0;

    size_t value_len = 50;
    std::string value(value_len, 'X');
    value[value_len - 1] = '\0';

    buffer = "set foo " + value;
    if (longSendTCP(socketfd1, buffer) == -1)
        return false;

    buffer = "get foo";
    if (longSendTCP(socketfd2, buffer) == -1)
        return false;

    buffer = "";
    while (buffer[buffer.size() - 1] != '\n') {
        check = longRecvTCP(socketfd2, buffer);
        if (check == -1)
            return false;
    }

    assert(check != 0);
    assert(buffer.size() == value_len + 1);
    assert(buffer == value + '\n');

    closeSocketTCP(socketfd1);
    closeSocketTCP(socketfd2);

    return true;
}

void ClientTCP(int port)
{
    bool check = true;
    check &= TestGetSetGetTCP(port);
    check &= TestTwoClientTCP(port);

    if (check) std::cout << "OK\n";
    else std::cout << "ERROR\n";
}
