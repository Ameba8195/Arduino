/*
Compile under windows (unsing mingw):

    mingw32-cc.exe -o ota_server ota_server.c -lwsock32 -static

*/

#include <io.h>
#include <stdio.h>
#include <winsock2.h>
#include <stdint.h>
 
#pragma comment(lib,"ws2_32.lib") //Winsock Library

int main(int argc , char *argv[])
{
    WSADATA wsa;
	FILE *pFile;
	long filesize;
	uint32_t ota_description[3];
	unsigned char *ota_buf = NULL;
	int port = -1;
    SOCKET s , new_socket;
    struct sockaddr_in serveraddr , clientaddr;
    int addrlen;
    int n, nOptval;

	if (argc < 3) {
		printf("Usage: ota_server <port> <ota_image_path>\n");
		return 0;
	}

	// init win sock
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("Failed. Error Code : %d\n",WSAGetLastError());
        return 1;
    }
    printf("Winsock initialised.\n");

	port = atoi(argv[1]);
	if (port <= 0) {
		printf("Invalid port number:%d\n", port);
		return 0;
	}

	pFile = fopen(argv[2], "rb");
	if ( pFile == NULL ) {
		printf("Fail to open file:%s\n", argv[2]);
		return 0;
	}

	// get file size;
	fseek(pFile, 0L, SEEK_END);
	filesize = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);

	ota_buf = (unsigned char *) malloc (filesize);
	fread(ota_buf, 1, filesize, pFile);
	fclose(pFile);

	ota_description[0] = ota_description[1] = 1;
	ota_description[2] = (uint32_t) filesize;

    //Create a socket
    if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET) {
        printf("Could not create socket : %d\n" , WSAGetLastError());
    }
    printf("Socket created.\n");

    //Prepare the sockaddr_in structure
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons( (unsigned short)port );

    //Bind
    if( bind(s ,(struct sockaddr *)&serveraddr , sizeof(serveraddr)) == SOCKET_ERROR) {
        printf("Bind failed with error code : %d" , WSAGetLastError());
        exit(EXIT_FAILURE);
    }
    printf("Bind done\n");

    //Listen to incoming connections
    listen(s , 3);

    //Accept and incoming connection
    printf("Waiting for incoming connections...\n");
     
    addrlen = sizeof(struct sockaddr_in);
     
    while( (new_socket = accept(s , (struct sockaddr *)&clientaddr, &addrlen)) != INVALID_SOCKET ) {
        printf("Connection accepted\n");

		nOptval = 1;
		setsockopt(new_socket, IPPROTO_TCP, TCP_NODELAY, (char *) &nOptval, sizeof(int));

		// Sending OTA image information
		n = send(new_socket, (const char *)ota_description, sizeof(ota_description), 0);
		printf("send %d bytes of data\n", n);
		n = send(new_socket, (const char *)ota_buf, filesize, 0);
		printf("send %d bytes of data\n", n);

		closesocket(new_socket);
    }
     
    if (new_socket == INVALID_SOCKET) {
        printf("accept failed with error code : %d" , WSAGetLastError());
    }

    closesocket(s);
    WSACleanup();

	if (ota_buf != NULL) {
		free(ota_buf);
		ota_buf = NULL;
	}
     
    return 0;
}