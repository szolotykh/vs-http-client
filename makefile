all: client

client: client.o HTTPClient.o
	gcc client.o HTTPClient.o -o client

client.o: client.c client.h
	gcc -c client.c

HTTPClient.o: HTTPClient.c HTTPClient.h
	gcc -c HTTPClient.c

clean:
	rm -rf *o client
