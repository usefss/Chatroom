#include "connstruct.h"

connection_t* clients[1398];
int clientLimit = 0;


void* process(void* ptr)
{
        if (!ptr) pthread_exit(0);
				int i;
				int len;
        connection_t* conn = (connection_t*) ptr;
				char ip[INET_ADDRSTRLEN];
				char othersIp[INET_ADDRSTRLEN];
				int port;
				int othersPort;
				inet_ntop(AF_INET, &((conn->address).sin_addr), ip, INET_ADDRSTRLEN);
				port = ntohs((conn->address).sin_port);
				printf("connection established with IP : %s and PORT : %d\n",
																				ip, port);
        while(1)
        {
								char buffer[256] = {0};
								read (conn->sock, buffer, 256 * sizeof(char));
                if (buffer == "" || buffer == NULL){
									printf("connection closed form client\n");
									break;
								}
								for (i = 0; i < clientLimit; i ++){
										inet_ntop(AF_INET, &((clients[i]->address).sin_addr), othersIp, INET_ADDRSTRLEN);
										othersPort = ntohs((clients[i]->address).sin_port);
										// printf("checking with client %s %d\n", othersIp, othersPort);
										if (strcmp(ip, othersIp) != 0 || port != othersPort){
											len = strlen(buffer);
											write(clients[i]->sock, buffer, len * sizeof(char));
										}
								}
        }
        close(conn->sock);
        free(conn);
        pthread_exit(0);
}

int main(int argc, char const *argv[])
{
	int sock = -1;
	struct sockaddr_in address;
	int port;
	connection_t* connection;
	socklen_t cli_len=sizeof(struct sockaddr);
	pthread_t thread;
	if (argc != 2)
	{
		printf("[*] input error absorbed... [*]\n");
		return -1;
	}
	if (sscanf(argv[1], "%d", &port) <= 0)
	{
		printf("[*] unappropriate variable inputed: port [*]\n");
		return -2;
	}
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock <= 0)
	{
		printf("[*] failed to open socket... [*]\n");
		return -3;
	}
	printf("[*] socket successfully created [*]\n");
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");//it was other in org
	address.sin_port = htons(port);
	if (bind(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) < 0)
	{
		printf("[*] failed to bind to the port... [*]\n");
		return -4;
	}printf("[*] binded to port successfully [*]\n");
	if (listen(sock, 5) < 0)
	{
		printf("[*] failed to listen... [*]\n");
		return -5;
	}
	while (1)
	{
		printf("[*] server: ...\n");
		printf("	   listening started... [*]\n");
		connection = (connection_t*)malloc(sizeof(connection_t));
		connection->sock = accept(sock, (struct sockaddr*) &(connection->address), &cli_len);
		connection->addr_len = cli_len;

		if (connection->sock <= 0)
		{
			printf("[*] one unvalid connection lost... [*]\n");
			free(connection);
		}
		else{
			printf("[*] connection created for client [*]\n");
			if (clientLimit < 1398){
				clients[clientLimit] = connection;
				clientLimit++;
				pthread_create(&thread, 0, process, (void*)connection);
			}
			else {
				printf("maximom clients reached \n");
			}
		}
		sleep(2);
	}
	return 0;
}
