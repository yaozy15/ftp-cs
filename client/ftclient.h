#ifndef FTCLIENT_H
#define FTCLIENT_H

#include <arpa/inet.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define DEBUG				1
#define MAXSIZE 			4096
#define CLIENT_PORT_ID		30020
struct command
{
    char arg[255];
    char code[10];
};

int read_reply();

void print_reply(int rc);

int compare_string(char *s, char *r);

int ftclient_read_command(char* buf, int size, struct command *cstruct);

int ftclient_get(int data_sock, int sock_control, char* arg);

int ftclient_put(int data_sock, int sock_control, char* arg);

int ftclient_open_conn(int sock_con, char *arg);

int ftclient_open_pasv(int sock_con, char *result);

int ftclient_open_already(int sock_con, char *arg, int port);

int ftclient_list(int sock_data, int sock_con);



int ftclient_rm(int sock_data, int sock_con);


int ftclient_start_data_already(int sock_control, int port);


void ftclient_new_login();

int send_socket(int socket_control, char* rc);


int socket_create(int port);

int socket_accept(int sock_listen);

int socket_connect(int port, char *host);

int recv_data(int sockfd, char* buf, int bufsize);

int send_response(int sockfd, int rc);

void trimstr(char *str, int n);

void read_input(char* buffer, int size);

#endif
