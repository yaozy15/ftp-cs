#include "ftclient.h"


int sock_control; 
int ownport = 0;
char aim_ip[100];
int read_reply()
{
	int retcode = 0;
	if (recv(sock_control, &retcode, sizeof retcode, 0) < 0) 
	{
		perror("client: error reading message from server\n");
		return -1;
	}	
	return ntohl(retcode);
}
int socket_create(int port)
{
	int sockfd;
	int yes = 1;
	struct sockaddr_in sock_addr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
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

	if (listen(sockfd, 5) < 0)
	{
		close(sockfd);
		perror("listen() error");
		return -1;
	}
	return sockfd;
}

int socket_accept(int sock_listen)
{
	int sockfd;
	struct sockaddr_in client_addr;
	socklen_t len = sizeof(client_addr);
	sockfd = accept(sock_listen, (struct sockaddr *) &client_addr, &len);

	if (sockfd < 0)
	{
		perror("accept() error");
		return -1;
	}
	return sockfd;
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


int recv_data(int sockfd, char* buf, int bufsize)
{
	size_t num_bytes;
	memset(buf, 0, bufsize);

	num_bytes = recv(sockfd, buf, bufsize, 0);
	if (num_bytes < 0)
		return -1;

	return num_bytes;
}

void trimstr(char *str, int n)
{
	int i;
	for (i = 0; i < n; i++)
	{
		if (isspace(str[i])) str[i] = 0;
		if (str[i] == '\n') str[i] = 0;
	}
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


void read_input(char* buffer, int size)
{
	char *nl = NULL;
	memset(buffer, 0, size);

	if ( fgets(buffer, size, stdin) != NULL )
	{
		nl = strchr(buffer, '\n');
		if (nl)
			*nl = '\0';
	}
}


int compare_string(char *s, char *r){


	int l = strlen(r);
	int i = 0;
	for(i = 0; i < l; i++){
		if(s[i] != r[i]){
			return 0;
		}

	}
	return 1;


}

void print_reply(int rc) 
{
	switch (rc)
	{
		case 220:
			printf("220 Welcome, server ready.\n");
			break;
		case 221:
			printf("221 Goodbye!\n");
			break;
		case 226:
			printf("226 Closing data connection. Requested file action successful.\n");
			break;
		case 550:
			printf("550 Requested action not taken. File unavailable.\n");
			break;
	}
}

int mytype = 0;
int ftclient_read_command(char* buf, int size, struct command *cstruct)
{
	memset(cstruct->code, 0, sizeof(cstruct->code));
	memset(cstruct->arg, 0, sizeof(cstruct->arg));
	
	printf("ftclient> ");	
	fflush(stdout);
	fflush(stdin);
	read_input(buf, size); 
	char *arg = NULL;
	arg = strtok (buf," ");
	arg = strtok (NULL, " ");

	if (arg != NULL)
		strncpy(cstruct->arg, arg, strlen(arg));

	if (strcmp(buf, "LIST") == 0) {
		strcpy(cstruct->code, "LIST");
	}
	else if (strcmp(buf, "RETR") == 0)
		strcpy(cstruct->code, "RETR");
	else if (strcmp(buf, "RMD") == 0)
		strcpy(cstruct->code, "RMD");
	else if (strcmp(buf, "MKD") == 0)
		strcpy(cstruct->code, "MKD");
	else if (strcmp(buf, "SYST") == 0)
		strcpy(cstruct->code, "SYST");
	else if (strcmp(buf, "TYPE") == 0)
		strcpy(cstruct->code, "TYPE");
	else if (strcmp(buf, "QUIT") == 0) 
		strcpy(cstruct->code, "QUIT");
	else if (strcmp(buf, "ABOR") == 0)
		strcpy(cstruct->code, "ABOR");
	else if (strcmp(buf, "STOR") == 0)
		strcpy(cstruct->code, "STOR");
	else if (strcmp(buf, "PASV") == 0)
		strcpy(cstruct->code, "PASV");
	else if (strcmp(buf, "PORT") == 0)
		strcpy(cstruct->code, "PORT");
	else if (strcmp(buf, "CWD") == 0)
		strcpy(cstruct->code, "CWD");
	else 
		return -1; 

	memset(buf, 0, 400);
	strcpy(buf, cstruct->code);  


	if (arg != NULL) 
	{
		strcat(buf, " ");
		strncat(buf, cstruct->arg, strlen(cstruct->arg));
	}
	strcat(buf, "\r\n");
	//printf("commands\n");
	send_socket(sock_control, buf);
	return 0;
}


int ftclient_get(int data_sock, int sock_control, char* arg)
{
    char data[MAXSIZE + 2];
    int size;
    for(int i = 0; i < strlen(arg); i++){
        if(arg[i] == '\r' || arg[i] == '\n'){
            arg[i] = '\0';
        }

    }
    FILE* fd = fopen(arg, "w+");
	char name[MAXSIZE];
    while(size = recv(data_sock, data, MAXSIZE, 0)){
		fwrite(data, 1, size, fd);
	//	if(size < MAXSIZE){
	//		break;
	//	}

	}

    if (size < 0) 
		perror("error\n");
    fclose(fd);
	close(data_sock);
    return 0;
}
int ftclient_put(int sock_data, int sock_control, char* filename){

    FILE* fd = NULL;
    char data[MAXSIZE];
    size_t num_read;
	for(int i = 0; i < strlen(filename); i++){
		if(filename[i] == '\r' || filename[i] == '\n'){
			filename[i] = '\0';
		}

	}
    fd = fopen(filename, "r");

    if (!fd){
		send_socket(sock_control, "550\r\n");
	}
    else
    {
        do
        {
            num_read = fread(data, 1, MAXSIZE, fd);
            if (num_read < 0)
                printf("error in fread()\n");

            if (send(sock_data, data, num_read, 0) < 0)
                perror("error sending file\n");

        }
        while (num_read > 0);
        fclose(fd);
		close(sock_data);
    }
    return 0;

}

int used = 0;
int client_id = 0;
int ftclient_open_conn(int sock_con, char *buf)
{

	char ip[100];
	char arg[100];
	strcpy(arg , strtok (buf,","));

	strcpy(ip, arg);


	int n = 3;
	while(n){
		strcat(ip, ".");
		strcpy(arg, strtok(NULL, ","));
		strcat(ip, arg);
		n--;
	}
	char high[100];
	char low[100];
	strcpy(high, strtok(NULL, ","));
	strcpy(low, strtok(NULL, ","));
	int port = atoi(high) * 256 + atoi(low);
	ownport = port;

	strcpy(aim_ip, ip);


	return 1;
}

int ftclient_open_already(int sock_con, char *buf, int port)
{
	int sock_listen = socket_create(port);
	int sock_conn = socket_accept(sock_listen);
	//close(sock_listen);
	return sock_conn;
}

int ftclient_list(int sock_data, int sock_con)
{
	size_t num_recvd;			
	char buf[10 * MAXSIZE];
	char buf_[MAXSIZE];
	int tmp;

	memset(buf, 0, sizeof(buf));
	int size = 0;
	while(size = recv(sock_data, buf, MAXSIZE, 0)){
		printf("%s", buf);
		if(size < MAXSIZE){
			break;
		}
	}
	close(sock_data);
	if (num_recvd < 0) 
		perror("error");

	return 0;
}

int ftclient_rm(int sock_data, int sock_con)
{

	int tmp = 0;
	if (recv(sock_con, &tmp, sizeof tmp, 0) < 0) 
	{
		perror("client: error reading message from server\n");
		return -1;
	}
        tmp = htonl(tmp);
	if(tmp == 550){
          printf("550 error file operation\r\n");
          return 0;
	}
	
	printf("220 successful to remove file\r\n");
	return 0;
}



int ftclient_send_cmd(struct command *cmd)
{
	char buffer[MAXSIZE];
	int rc;

	sprintf(buffer, "%s %s", cmd->code, cmd->arg);
	
	rc = send(sock_control, buffer, (int)strlen(buffer), 0);	
	if (rc < 0) 
	{
		perror("Error sending command to server");
		return -1;
	}
	
	return 0;
}

int send_socket(int socket_control, char* rc){

	if (send(socket_control, rc, strlen(rc), 0) < 0 )
	{
		perror("error sending...\n");
		return -1;
	}
	return 0;

}

void ftclient_new_login()
{
	struct command cmd;
	char user[256];
	memset(user, 0, 256);


	while(1){
		printf("Name: ");
		fflush(stdout);
		read_input(user, 256);
		strcat(user, "\r\n");
		send_socket(sock_control, user);
        char status[MAXSIZE];
		recv_data(sock_control, status, MAXSIZE);
		if(compare_string(status, "331")){
			printf("331 anonymous login, please send a email-address\r\n");
			break;
		}
		else{
			printf("530 not anonymous login\r\n");
			continue;

		}

	}
	while(1){
		printf("Passname:");
		char pass[MAXSIZE];
		read_input(pass, 256);
		strcat(pass, "\r\n");
		send_socket(sock_control, pass);
		char status[MAXSIZE];
		recv_data(sock_control, status, MAXSIZE);
		if(compare_string(status, "230")){

			printf("230 login in successfully\r\n");
			break;

		}
		printf("230 login in successfully\r\n");
		break;


	}

}
int ftclient_open_pasv(int sock_con, char *my_pasv)
{

    char buf[MAXSIZE];
    int wait;
	int sock_data = 0;

    char ip[100];
    char myport[100];
	strcpy(buf, my_pasv);
	char temp[MAXSIZE];
	char my_aim[MAXSIZE];
	strcpy(temp, my_pasv);
	strcpy(temp, strtok(buf, "("));
	strcpy(temp, strtok(NULL, "("));
	strcpy(my_aim, strtok(temp, ")"));


	char arg[MAXSIZE];
	strcpy(arg , strtok (my_aim,","));

	strcpy(ip, arg);

	int n = 3;
	while(n){
		strcat(ip, ".");
		strcpy(arg, strtok(NULL, ","));
		strcat(ip, arg);
		n--;

	}
	char high[100];
	char low[100];
	strcpy(high, strtok(NULL, ","));
	strcpy(low, strtok(NULL, ","));
	int newport = atoi(high) * 256 + atoi(low);

    strcpy(aim_ip, ip);

    ownport = newport;
    return 1;


}
int ftclient_start_data_already(int sock_control, int port)
{
    int newport;
    char buf[1024];
    int wait, sock_data;

	struct sockaddr_in client_addr;
	socklen_t len = sizeof client_addr;
	getpeername(sock_control, (struct sockaddr*)&client_addr, &len);
	inet_ntop(AF_INET, &client_addr.sin_addr, buf, sizeof(buf));
	if ((sock_data = socket_connect(port, aim_ip)) < 0)
		return -1;
    return sock_data;
}

int main(int argc, char* argv[]) 
{		
	int situation = 0;
	int data_sock = -111;
	int	retcode, s;
	char buffer[MAXSIZE];
	struct command cmd;	
	struct addrinfo hints, *res, *rp;

	if (argc != 3)
	{
		printf("usage: ./ftclient hostname port\n");
		exit(0);
	}

	char *host = argv[1]; 
	char *port = argv[2]; 

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	s = getaddrinfo(host, port, &hints, &res);
	if (s != 0) 
	{
		printf("getaddrinfo() error %s", gai_strerror(s));
		exit(1);
	}

	for (rp = res; rp != NULL; rp = rp->ai_next) 
	{
		sock_control = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol); 

		if (sock_control < 0)
			continue;

		if(connect(sock_control, res->ai_addr, res->ai_addrlen)==0)   
			break;
		
		else 
		{
			perror("connecting stream socket");
			exit(1);
		}
		close(sock_control);
	}
	freeaddrinfo(rp);


	printf("Connected to %s.\n", host);
	char buf_connect[MAXSIZE];
	memset(buf_connect, 0, MAXSIZE);
	recv_data(sock_control, buf_connect, MAXSIZE);
	if(compare_string(buf_connect, "220") == 1){

		print_reply(atoi("220"));
	}
	else{
		print_reply(atoi("550"));

	}



	ftclient_new_login();
	int judge = 0;

	while (1) 
	{

		if(situation == 2) {
			data_sock = ftclient_start_data_already(sock_control, ownport);
			memset(buffer, 0 , MAXSIZE);
			if ( ftclient_read_command(buffer, sizeof buffer, &cmd) < 0)
			{
				printf("Invalid command\n");
				continue;
			}

		}
		else if(situation == 1){

			//data_sock = ftclient_open_already(sock_control, cmd.arg, ownport);
			//printf("command\n");
			memset(buffer, 0 , MAXSIZE);
			if ( ftclient_read_command(buffer, sizeof buffer, &cmd) < 0)
			{
				printf("Invalid command\n");
				continue;
			}
			//printf("commandss\n");
			data_sock = ftclient_open_already(sock_control, cmd.arg, ownport);
			//printf("commandss\n");

		}
        else{
			memset(buffer, 0 , MAXSIZE);
			//printf("command\n");
			if ( ftclient_read_command(buffer, sizeof buffer, &cmd) < 0)
			{
				printf("Invalid command\n");
				continue;
			}


		}

        char result[MAXSIZE];
		recv_data(sock_control, result, MAXSIZE);
		//printf("%s\n", result);
		if(compare_string(result, "221") == 1 || compare_string(result, "225") == 1){

			print_reply(221);
			break;

		}
		else 
		{
           // printf("%s\n", result);
			if(strcmp(cmd.code, "PORT") == 0){
				data_sock = ftclient_open_conn(sock_control, cmd.arg);
				if(data_sock < 0){
					perror("Error opening socket for data connection");
					exit(1);
				}
				printf("200  PORT mode\n");
				used = 1;
				judge = 1;
				situation = 1;


			}
			else if(strcmp(cmd.code, "PASV") == 0){
              //  if(compare_string(result, "226")){

				//qu}
				data_sock = ftclient_open_pasv(sock_control, result);
				if(data_sock < 0){
					perror("Error opening socket for data connection");
					exit(1);
				}
				printf("227  PASV mode\n");
				used = 1;
				judge = 1;
				situation = 2;
			}


		
			if (strcmp(cmd.code, "LIST") == 0) {
				if(situation == 0){
					printf("%s", result);
					continue;
				}
				printf("%s", result);
				if(data_sock != -111){
					if(compare_string(result, "150") == 1){
						ftclient_list(data_sock, sock_control);
						char results[MAXSIZE];
						recv_data(sock_control, results, MAXSIZE);
						printf("%s", results);
						situation = 0;
					}
					else{
						printf("530 no such list or no module\n");
					}
				}
				else{
					printf("no module has chosen!\n");
				}


			}
			else if (strcmp(cmd.code, "RMD") == 0){
				if(compare_string(result, "250")){
					printf("250  successfully remove\n");
				}
				else{

					printf("530 not successfully remove\n");
				}
			}
			else if (strcmp(cmd.code, "MKD") == 0){
				//printf("%s", result);
				if(compare_string(result, "257")){
					printf("220  successfully build\n");
				}
				else{

					printf("530 not successfully build\n");
				}


			}
			else if (strcmp(cmd.code, "CWD") == 0){
                //printf("%s", result);
				if(compare_string(result, "250")){
					//printf("220  successfully changed\n");
                    printf("%s", result);
				}
				else{
                    printf("%s", result);
					//printf("530 not successfully changed\n");
				}


			}
			else if (strcmp(cmd.code, "RETR") == 0) 
			{
				if(situation == 0){
					printf("%s", result);
					continue;
				}
				printf("%s", result);
				if(data_sock != -111) {
					if(compare_string(result, "150")) {
                                                //char results[MAXSIZE];
                                                //recv_data(sock_control, results, MAXSIZE);
					        //printf("%s", results);
                                                //if(compare_string(results, "226"))
						ftclient_get(data_sock, sock_control, cmd.arg);
                                               // else
                                                //   printf("550 download fail\n");
						char results[MAXSIZE];
						recv_data(sock_control, results, MAXSIZE);
						printf("%s", results);


					}
					else{

						printf("550 download fail\n");
					}
					situation = 0;
				}
				else{
					printf("no module has chosen!\n");
				}

			}
			else if (strcmp(cmd.code, "STOR") == 0)
			{
				if(situation == 0){
					printf("%s", result);
					continue;
				}
				printf("%s", result);
				if(data_sock != -111){
                                        

					ftclient_put(data_sock, sock_control, cmd.arg);
					char results[MAXSIZE];
					recv_data(sock_control, results, MAXSIZE);
					printf("%s", results);
					situation = 0;
				}
				else{
					printf("no module has chosen!\n");
				}
			}
			else if (strcmp(cmd.code, "SYST") == 0)
			{
				printf("%s",result);
	
			}
			else if (strcmp(cmd.code, "TYPE") == 0)
			{
				if(strcmp(cmd.arg, "I") == 0)
					printf("200 Type set to I.\r\n");
				else{
					printf("500 for unacceptable syntax\r\n");
				}
	
			}

			//close(data_sock);
		}

	} 
	close(sock_control); 
    return 0;  
}
