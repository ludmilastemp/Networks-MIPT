#pragma once

#include <cstdint>
#include <iostream>
#include <string>

class ClientConfig {
public:
    std::string type;
    std::string addr;
    uint16_t port;
    std::string serverAddr;
    uint16_t serverPort;

    void print() const;
};

class ServerConfig {
public:
    std::string addr;
    uint16_t port;

    void print() const;
};

bool ClientCommandLineParse(ClientConfig& config, int argc, char* argv[]);
bool ServerCommandLineParse(ServerConfig& config, int argc, char* argv[]);