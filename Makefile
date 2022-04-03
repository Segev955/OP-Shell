all: main server

main: main.o server.o client.o
	gcc main.o client.o -o main
	
server: server.o
	gcc server.o -o server
	
main.o: shell.h main.c 
	gcc -c main.c 
	
server.o: server.c shell.h
	gcc -c server.c 
	
client.o: client.c shell.h
	gcc -c client.c 
	
clean:
	rm -f *.o main server
