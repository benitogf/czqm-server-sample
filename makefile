server: server.c
	gcc -Wall -g server.c -o server -lczmq -lzmq

client: client.c
	gcc -Wall -g client.c -o client -lczmq -lzmq