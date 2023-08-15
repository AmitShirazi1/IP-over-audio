#ifndef NETWORKING_H
#define NETWORKING_H


#define BUFFER_SIZE 1024
#define PORT 54321
#define ERROR 1
#define SUCCESS 0

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>


// UDP:
#include <unistd.h>
#include <sys/types.h>

int recvFile(char* buf, int s);
int start_udp_server();

int readFile(FILE* fp, char* buf, int s);
int start_udp_client(char audio_file[], char ip_address[]);


// TCP:
void receive_message(int server_socket);
void send_response(int server_socket);
int start_tcp_server();
void shutdown_connection(int server_socket);
#define RECEIVED_VALIDATION "The server has received your message.\n"

void send_file(FILE *file, int client_socket);
void receive_server_response(int client_socket);
int start_tcp_client(char text_file[], char ip_address[]);
void shutdown_connection(int client_socket);

#endif