#include <arpa/inet.h>
#include <cassert>
#include <climits>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <iostream>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/user.h>
#include <unistd.h>
#include <vector>

#include "common.hpp"
#include "parser.hpp"

const size_t kMaxMessageLen = 1024;
const long int kTimeoutNormal = 100;
const long int kTimeoutCheck  = 1;

enum ClientType {
    A, B,
};

class Client {
public:
    Client(
        ClientType type,
        const char *const clientAddrStr, 
        const uint16_t clientPort,
        const char *const serverAddrStr, 
        const uint16_t serverPort
    );

    void connectToRendezvous();
    void requestHelpFromServer();
    void requestConnectionFromB();
    void giveLocalAddressB();
    void respondAWithAddressesB();
    void connectToCompanion();
    void gameA();
    void gameB();

private:
    void sendMessage(Message m, Socket& socket, bool check = true);
    MessageType recvMessage(Message& m);
    void setTimeout(time_t sec);
    bool isConnectionEstablished(Message& m, Socket& addr);
    void gameSend(char symbol);
    char gameRecv();

public:
    ClientType type;
    int sockfd;
    Socket addr;
    Socket rendezvous;
    Socket companion;
    Socket companionGlobal; // tmp
    Socket companionLocal;  // tmp
};

int main(int argc, char* argv[]) {

    ClientConfig config;
    if (!ClientCommandLineParse(config, argc, argv))
        return 0;

    if (config.type == "A") {
        Client client(A, config.addr.c_str(), config.port, config.serverAddr.c_str(), config.serverPort);
        std::cout << "clientA: local ip = " << client.addr.sin_addr.s_addr << "\n";

        // First
        std::cout << "\nFirst Step\n";
        client.connectToRendezvous();

        // Second
        std::cout << "\nSecond Step\n";
        client.requestHelpFromServer(); 

        // Third
        std::cout << "\nThird Step\n";

        // Fourth
        std::cout << "\nFourth Step\n";

        // Fifth
        std::cout << "\nFifth Step\n";
        client.respondAWithAddressesB(); 
        std::cout << "companion: global ip = " << client.companionGlobal.sin_addr.s_addr << "\n";
        std::cout << "companion:  local ip = " << client.companionLocal.sin_addr.s_addr << "\n";

        // Sixth
        std::cout << "\nSixth Step\n";
        client.connectToCompanion();
        assert(client.companion.sin_addr.s_addr != 0);
        std::cout << "companion: ip = " << client.companion.sin_addr.s_addr << "\n";

        // Seventh
        std::cout << "\nSeventh Step\n";
        client.gameA();
    }

    if (config.type == "B") {
        Client client(B, config.addr.c_str(), config.port, config.serverAddr.c_str(), config.serverPort);
        std::cout << "clientA: local ip = " << client.addr.sin_addr.s_addr << "\n";

        // First
        std::cout << "\nFirst Step\n";
        client.connectToRendezvous();

        // Second
        std::cout << "\nSecond Step\n";

        // Third
        std::cout << "\nThird Step\n";
        client.requestConnectionFromB();

        // Fourth
        std::cout << "\nFourth Step\n";
        client.giveLocalAddressB();
        std::cout << "companion: global ip = " << client.companionGlobal.sin_addr.s_addr << "\n";
        std::cout << "companion:  local ip = " << client.companionLocal.sin_addr.s_addr << "\n";

        // Fifth
        std::cout << "\nFifth Step\n";

        // Sixth
        std::cout << "\nSixth Step\n";
        client.connectToCompanion();
        assert(client.companion.sin_addr.s_addr != 0);        
        std::cout << "companion: ip = " << client.companion.sin_addr.s_addr << "\n";

        // Seventh
        std::cout << "\nSeventh Step\n";
        client.gameB();
    }
}

