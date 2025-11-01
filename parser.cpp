#include "parser.hpp"

#include <cstring>
#include <vector>
#include <map>

void ClientConfig::print() const {
    std::cout << "Client Configuration:\n"
        << "  Type: " << type << "\n"
        << "  Client Address: " << addr << "\n"
        << "  Client Port: " << port << "\n"
        << "  Server Address: " << serverAddr << "\n"
        << "  Server Port: " << serverPort << "\n";
}

void ServerConfig::print() const {
    std::cout << "Server Configuration:\n"
        << "  Server Address: " << addr << "\n"
        << "  Server Port: " << port << "\n";
}

class ClientCommandLineParser {
private:
    std::map<std::string, std::string> requiredArgs = {
        {"--type", ""},
        {"--addr", ""},
        {"--port", ""},
        {"--server-addr", ""},
        {"--server-port", ""}
    };

public:
    bool parse(ClientConfig& config, int argc, char* argv[]) {
        if (argc == 1) {
            printHelp(argv[0]);
            return false;
        }

        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                printHelp(argv[0]);
                return false;
            }
        }

        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            
            if (requiredArgs.find(arg) != requiredArgs.end()) {
                if (i + 1 >= argc) {
                    std::cout << "Missing value for argument: " << arg << "\n";
                    return false;
                }
                
                std::string value = argv[++i];
                
                if (arg == "--type") {
                    config.type = value;
                } else if (arg == "--addr") {
                    config.addr = value;
                } else if (arg == "--port") {
                    config.port = std::stoi(value);
                } else if (arg == "--server-addr") {
                    config.serverAddr = value;
                } else if (arg == "--server-port") {
                    config.serverPort = std::stoi(value);
                }
                
                requiredArgs[arg] = value;
            } else {
                std::cout << "Unknown argument: " << arg << "\n";
                return false;
            }
        }

        bool correct = true;
        for (const auto& [arg, value] : requiredArgs) {
            if (value.empty()) {
                correct = false;
                std::cout << "Missing required argument: " + arg << "\n";
            }
        }
        return correct;
    }

private:
    void printHelp(const char* programName) {
        std::cout << "Usage: " << programName << " [OPTIONS]\n\n"
            << "Required Options:\n"
            << "  --type TYPE            Type of client (A or B)\n"
            << "  --addr ADDRESS         Local machine IP address\n"
            << "  --port PORT            Local machine port\n"
            << "  --server-addr ADDRESS  Rendezvous server IP address\n"
            << "  --server-port PORT     Rendezvous server port\n\n"
            << "Example:\n"
            << "  " << programName << " --type A --addr 10.6.0.1 --port 1111 --server-addr 10.5.0.1 --server-port 1234\n"

            << std::endl;
    }
};

class ServerCommandLineParser {
private:
    std::map<std::string, std::string> requiredArgs = {
        {"--addr", ""},
        {"--port", ""}
    };

public:
    bool parse(ServerConfig& config, int argc, char* argv[]) {
        if (argc == 1) {
            printHelp(argv[0]);
            return false;
        }

        for (int i = 1; i < argc; i++) {
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                printHelp(argv[0]);
                return false;
            }
        }

        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            
            if (requiredArgs.find(arg) != requiredArgs.end()) {
                if (i + 1 >= argc) {
                    std::cout << "Missing value for argument: " << arg << "\n";
                    return false;
                }
                
                std::string value = argv[++i];
                
                if (arg == "--addr") {
                    config.addr = value;
                } else if (arg == "--port") {
                    config.port = std::stoi(value);
                }
                
                requiredArgs[arg] = value;
            } else {
                std::cout << "Unknown argument: " << arg << "\n";
                return false;
            }
        }

        bool correct = true;
        for (const auto& [arg, value] : requiredArgs) {
            if (value.empty()) {
                correct = false;
                std::cout << "Missing required argument: " + arg << "\n";
            }
        }
        return correct;
    }

private:
    void printHelp(const char* programName) {
        std::cout << "Usage: " << programName << " [OPTIONS]\n\n"
            << "Required Options:\n"
            << "  --addr ADDRESS         Rendezvous server IP address\n"
            << "  --port PORT            Rendezvous server port\n"
            << "Example:\n"
            << "  " << programName << " --addr 10.1.0.1 --port 1234\n"
            << std::endl;
    }
};

bool ClientCommandLineParse(ClientConfig& config, int argc, char* argv[]) {
    ClientCommandLineParser parser;

    if (!parser.parse(config, argc, argv)) 
        return false;
    
    config.print();
    return true;
}

bool ServerCommandLineParse(ServerConfig& config, int argc, char* argv[]) {
    ServerCommandLineParser parser;

    if (!parser.parse(config, argc, argv)) 
        return false;
    
    config.print();
    return true;
}