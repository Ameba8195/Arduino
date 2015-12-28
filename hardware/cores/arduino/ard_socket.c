#include <platform_opts.h>
#include <lwip/sockets.h> 
#include <lwip/netif.h>
#include <platform/platform_stdlib.h>
#include "ard_socket.h"

int start_server(uint16_t port, uint8_t protMode)
{
    int _sock;
    if(protMode == 0) {
        _sock = lwip_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    } else {
        _sock = lwip_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    }

    if (_sock < 0) {
        printf("\r\nERROR opening socket\r\n");
        return -1;
    }

    struct sockaddr_in localHost;
    memset(&localHost, 0, sizeof(localHost));

    localHost.sin_family = AF_INET;
    localHost.sin_port = htons(port);
    localHost.sin_addr.s_addr = INADDR_ANY;

    if (lwip_bind(_sock, (struct sockaddr *)&localHost, sizeof(localHost)) < 0) {
        printf("\r\nERROR on binding\r\n");
        return -1;
    }

    return _sock;
}

int sock_listen(int sock, int max)
{
	if(lwip_listen(sock , max) < 0){
		printf("\r\nERROR on listening\r\n");
		return -1;
	}	
	return 0;            
}

int get_available(int sock)
{
    int client_fd;
	struct sockaddr_in cli_addr;

	socklen_t client = sizeof(cli_addr);
	if((client_fd = lwip_accept(sock, (struct sockaddr *) &cli_addr, &client)) < 0){
		printf("\r\nERROR on accept\r\n");
		return -1;
	}
	else {
		printf("\r\nA client connected to this server :\r\n[PORT]: %d\r\n[IP]:%s\r\n\r\n", ntohs(cli_addr.sin_port), inet_ntoa(cli_addr.sin_addr.s_addr));
		return client_fd;	
	}
}

int get_receive(int sock, uint8_t* data, int length, int flag, uint32_t *peer_addr, uint16_t *peer_port)
{
    int ret = 0;
    struct sockaddr from;
    socklen_t fromlen;

    ret = lwip_recvfrom(sock, data, length, flag, &from, &fromlen);
    if ( ret >= 0 ) {
        if (peer_addr != NULL) {
            *peer_addr = ((struct sockaddr_in *)&from)->sin_addr.s_addr;
        }
        if (peer_port != NULL) {
            *peer_port = ntohs(((struct sockaddr_in *)&from)->sin_port);
        }
    }

    return ret;
}

void stop_socket(int sock)
{
    lwip_close(sock);
}

int send_data(int sock, const uint8_t *data, uint16_t len)
{
    int ret;

    ret = lwip_write(sock, data, len);

    return ret;
}

int sendto_data(int sock, const uint8_t *data, uint16_t len, uint32_t peer_ip, uint16_t peer_port)
{
    int ret;

    struct sockaddr_in peer_addr;
    memset(&peer_addr, 0, sizeof(peer_addr));
    peer_addr.sin_family = AF_INET;
    peer_addr.sin_addr.s_addr = peer_ip;
    peer_addr.sin_port = htons(peer_port);

    ret = lwip_sendto(sock, data, len, 0, (struct sockaddr*)&peer_addr, sizeof(struct sockaddr_in));

    return ret;
}

int start_client(uint32_t ipAddress, uint16_t port, uint8_t protMode)
{
    int _sock;

    if(protMode == 0)//tcp
        _sock = lwip_socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    else
        _sock = lwip_socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (_sock < 0) {
        printf("\r\nERROR opening socket\r\n");
        return -1;
    }

    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = ipAddress;
    serv_addr.sin_port = htons(port);

    if (protMode == 0){//TCP MODE
        if(connect(_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == 0){
            printf("\r\nConnect to Server successful!\r\n");
            return _sock;
        }
        else{
            printf("\r\nConnect to Server failed!\r\n");
            stop_socket(_sock);
            return -1;
        }
    }
    else
        printf("\r\nUdp client setup Server's information successful!\r\n");

    return _sock;
}
