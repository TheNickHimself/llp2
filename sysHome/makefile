build_all: clean clean client server

client:
	gcc -Wall -o client simple_client.c

server:
	gcc -Wall -o server dummy_http_multi_server.c -lpthread

clean:
	-rm server client

release:
	