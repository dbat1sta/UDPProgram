#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>

#define BUFFER_SIZE 1500

unsigned short calculate_checksum(unsigned short *buf, int len) {
    unsigned long sum = 0;
    unsigned short result;

    while (len > 1) {
        sum += *buf++;
        len -= sizeof(unsigned short);
    }

    if (len)
        sum += *(unsigned char *)buf;

    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_ip> <seed>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *server_ip = argv[1];
    unsigned int seed = (unsigned int)atoi(argv[2]);
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int data_size;
    unsigned short source_port, dest_port, length, checksum;

    srand(seed);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(0);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    data_size = (rand() % (100 - 50 + 1)) + 50;
    for (int i = 0; i < data_size; i++) {
        buffer[i] = (char)(rand() % 256);
    }

    source_port = rand() % 65536;
    dest_port = 12345;
    length = sizeof(source_port) + sizeof(dest_port) + sizeof(length) + sizeof(checksum) + data_size;

    unsigned short *header = (unsigned short *)buffer;
    header[0] = source_port;
    header[1] = dest_port;
    header[2] = length;
    header[3] = 0; // checksum placeholder

    checksum = calculate_checksum((unsigned short *)buffer, length);
    header[3] = checksum;

    if (send(sockfd, buffer, length, 0) < 0) {
        perror("send");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Sent packet to %s:%d\n", server_ip, ntohs(server_addr.sin_port));
    printf("Packet size: %d bytes\n", length);

    for (int i = 0; i < length; i++) {
        if (i % 8 == 0 && i != 0)
            printf("\n");
        printf("%02x ", (unsigned char)buffer[i]);
    }
    printf("\n");

    close(sockfd);
    return 0;
}
