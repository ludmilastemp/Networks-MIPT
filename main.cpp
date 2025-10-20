#include <string.h>

void ServerTCP(int port);
void ServerUDP(int port);
void ClientTCP(int port);
void ClientUDP(int port);

int main(int argc, char* argv[]) {
    if (argc != 3) return 0;

    int port = 5001;
    if (strcmp(argv[1], "server") == 0) {
        if (strcmp(argv[2], "tcp") == 0) {
            ServerTCP(port);
        }
        if (strcmp(argv[2], "udp") == 0) {
            ServerUDP(port);
        }
    }
    if (strcmp(argv[1], "client") == 0) {
        if (strcmp(argv[2], "tcp") == 0) {
            ClientTCP(port);
        }
        if (strcmp(argv[2], "udp") == 0) {
            ClientUDP(port);
        }
    }
}