#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define DEFAULT_BUFSIZE 4096

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s fileName IP-address:port-number [bufSize]\n", argv[0]);
        exit(1);
    }

    char *fileName = argv[1];
    char *ipAndPort = argv[2];
    int bufSize = (argc == 4) ? atoi(argv[3]) : DEFAULT_BUFSIZE;
    char buffer[bufSize];
    char ipAddress[INET_ADDRSTRLEN];
    int port;

    char *colonPos = strchr(ipAndPort, ':');
    if (!colonPos) {
        fprintf(stderr, "Invalid IP-address:port-number format\n");
        exit(1);
    }
    strncpy(ipAddress, ipAndPort, colonPos - ipAndPort);
    ipAddress[colonPos - ipAndPort] = '\0';
    port = atoi(colonPos + 1);

    FILE *file = fopen(fileName, "rb");
    if (!file) {
        perror("File open failed");
        exit(1);
    }

    int mysocket;
    struct sockaddr_in dest;

    memset(&dest, 0, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(port);

    if (inet_pton(AF_INET, ipAddress, &dest.sin_addr) <= 0) {
        perror("Invalid IP address");
        fclose(file);
        exit(1);
    }

    mysocket = socket(AF_INET, SOCK_STREAM, 0);
    if (mysocket < 0) {
        perror("Socket creation failed");
        fclose(file);
        exit(1);
    }

    if (connect(mysocket, (struct sockaddr *)&dest, sizeof(dest)) < 0) {
        perror("Connection failed");
        close(mysocket);
        fclose(file);
        exit(1);
    }

    int fileNameLength = htons(strlen(fileName));
    send(mysocket, &fileNameLength, sizeof(fileNameLength), 0);
    send(mysocket, fileName, strlen(fileName), 0);

    size_t bytesRead;
    while ((bytesRead = fread(buffer, 1, bufSize, file)) > 0) {
        send(mysocket, buffer, bytesRead, 0);
    }

    printf("File '%s' sent successfully.\n", fileName);

    close(mysocket);
    fclose(file);
    return 0;
}
