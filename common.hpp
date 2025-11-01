#pragma once
#include <netdb.h>

enum MessageType {
    None,
    ConnectToRendezvous,       // a, b -> s
    RequestHelpFromServer,     // a -> s
    RequestConnectionFromB,    // s -> b
    GiveLocalAddressB,         // b -> s
    RespondAWithAddressesB,    // s -> a
    ConnectToCompanion,        // a, b -> b, a
    RequestConnectToCompanion, // a, b -> b, a
};

typedef sockaddr_in Socket;

struct Message {
    Message(MessageType type_ = None)
        : type(type_) {}

    MessageType type;
    Socket local, global;
};
