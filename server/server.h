//
// Created by yaozy on 17-10-28.
//

#ifndef SERVER_H
#define SERVER_H

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
void ftserve_cd(int sock_data, int sock_control, char* filename);
void ftserve_handle(int sock_control);
int recv_data(int sockfd, char*buf, int size);
int ftserve_new_login(int sock_control);
int create_socket(int port);
int accept_socket(int socket_listen);
int ftserve_recv_cmd(int sock_control, char*cmd, char*arg);
void adjust_str(char *str, int n);
int ftserve_start_data_conn(int sock_control, char* buf);
int ftserve_start_data_already(int sock_control, int port);
int socket_connect(int port, char*host);
void ftserve_store(int sock_control, int sock_data, char* filename);
void ftserve_retr(int sock_control, int sock_data, char* filename);
int send_response(int sockfd, int rc);
int ftserve_list(int sock_data, int sock_control, char* filename);
int ftserve_rm(int sock_data, int sock_control, char* filename);
int ftserve_mkdir(int sock_data, int sock_control, char* filename);
int read_reply(int sock_control);
int ftserver_open_conn(int sock_con);
void get_ip();
void print_reply(int rc);
int create_port();
int ftserver_open_already(int sock_con, int port, int pasv);
int trimSpace(char *inbuf, char *outbuf);
int send_socket(int socket_control, char * rc);
int recv_socket(int socket_control, char * rc);
int compare_string(char *s, char *r);
#endif //MYFTP_SERVER_H
