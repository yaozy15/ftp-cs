//
// Created by yaozy on 17-10-28.
//
#include<time.h>
#include<stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include"server.h"
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

#define    MAXLINE        1024
#define    LISTENQ        20
#define    SERV_PORT    21

int ownport = 0;
//char origin_dir[80] = "/home/yaozy/CLionProjects/myftp/";
char origin_dir[80] = "/tmp/";
char my_ip[100];
char port_string[2048];
char my_pasv[MAXSIZE];
void get_ip(){

    struct ifaddrs * ifAddrStruct=NULL;
    void * tmpAddrPtr=NULL;
    char addressBuffer[INET_ADDRSTRLEN];

    getifaddrs(&ifAddrStruct);

    while (ifAddrStruct!=NULL)
    {
        if (ifAddrStruct->ifa_addr->sa_family==AF_INET)
        {   // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr = &((struct sockaddr_in *)ifAddrStruct->ifa_addr)->sin_addr;
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            char mask[INET_ADDRSTRLEN];
            void* ptr = &((struct sockaddr_in*) ifAddrStruct->ifa_netmask)->sin_addr;
            inet_ntop(AF_INET, ptr, mask, INET_ADDRSTRLEN);
            if(! strcmp(mask, "255.255.255.0"))
                break;

        }

        ifAddrStruct = ifAddrStruct->ifa_next;
    }
    printf("%s\r\n", addressBuffer);
    strcpy(my_ip, addressBuffer);



}

int create_port(){
    int i_random_port;
    time_t t;
    char psz_port_cmd[256];
    srand((unsigned)time(&t));
    do
    {
        i_random_port = rand() % (65534 - 1025 + 1) + 1025;
        sprintf(psz_port_cmd, "netstat -an | grep :%d > /dev/null", i_random_port);
    }while(!system(psz_port_cmd));
    return i_random_port;
}
int recv_data(int sockfd, char* buf, int bufsize)
{
    size_t num_bytes;
    memset(buf, 0, bufsize);
    //printf("hell\r\n");
    num_bytes = recv(sockfd, buf, bufsize, 0);
    //printf("hell\r\n");
    //printf("%s\r\n", buf);
    if (num_bytes < 0)
        return -1;

    return num_bytes;
}
void adjust_str(char *str_, int n)
{
    int i = 0;
    for (i = 0; i < n; i++)
    {
        if (isspace(str_[i])) str_[i] = 0;
        if (str_[i] == '\n') str_[i] = 0;
    }
}
int trimSpace(char *inbuf, char *outbuf)
{
    char *in = inbuf;
    char *out = outbuf;
    int ret = 0;
    //printf("%s\r\n", in);
    int inLen = strlen(in);
   // printf("%d\r\n", inLen);
    if (!inbuf || !outbuf)
    {
        ret = -1;
        printf("func trimSpace(char *inbuf, char *outbuf) err: ret = %d->function parameter is null.\n", ret);
        return ret;
    }
    else
    {
        ret = 1;
        int i = 0;
        for (i = 0; i < inLen; i++)
        {
            if (in[i] != ' ')
            {
                *out++ = in[i];
            }
        }
        *out++ = '\0';
       // *out++ = '\r';
       // *out = '\n';
    }
    return ret;
}





int send_socket(int socket_control, char* rc){

    if (send(socket_control, rc, strlen(rc), 0) < 0 )
    {
        perror("error sending...\n");
       return -1;
    }
    return 0;

}

int recv_socket(int socket_control, char* rc){

    if (recv(socket_control, rc, sizeof rc, 0) < 0 )
    {
        perror("error sending...\n");
        return -1;
    }
    return 0;

}
int ftserve_new_login(int sock_control)
{
    char buf[MAXSIZE];
    char buf_pass[MAXSIZE];
    char user[MAXSIZE];

    char pass[MAXSIZE];
    memset(user, 0, MAXSIZE);
    memset(pass, 0, MAXSIZE);
    memset(buf, 0, MAXSIZE);
    memset(buf_pass, 0, MAXSIZE);
    while(1){

        //if ( recv_data(sock_control, buf, 15) < 0 )
        if ( read(sock_control, buf, MAXSIZE) < 0 )
        {
            //printf("hell\r\n");
            perror("recv error\n");
            exit(1);
        }
      //  fflush(stdout);
       // int conv = 0;
       // int i = 5;
       // int n = 0;

        char out[100];
        strncpy(out, buf, 14);
       // printf("%d\r\n", (int)strlen(out));
       // printf("%d\r\n", strlen("anonymous\r\n"));
        //printf("%s\r\n", "testqu");
        //printf("%s\r\n", out);
        if(strcmp(out, "USER anonymous") == 0){

            break;
        }
        else
            continue;



    }

    send_socket(sock_control, "331\r\n");
    while(1){

        if ( read(sock_control, buf_pass, MAXSIZE) < 0 )
        {
            perror("recv error\n");
            exit(1);
        }
       // int conv = 0;
    
        printf("%s", buf_pass);
        printf("%s\r\n", "tequi");

        char out[100];
        trimSpace(user, out);
        printf("%s\r\n", "tequi");

        send_socket(sock_control, "230 login in Please send email\r\n");
        break;


    }

    return 1;
}

