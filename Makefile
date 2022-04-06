all: main server

main: main.o server.o client.o
	gcc -o main main.o client.o
	
server: server.o
	gcc -o server server.o
	
main.o: shell.h main.c 
	gcc -c main.c 
	
server.o: server.c shell.h
	gcc -c server.c 
	
client.o: client.c shell.h
	gcc -c client.c 
	
clean:
	rm -f *.o main server
