.PHONY: db 

db:
	g++ main.cpp		\
		server_tcp.cpp 	\
		client_tcp.cpp 	\
		network_tcp.cpp \
		server_udp.cpp 	\
		client_udp.cpp 	\
		network_udp.cpp \
		-I. -Wall -Werror -O3 -fno-omit-frame-pointer -o $@.out