int compare_string(char *s, char *r){


    int l = (int)strlen(r);
    int i = 0;
    for(i = 0; i < l; i++){
        if(s[i] != r[i]){
            return 0;
        }
       // printf("%c", s[i]);

    }
    return 1;


}
int has = 0;
int ftserve_recv_cmd(int sock_control, char*cmd, char*arg)
{

    char buffer[MAXSIZE];
    memset(buffer, 0, MAXSIZE);
    memset(cmd, 0, MAXSIZE);
    memset(arg, 0, MAXSIZE);
    if ((recv(sock_control, buffer, MAXSIZE, 0) ) == -1)
    {
        perror("recv error\n");
        return -1;
    }
    printf("%s\r\n", buffer);
    strcpy(port_string, buffer);
    int rc = 0;
    if(compare_string(buffer, "MKD") == 1 || compare_string(buffer, "RMD") == 1 || compare_string(buffer, "CWD") == 1){
        strncpy(cmd, buffer, 3);
        cmd[3] = '\0';
        char *tmp = buffer + 4;
        strcpy(arg, tmp);
    }
    else{
        strncpy(cmd, buffer, 4);
        cmd[4] = '\0';
        char *tmp = buffer + 5;
        strcpy(arg, tmp);
    }

   // char *tmp = buffer + 5;
    //strcpy(arg, tmp);
    printf("%s\r\n", cmd);
   // printf("hloo\r\n");
    if (strcmp(cmd, "QUIT")==0 || strcmp(cmd, "ABOR")==0 ){
        rc = 221;
        send_socket(sock_control, "221 goodbye.\r\n");
    }

    else if ((strcmp(cmd, "USER") == 0) || (strcmp(cmd, "PASS") == 0) || (strcmp(cmd, "LIST") == 0) || (strcmp(cmd, "RETR") == 0) || (strcmp(cmd, "STOR") == 0) || (strcmp(cmd, "TYPE") == 0) || (strcmp(cmd, "RMDB") == 0) || (strcmp(cmd, "MKDB") == 0) || (strcmp(cmd, "PORT") == 0) || (strcmp(cmd, "PASV") == 0))
        rc = 200;
    else if(strcmp(cmd, "PASV") == 0){
        printf("heloo\r\n");
        rc = 227;
    }
   // printf("heloo\r\n");
    if(compare_string(cmd, "SYST") == 1){
        rc = 201;
        printf("SYST\r\n");

        send_socket(sock_control, "215 UNIX Type: L8\r\n");
    }
    else if(compare_string(buffer, "TYPE") == 1){
        rc = 202;
        printf("TYPE I11\r\n");
        if(compare_string(buffer, "TYPE I"))
           send_socket(sock_control, "200 Type set to I.\r\n");
        else
            send_socket(sock_control, "503 unknown type.\r\n");
    }
    else if(compare_string(buffer, "PORT") == 1){
        rc = 200;
        printf("PORTS\r\n");

    }
    else if(compare_string(buffer, "PASV") == 1) {
        rc = 200;
        printf("PASV\r\n");
    }
    else if(compare_string(buffer, "RETR") == 1){
        rc = 200;
        printf("RETR\r\n");

    }
    else if(compare_string(buffer, "STOR") == 1){
        rc = 200;
        printf("%s\r\n", buffer);

    }
    else if(compare_string(buffer, "LIST") == 1){
        rc = 200;
        printf("%s\r\n", buffer);

    }
    else if(compare_string(buffer, "MKD") == 1){
        rc = 200;
        printf("%s\r\n", buffer);

    }
    else if(compare_string(buffer, "RMD") == 1){
        rc = 200;
        printf("%s\r\n", buffer);

    }
    else if(compare_string(buffer, "CWD") == 1){
        rc = 200;
        printf("%s\r\n", buffer);

    }
    else
        rc = 502;

    return rc;
}
int socket_connect(int port, char*host)
{
    int sockfd;
    struct sockaddr_in dest_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("error creating socket");
        return -1;
    }
    printf("shh\r\n");
    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr.s_addr = inet_addr(host);

    if(connect(sockfd, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0 )
    {
        perror("error connecting to server");
        return -1;
    }

    return sockfd;
}
int used = 0;
int ftserve_start_data_conn(int sock_control, char* buf)
{

   // int sock_data;
  //  char num[100] = "1113";
   // char aim[100];

    printf("%s\r\n", buf);
    char ip[100];
    char arg[100];

    char buf1[100];
    strncpy(buf1, buf + 5, strlen(buf) - 5);
    printf("%s\r\n", buf1);

    strcpy(arg , strtok (buf1,","));

    strcpy(ip, arg);

    int n = 3;
    while(n){
        strcat(ip, ".");
        strcpy(arg, strtok(NULL, ","));
        printf("%s\r\n", arg);
        strcat(ip, arg);
        n--;

    }
    char high[100];
    char low[100];
    strcpy(high, strtok(NULL, ","));
    strcpy(low, strtok(NULL, ","));
    int newport = atoi(high) * 256 + atoi(low);

    ownport = newport;
    printf("%d\r\n", newport);
    printf("%s\r\n", ip);
    strcpy(my_ip, ip);

    send_socket(sock_control, "200\r\n");


    return 0;
}

