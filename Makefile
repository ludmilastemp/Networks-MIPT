.PHONY: tls 

tls:
	g++ main.cpp		\
		server.cpp 	\
		client.cpp 	\
		network.cpp \
		parser.cpp 		\
		-I. -lssl -lcrypto -Wall -Werror -O3 -fno-omit-frame-pointer -o $@.out
