all: server sendFile

server: TCPserver.c
	gcc -o server TCPserver.c

sendFile: TCPclient.c
	gcc -o sendFile TCPclient.c

clean:
	rm -f server sendFile