int ftserver_open_conn(int sock_con)
{
    int my_port = create_port();
    char iport[100];
   // int highport, lowport;
    char higher[100];
    char lower[100];
    sprintf(iport, "%d", my_port);
    sprintf(higher, "%d", my_port/256);
    sprintf(lower, "%d", my_port%256);
    char arg[100];

    get_ip();
    printf("%s\r\n", my_ip);
    printf("%d\r\n", my_port);
    ownport = my_port;
    printf("%d\r\n", ownport);
    strcpy(arg, my_ip);

    printf("%s\r\n", arg);

    char arg_c[MAXSIZE];
    char ip[MAXSIZE];
    strcpy(arg_c , strtok (arg,"."));
    strcpy(ip, arg_c);

    int n = 3;
    while(n){
        strcat(ip, ",");
        strcpy(arg, strtok(NULL, "."));
        strcat(ip, arg);
        n--;

    }
    char pasv[MAXSIZE];
    strcpy(pasv, "227 Entering Passive Mode(");
    strcat(pasv, ip);
    strcat(pasv, ",");
    strcat(pasv, higher);
    strcat(pasv, ",");
    strcat(pasv, lower);
    strcat(pasv, ")\r\n");
    printf("%s\r\n", pasv);

    int sock_listen = create_socket(my_port);
    send_socket(sock_con, pasv);
    strcpy(my_pasv, pasv);



    return sock_listen;
}


int ftserver_open_already(int sock_con, int port, int pasv)
{
    //int ack = 1;

    int sock_conn = accept_socket(pasv);
    send_socket(sock_con, "150\r\n");
    return sock_conn;
}
int ftserve_start_data_already(int sock_control, int port)
{
  //  int newport;
    char buf[1024];
    int sock_data;

    struct sockaddr_in client_addr;
    socklen_t len = sizeof client_addr;
    getpeername(sock_control, (struct sockaddr*)&client_addr, &len);
    inet_ntop(AF_INET, &client_addr.sin_addr, buf, sizeof(buf));

    if ((sock_data = socket_connect(port, my_ip)) < 0)
        return -1;
    send_socket(sock_control, "150\r\n");
    return sock_data;
}
void ftserve_retr(int sock_control, int sock_data, char* filename)
{


    char data[MAXSIZE];
    memset(data, 0, MAXSIZE);

    size_t num_read;
    char address[MAXSIZE];
    char myfilename[MAXSIZE];

    int i = 0;
    while(i < 12){
        myfilename[i] = filename[i];
        i++;
    }
    for(int i = 0; i < strlen(filename); i++){
        if(filename[i] == '\r' || filename[i] == '\n'){
            filename[i] = '\0';
        }
    }
    myfilename[i] = '\0';
    strcpy(address, origin_dir);
    strcat(address, myfilename);
   // printf("%d\r\n", strlen(myfilename));
    printf("%s\r\n", filename);

    printf("%s\r\n", address);

    FILE* fd = fopen(filename, "rb");
    //FILE* fd = fopen(filename, "rb");
    if (!fd) {
        printf("hell\r\n");
        send_socket(sock_control, "550\r\n");
    }
    else
    {
        printf("hello\r\n");
        while((num_read = fread(data, 1, MAXSIZE, fd)) > 0){
            send(sock_data, data, num_read, 0);
        }
        printf("hello\r\n");
        fclose(fd);


        close(sock_data);

        send_socket(sock_control, "226 222\r\n");

    }
}