Client::Client(ClientType type_, const char *const clientAddrStr, const uint16_t clientPort, const char *const serverAddrStr, const uint16_t serverPort) 
    : type(type_) {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sockfd != -1);

    in_addr ip_addr;
    assert(inet_pton(AF_INET, clientAddrStr, &ip_addr) > 0);

    addr.sin_family = AF_INET;
    addr.sin_port = clientPort;
    addr.sin_addr = ip_addr;

    assert(bind(sockfd, (sockaddr*)&addr, sizeof(addr)) == 0);

    in_addr_t rendezvous_ip_addr;
    assert(inet_pton(AF_INET, serverAddrStr, &rendezvous_ip_addr) > 0);

    rendezvous.sin_family      = AF_INET;
    rendezvous.sin_port        = htons(serverPort);
    rendezvous.sin_addr.s_addr = rendezvous_ip_addr;
}

void Client::connectToRendezvous() {
    Message m(ConnectToRendezvous);
    sendMessage(m, rendezvous);
}

void Client::requestHelpFromServer() {
    Message m(RequestHelpFromServer);
    m.local = addr;
    sendMessage(m, rendezvous);
}

void Client::requestConnectionFromB() {
    Message m;
    while(recvMessage(m) != RequestConnectionFromB) {}
    companionGlobal = m.global;
    companionLocal  = m.local;
}

void Client::giveLocalAddressB() {
    Message m(GiveLocalAddressB);
    m.local = addr;
    sendMessage(m, rendezvous);
}

void Client::respondAWithAddressesB() {
    Message m;
    while(recvMessage(m) != RespondAWithAddressesB) {}
    companionGlobal = m.global;
    companionLocal  = m.local;
}

void Client::connectToCompanion() {
    setTimeout(kTimeoutCheck);
    Message m;
    while(true) {
        if (isConnectionEstablished(m, companionLocal)) break;
        if (isConnectionEstablished(m, companionGlobal)) break;
    }

    Message r(RequestConnectToCompanion);
    sendMessage(r, companion, false);

    if (m.type != RequestConnectToCompanion) {
        recvMessage(m);
    }
    assert (m.type == RequestConnectToCompanion);
}

void Client::sendMessage(Message m, Socket& socket, bool check) {
    ssize_t curSize = sendto(sockfd, &m, sizeof(m), 0,
            (sockaddr*)&socket, sizeof(sockaddr_in));
    if (check) assert (curSize != -1);
}

MessageType Client::recvMessage(Message& m) {
    sockaddr_in client_addr;
    socklen_t sizeof_struct = sizeof(sockaddr_in);

    recvfrom(sockfd, (void*)&m, sizeof(m), 0, 
            (struct sockaddr*)&client_addr, &sizeof_struct);
    return m.type;
}

void Client::setTimeout(time_t sec) {
    const timeval timeout = {
        .tv_sec  = sec,
        .tv_usec = 0
    };
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

bool Client::isConnectionEstablished(Message& m, Socket& addr) {
    m.type = ConnectToCompanion;
    sendMessage(m, addr, false);

    Message m2;
    m.type = recvMessage(m2);

    if (m.type == ConnectToCompanion || m.type == RequestConnectToCompanion) {
        setTimeout(kTimeoutNormal);
        companion = addr;
        return true;
    }
    return false;
}

void Client::gameSend(char symbol) {
    std::cout << "You: " << symbol;
    char word[kMaxMessageLen] = {};
    size_t len = 0;
    word[0] = symbol;

    while(true) {
        std::cin >> (word + 1);

        len = strlen(word);
        if (len < kMaxMessageLen) break;
    }

    ssize_t curSize = sendto(sockfd, word, len, 0,
            (sockaddr*)&companion, sizeof(sockaddr_in));
    assert (curSize != -1);
}

char Client::gameRecv() {
    char word[kMaxMessageLen] = {};

    sockaddr_in client_addr;
    socklen_t sizeof_struct = sizeof(sockaddr_in);
    size_t len = recvfrom(sockfd, (void*)&word, kMaxMessageLen, 0, 
            (struct sockaddr*)&client_addr, &sizeof_struct);
    
    std::cout << "Opponent: " << word << "\n";
    return word[len - 1];
}

void Client::gameA() {
    char symbol = 'a';
    while(true) {
        gameSend(symbol);
        symbol = gameRecv();
    }
}

void Client::gameB() {
    char symbol = 'a';
    while(true) {
        symbol = gameRecv();
        gameSend(symbol);
    }
}
