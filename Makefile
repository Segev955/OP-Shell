all: main server

main: main.c
	gcc -o main main.c

server: server.c
	gcc -o server server.c

clean:
	rm -f *.o main server
