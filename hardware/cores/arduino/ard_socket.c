#include <platform_opts.h>
#include <lwip/sockets.h> 
#include <lwip/netif.h>
#include <platform/platform_stdlib.h>
#include "ard_socket.h"

 int start_server(uint16_t port, uint8_t sock, uint8_t protMode)
{
	if(protMode == 0)//tcp
		sock = lwip_socket(AF_INET, SOCK_STREAM, 0);
	else
		sock = lwip_socket(AF_INET, SOCK_DGRAM, 0);

	if (sock < 0) {
		printf("\r\nERROR opening socket\r\n");
		return -1;
	}

    	struct sockaddr_in localHost;
    	memset(&localHost, 0, sizeof(localHost));
    
    	localHost.sin_family = AF_INET;
    	localHost.sin_port = htons(port);
    	localHost.sin_addr.s_addr = INADDR_ANY;
     
	if (lwip_bind(sock, (struct sockaddr *)&localHost,sizeof(localHost)) < 0) {
		printf("\r\nERROR on binding\r\n");
		return -1;
	}
	return 0;
}

 int tcp_listen(uint8_t sock, int max)
{
	if(lwip_listen(sock , max) < 0){
		printf("\r\nERROR on listening\r\n");
		return -1;
	}	
	return 0;            
}
	
 uint8_t get_available(uint8_t sock)
{
	struct sockaddr_in cli_addr;
	uint8_t newsockfd;
	socklen_t client = sizeof(cli_addr);
	if((newsockfd = lwip_accept(sock,(struct sockaddr *) &cli_addr,&client)) < 0){
		printf("\r\nERROR on accept\r\n");
		return -1;
	}
	else{
		printf("\r\nA client connected to this server :\r\n[PORT]: %d\r\n[IP]:%s\r\n\r\n", ntohs(cli_addr.sin_port), inet_ntoa(cli_addr.sin_addr.s_addr));
		return newsockfd;	
	}
}

 int get_receive(uint8_t sock, uint8_t* data, int length)
{
	int ret = 0;
	ret = lwip_read(sock, data, length);
	return ret;
}


 void stop_socket(uint8_t sock)
{
	lwip_close(sock);
}

 int send_data(uint8_t sock, const uint8_t *data, uint16_t len)
{
	int ret;
	ret = lwip_write(sock, data, len);
    	return ret ;
}

int  start_client(char* ipAddress, uint16_t port, uint8_t sock, uint8_t protMode)
{
	if(protMode == 0)//tcp
		sock = lwip_socket(AF_INET, SOCK_STREAM, 0);
	else
		sock = lwip_socket(AF_INET, SOCK_DGRAM, 0);

	if (sock < 0) {
		printf("\r\nERROR opening socket\r\n");
		return -1;
	}
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ipAddress);
	serv_addr.sin_port = htons(port);

	if (protMode == 0){//TCP MODE
		if(connect(sock, (struct sockaddr *)&serv_addr,  sizeof(serv_addr)) == 0){
			printf("\r\nConnect to Server successful!\r\n");
			return 0;
		}
		else{
                  
			printf("\r\nConnect to Server failed!\r\n");
			stop_socket(sock);
			return -1;
		}
	}
	else
		printf("\r\nUdp client setup Server's information successful!\r\n");
        return 0;
}