void ftserve_store(int sock_control, int sock_data, char* filename)
{
    char data[MAXSIZE];
    memset(data, 0, MAXSIZE);
    int size;
    int i = 0;
    for(i = 0; i < strlen(filename); i++){
        if(filename[i] == '\r' || filename[i] == '\n'){
            filename[i] = '\0';
        }

    }
    printf("%s\r\n", filename);
    FILE* fd = fopen(filename, "wb+");


    while(1){
        size = recv(sock_data, data, MAXSIZE, 0);
        fwrite(data, 1, size, fd);
        if(size < MAXSIZE){
            break;
        }

    }

    printf("%d\n", size);
    close(sock_data);
    fclose(fd);

    send_socket(sock_control, "226 11\r\n");

}
int send_response(int sockfd, int rc)
{
    int conv = htonl(rc);
    if (send(sockfd, &conv, sizeof conv, 0) < 0 )
    {
        perror("error sending...\n");
        return -1;
    }
    return 0;
}
void ftserve_cd(int sock_data, int sock_control, char* filename){
    char now[80];
    getcwd(now, sizeof now);
    printf("%s\r\n", now);
    char temp[80];
    strcpy(temp, now);
    strcat(temp, "/");
    for(int i = 0; i < strlen(filename); i++){

        if(filename[i] == '\r'){
            filename[i] = '\0';
        }
        if(filename[i] == '\n'){
            filename[i] = '\0';
        }

    }
    printf("%s\r\n", temp);
    strcat(temp, filename);
    printf("%s\r\n", temp);
    int rc = chdir(temp);
    printf("%d\r\n", rc);
    if(rc >= 0)
        send_socket(sock_control, "226\r\n");
    else
        send_socket(sock_control, "530\r\n");

}
int ftserve_list(int sock_data, int sock_control, char* filename)
{
    char data[MAXSIZE];
    size_t num_read;
/*    FILE *testd = fopen(filename, "r"); // ���ļ�
    printf("%s\r\n", filename);
    if (!testd)
        send_response(sock_control, 550);
    fclose(testd);*/
    char now[80];
    getcwd(now, sizeof now);
    printf("%s\r\n", now);
    char temp[80];
    strcpy(temp, now);
    strcat(temp, "/");
  //  strcat(temp, filename);
   // char newfilename[MAXSIZE];
    for(int i = 0; i < strlen(filename); i++){

        if(filename[i] == '\r'){
            filename[i] = '\0';
        }
        if(filename[i] == '\n'){
            filename[i] = '\0';
        }

    }
    //newfilename[strlen(filename)] = '\0';
    printf("%s\r\n", temp);
    //strcpy(origin_dir, temp);
    FILE* fd;
    strcat(temp, filename);
    int rl =  chdir(temp);
    if(rl < 0){
        send_socket(sock_control, "530\r\n");
        return 0;

    }
    getcwd(now, sizeof now);
    printf("%s\r\n", now);
    char buf[80];
    strcpy(buf, "ls -l");
    strcat(buf, filename);
    strcat(buf, " | tail -n+2 > tmp.txt");
    printf("%s\n", buf);
    //chdir()

    int rs = system("ls -l | tail -n+2 > tmp.txt");
    //int rs = system(buf);

    if ( rs < 0)
    {
        exit(1);
    }

    fd = fopen("tmp.txt", "r");
    if (!fd)
        exit(1);


    fseek(fd, SEEK_SET, 0);
    printf("%d\r\n", sock_control);
    //send_socket(sock_control, "1\r\n");

    memset(data, 0, MAXSIZE);

    //send_response(sock_control, 1);

    while ((num_read = fread(data, 1, MAXSIZE, fd)) > 0)
    {
        strcat(data, "\0\r\n");
       // printf("%d\r\n", num_read);
        if (send(sock_data, data, num_read, 0) < 0)
            perror("err");

        memset(data, 0, MAXSIZE);
    }

    fclose(fd);
    send_socket(sock_control, "226\r\n");
    //chdir(origin_dir);
    //send_response(sock_control, 226);

    return 0;
}
int ftserve_rm(int sock_data, int sock_control, char* filename)
{
    //size_t num_read;
    char temp[80];
    strcpy(temp, origin_dir);
    strcat(temp, filename);
    printf("%s\r\n", temp);
    for(int i = 0; i < strlen(temp); i++){
        if(temp[i] == '\r' || temp[i] == '\n'){
            temp[i] = '\0';
        }
    }
    for(int i = 0; i < strlen(filename); i++){
        if(filename[i] == '\r' || filename[i] == '\n'){
            filename[i] = '\0';
        }
    }

    FILE *testd = fopen(temp, "r"); // ���ļ�
    if (!testd){
        send_socket(sock_control, "not successfull build");
        return 0;
    }

    fclose(testd);
   // send_response(sock_control, 220);
    char buf[80] = "rmdir ";
    strcat(buf, filename);

    int rs = system(buf);
    printf("%d\n", rs);
    send_socket(sock_control, "250 successfull remove");
    return 0;
}

