#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#define DEFAULT_BUFSIZE 4096

void get_unique_filename(char *filename) {
    int count = 2;
    char new_filename[256];
    char base_name[256];
    char extension[50];

    // Split the base name and extension
    char *dot = strrchr(filename, '.');
    if (dot) {
        strncpy(base_name, filename, dot - filename); 
        base_name[dot - filename] = '\0';
        strcpy(extension, dot);
    } else {
        strcpy(base_name, filename);
        extension[0] = '\0'; 
    }

    // Try new filenames with "(count)" format until an unused one is found
    while (access(filename, F_OK) == 0) {
        snprintf(new_filename, sizeof(new_filename), "%s(%d)%s", base_name, count++, extension);
        strcpy(filename, new_filename);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s port-number bufSize\n", argv[0]);
        exit(1);
    }

    int port = atoi(argv[1]);
    int bufSize = atoi(argv[2]);
    char buffer[bufSize];
    int mysocket, consocket;
    struct sockaddr_in serv, client;
    socklen_t client_size = sizeof(struct sockaddr_in);

    mysocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mysocket < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    serv.sin_port = htons(port);

    if (bind(mysocket, (struct sockaddr *)&serv, sizeof(serv)) < 0) {
        perror("Bind failed");
        close(mysocket);
        exit(1);
    }

    listen(mysocket, 20);
    printf("Server listening on port %d with buffer size %d...\n", port, bufSize);

    while (1) {
        consocket = accept(mysocket, (struct sockaddr *)&client, &client_size);
        if (consocket < 0) {
            perror("Accept failed");
            continue;
        }

        // Receive filename length
        int fileNameLength;
        if (recv(consocket, &fileNameLength, sizeof(fileNameLength), 0) <= 0) {
            perror("Receiving file name length failed");
            close(consocket);
            continue;
        }
        fileNameLength = ntohs(fileNameLength);

        // Receive filename
        char fileName[256];
        if (recv(consocket, fileName, fileNameLength, 0) <= 0) {
            perror("Receiving file name failed");
            close(consocket);
            continue;
        }
        fileName[fileNameLength] = '\0'; // Null-terminate the file name

        // Resolve duplicate file names
        get_unique_filename(fileName);

        // Open the file to write the received data
        FILE *receivedFile = fopen(fileName, "wb");
        if (!receivedFile) {
            perror("File open failed");
            close(consocket);
            continue;
        }

        printf("Receiving file: %s\n", fileName); // Debug message

        // Receive file contents in chunks and write to disk
        int bytesReceived, totalBytes = 0;
        while ((bytesReceived = recv(consocket, buffer, bufSize, 0)) > 0) {
            fwrite(buffer, 1, bytesReceived, receivedFile);
            totalBytes += bytesReceived;
        }


        if (bytesReceived < 0) {
            perror("Error receiving file data");
        } else {
            printf("File received successfully: %s (%d bytes)\n", fileName, totalBytes);
        }

        fclose(receivedFile); 
        close(consocket);      
    }

    close(mysocket); 
    return 0;
}
