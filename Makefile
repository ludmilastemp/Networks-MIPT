.PHONY: all

all: server client 

server:
	g++ server.cpp parser.cpp -o $@.out

client:
	g++ client.cpp parser.cpp -o $@.out