int ftserve_mkdir(int sock_data, int sock_control, char* filename)
{
    printf("test\r\n");
   // size_t num_read;
    char temp[80];
    strcpy(temp, origin_dir);
    strcat(temp, filename);
    printf("%s\r\n", temp);
    FILE *testd = fopen(temp, "r");
    for(int i = 0; i < strlen(filename); i++){
        if(filename[i] == '\r' || filename[i] == '\n'){
            filename[i] = '\0';
        }
    }

    printf("%s\r\n", filename);
    if (!testd){
        char buf[80] = "mkdir ";
        strcat(buf, filename);

        system(buf);
        send_socket(sock_control, "257successfull build");
        return 0;
    }

    fclose(testd);
    send_socket(sock_control, "successfull build");


    return 0;
}
int read_reply(int sock_control)
{
    int retcode = 0;
    if (recv(sock_control, &retcode, sizeof retcode, 0) < 0)
    {
        perror("client: error reading message from server\n");
        return -1;
    }
    return ntohl(retcode);
}


void print_reply(int rc)
{
    switch (rc)
    {
        case 220:
            printf("220 Welcome, server ready.\r\n");
            break;
        case 221:
            printf("221 Goodbye!\n");
            break;
        case 226:
            printf("226 Closing data connection. Requested file action successful.\r\n");
            break;
        case 550:
            printf("550 Requested action not taken. File unavailable.\r\n");
            break;
    }
}
int pasv = 0;
void ftserve_handle(int sock_control){
    int sock_data = 0;
    int situation = 3;
    char cmd[10];
    char arg[MAXSIZE];

    send_socket(sock_control, "220\r\n");


    if (ftserve_new_login(sock_control) == 1) {

    }
    else
    {
        int conv = htonl(430);
        send(sock_control, &conv, sizeof conv, 0);
        exit(0);
    }
    //printf("hellow\r\n");

    int error_account = 0;
    while (1)
    {
      //  printf("hellow\r\n");
        //printf("sock_control:%d\r\n", sock_control);

        int rc = ftserve_recv_cmd(sock_control, cmd, arg);
        //printf("%s\r\n", cmd);
       //printf("%d\r\n", rc);
        if ((rc < 0) || (rc == 221)){
            //close(sock_control);
            close(sock_data);
            break;
        }
            //break;
        if(rc == 502){
            //send_socket(sock_control, "503 unkown\r\n");
            //close()
            if(error_account > 15){
                break;
            }
            error_account++;
            continue;
        }
        if (rc == 200 || rc == 227)
        {
            //printf("h\r\n");
            if(strcmp(cmd, "PORT") == 0){
               // printf("h1\r\n");
                if ((sock_data = ftserve_start_data_conn(sock_control, port_string)) < 0)
                {
                        close(sock_control);
                        exit(1);
                }
               // printf("h2\r\n");
                used = 1;
                situation = 1;
            }
            else if(strcmp(cmd, "PASV") == 0){
               // printf("ru");
                if(pasv > 0){
                    close(pasv);
                }
                pasv = ftserver_open_conn(sock_control);
                if(pasv < 0)
                {
                    close(sock_control);
                    exit(1);
                }
                used = 1;
                situation = 2;
            }
            else
            {
                printf("%s\r\n", cmd);
                //printf("enter\r\n");
                //if ((sock_data = ftserve_start_data_already(sock_control, ownport)) < 0)
                if (compare_string(cmd, "RMD")==1) {
                    printf("hello\r\n");
                    ftserve_rm(sock_data, sock_control, arg);
                    situation = 3;
                }
                else if (compare_string(cmd, "MKD")==1) {
                    printf("hell11\r\n");
                    ftserve_mkdir(sock_data, sock_control, arg);
                    situation = 3;
                }
                else if (compare_string(cmd, "CWD")==1) {
                    printf("helllcd\r\n");
                    ftserve_cd(sock_data, sock_control, arg);
                    situation = 3;

                }
                printf("%d\r\n", situation);
                if(situation == 1){
                    if ((sock_data = ftserve_start_data_already(sock_control, ownport)) < 0)
                    {
                        close(sock_control);
                        exit(1);
                    }


                }
                else if(situation == 2){

                    printf("enter\r\n");
                    printf("%s\r\n", cmd);
                    if ((sock_data = ftserver_open_already(sock_control, ownport, pasv)) < 0)
                    {
                        close(sock_control);
                        exit(1);
                    }
                    printf("%s\r\n", cmd);
                }
                else if(situation == 0){
                    send_socket(sock_control, "425 no module\r\n");
                    situation = 0;
                    continue;
                }

                printf("shshsj\r\n");
                printf("%s\r\n", cmd);
                if (strcmp(cmd, "LIST")==0) {
                    ftserve_list(sock_data, sock_control, arg);
                    situation = 0;
                }
                else if (strcmp(cmd, "RETR")==0) {
                    printf("h1\r\n");
                    ftserve_retr(sock_control, sock_data, arg);
                    situation = 0;
                    //send_socket(sock_control, '226\r\n');
                }
                else if (compare_string(cmd, "STOR")==1) {
                    printf("hell\r\n");

                    ftserve_store(sock_control, sock_data, arg);
                    situation = 0;
                  //  print_reply(read_reply(sock_control));
                }



            }



            //close(sock_data);
        }
       // close(sock_data);
    }


}
int create_socket(int port)
{
    int sockfd;
    int yes = 1;
    struct sockaddr_in sock_addr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
    {
        perror("socket() error");
        return -1;
    }

    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port);
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        close(sockfd);
        perror("setsockopt() error");
        return -1;
    }

    if (bind(sockfd, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) < 0)
    {
        close(sockfd);
        perror("bind() error");
        return -1;
    }

    if (listen(sockfd, 20) < 0)
    {
        close(sockfd);
        perror("listen() error");
        return -1;
    }
    return sockfd;
}


