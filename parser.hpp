#pragma once

#include <cstdint>
#include <iostream>
#include <string>

class ClientConfig {
public:
    std::string addr;
    uint16_t port;

    void print() const;
};

class ServerConfig {
public:
    std::string addr;
    uint16_t port;
    std::string crt;
    std::string key;

    void print() const;
};

bool ClientCommandLineParse(ClientConfig& config, int argc, char* argv[]);
bool ServerCommandLineParse(ServerConfig& config, int argc, char* argv[]);