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

struct Client{
    Socket local, global;
};

class Rendezvous {
public:
    Rendezvous(
        const char *const serverAddrStr, 
        const uint16_t serverPort
    );

    void waitClientsAndRequest();
    void getIpBRequest();
    void getIpBResponse();
    void sendLocalGlocalIpB();

public:
    int sockfd;
    Client clientA;
    Client clientB;
};

int main(int argc, char* argv[]) {

    ServerConfig config;
    if (!ServerCommandLineParse(config, argc, argv))
        return 0;
    
    Rendezvous server(config.addr.c_str(), config.port);

    // First and Second
    std::cout << "\nFirst and Second Steps\n";
    server.waitClientsAndRequest();
    std::cout << "clientA: global ip = " << server.clientA.global.sin_addr.s_addr << "\n";
    std::cout << "clientA:  local ip = " << server.clientA.local.sin_addr.s_addr << "\n";
    std::cout << "clientB: global ip = " << server.clientB.global.sin_addr.s_addr << "\n";

    // Third
    std::cout << "\nThird Step\n";
    server.getIpBRequest();

    // Fourth
    std::cout << "\nFourth Step\n";
    server.getIpBResponse();
    std::cout << "clientA: global ip = " << server.clientA.global.sin_addr.s_addr << "\n";
    std::cout << "clientA:  local ip = " << server.clientA.local.sin_addr.s_addr << "\n";
    std::cout << "clientB: global ip = " << server.clientB.global.sin_addr.s_addr << "\n";
    std::cout << "clientB:  local ip = " << server.clientB.local.sin_addr.s_addr << "\n";

    // Fifth
    std::cout << "\nFifth Step\n";
    server.sendLocalGlocalIpB();
}

Rendezvous::Rendezvous(const char *const serverAddrStr, const uint16_t serverPort) {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    assert(sockfd != -1);

    in_addr ip_addr;
    assert(inet_pton(AF_INET, serverAddrStr, &ip_addr) > 0);

    sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(serverPort);
    sock_addr.sin_addr = ip_addr;
    
    assert(bind(sockfd, (sockaddr*)&sock_addr, sizeof(sock_addr)) == 0);
}

void Rendezvous::waitClientsAndRequest() {
    Message m;
    sockaddr_in client_addr;
    socklen_t sizeof_struct = sizeof(sockaddr_in);
    bool isFirst = true;
    size_t nMassages = 0;

    while(nMassages != 3) {
        recvfrom(sockfd, (void*)&m, sizeof(m), 0, 
                (struct sockaddr*)&client_addr, &sizeof_struct);

        if (m.type == ConnectToRendezvous) {
            nMassages++;
            Client client;
            client.global = client_addr;
            if (isFirst) { 
                clientA = client;
                isFirst = false;
            }
            else {
                clientB = client;
            }
        }

        if (m.type == RequestHelpFromServer) {
            nMassages++;
            if (client_addr.sin_addr.s_addr == clientB.global.sin_addr.s_addr) {
                std::swap(clientA, clientB);
            }
            assert(client_addr.sin_addr.s_addr == clientA.global.sin_addr.s_addr);
            clientA.local = m.local;
        }
    }
}

void Rendezvous::getIpBRequest() {
    Message m(RequestConnectionFromB);
    m.local = clientA.local;
    m.global = clientA.global;

    ssize_t curSize = sendto(sockfd, &m, sizeof(m), 0,
            (sockaddr*)&clientB.global, sizeof(sockaddr_in));
    assert (curSize != -1);
}

void Rendezvous::getIpBResponse() {
    Message m;
    sockaddr_in client_addr;
    socklen_t sizeof_struct = sizeof(sockaddr_in);

    while(true) {
        recvfrom(sockfd, (void*)&m, sizeof(m), 0,
                (struct sockaddr*)&client_addr, &sizeof_struct);

        if (m.type == GiveLocalAddressB) 
            break;
    }
    clientB.local = m.local;
}

void Rendezvous::sendLocalGlocalIpB() {
    Message m(RespondAWithAddressesB);
    m.global = clientB.global;
    m.local  = clientB.local;

    ssize_t curSize = sendto(sockfd, &m, sizeof(m), 0,
            (sockaddr*)&clientA.global, sizeof(sockaddr_in));
    assert (curSize != -1);
}