int accept_socket(int sock_listen)
{
    //printf("shshsj");
    int sockfd;
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    sockfd = accept(sock_listen, (struct sockaddr *) &client_addr, &len); // µÈŽýÁ¬œÓ
    //printf("shshsj");
    if (sockfd < 0)
    {
        perror("accept() error");
        return -1;
    }

    return sockfd;
}

char tmp[MAXSIZE];
int sock_control;
int main(int argc, char *argv[])
{
   // get_ip();
    int my_judge = 0;
    int sock_listen, port, pid;
    port = SERV_PORT;
    int i = 0;
    for(i = 0; i < argc; i++){
        if(strcmp(argv[i], "-root") == 0){
            strcpy(tmp, argv[i+1]);
            my_judge = 1;
        }
        if(strcmp(argv[i], "-port") == 0){
            port = atoi(argv[i+1]);
        }

    }
    printf("%d\r\n", port);
   // port = SERV_PORT;
   // printf("hell\r\n");
    if(my_judge == 1){
        printf("%s\r\n", tmp);
        strcpy(origin_dir, tmp);
    }

    sock_listen = create_socket(port);
    chdir(origin_dir);
   // send_response(sock_listen, 220);
    if(sock_listen < 0)
    {
        perror("Error creating socket");
        exit(1);
    }

    while(1)
    {
        sock_control = accept_socket(sock_listen);
        if(sock_control < 0)
          break;

        if ((pid = fork()) < 0)
            perror("Error forking child process");
        else if(pid == 0)
        {
            //chdir(origin_dir);
            close(sock_listen);
            ftserve_handle(sock_control);
            close(sock_control);
            //chdir(origin_dir);
            exit(0);
            //printf("testg\r\n");
        }

        close(sock_control);
    }



    return 0;
}
