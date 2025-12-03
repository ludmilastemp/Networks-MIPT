#include "parser.hpp"

#include <cstring>
#include <vector>
#include <map>

void ClientConfig::print() const {
    std::cout << "Client Configuration:\n"
        << "  Address: " << addr << "\n"
        << "  Port: " << port << "\n"
        << "  Crt: " << crt << "\n";
}

void ServerConfig::print() const {
    std::cout << "Server Configuration:\n"
        << "  Address: " << addr << "\n"
        << "  Port: " << port << "\n"
        << "  Crt: " << crt << "\n"
        << "  Key: " << key << "\n";
}

class ClientCommandLineParser {
private:
    std::map<std::string, std::string> requiredArgs = {
        {"--addr", ""},
        {"--port", ""},
        {"--crt", ""}
    };

public:
    bool parse(ClientConfig& config, int argc, char* argv[]) {
        if (argc == 2) {
            printHelp(argv[0]);
            return false;
        }

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                printHelp(argv[0]);
                return false;
            }
        }

        for (int i = 2; i < argc; i++) {
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
                } else if (arg == "--crt") {
                    config.crt = value;
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
        std::cout << "Usage: " << programName << " client [OPTIONS]\n\n"
            << "Required Options:\n"
            << "  --addr ADDRESS         Local machine IP address\n"
            << "  --port PORT            Local machine port\n"
            << "  --crt  PATH            Path to crt\n"
            << "Example:\n"
            << "  " << programName << " client --addr 1.1.1.1 --port 1234 --crt cert/crt.crt\n"

            << std::endl;
    }
};

class ServerCommandLineParser {
private:
    std::map<std::string, std::string> requiredArgs = {
        {"--addr", ""},
        {"--port", ""},
        {"--crt", ""},
        {"--key", ""}
    };

public:
    bool parse(ServerConfig& config, int argc, char* argv[]) {
        if (argc == 2) {
            printHelp(argv[0]);
            return false;
        }

        for (int i = 2; i < argc; i++) {
            if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
                printHelp(argv[0]);
                return false;
            }
        }

        for (int i = 2; i < argc; i++) {
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
                } else if (arg == "--crt") {
                    config.crt = value;
                } else if (arg == "--key") {
                    config.key = value;
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
        std::cout << "Usage: " << programName << " server [OPTIONS]\n\n"
            << "Required Options:\n"
            << "  --addr ADDRESS         Rendezvous server IP address\n"
            << "  --port PORT            Rendezvous server port\n"
            << "  --crt  PATH            Path to crt\n"
            << "  --key  PATH            Path to key\n"
            << "Example:\n"
            << "  " << programName << " server --addr 1.1.1.1 --port 1234 --crt cert/crt.crt --key cert/key.key\n"
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