/*
Compile under windows (unsing mingw):

    mingw32-cc.exe -o jpeg_tcp_server.c jpeg_tcp_server.c.c -lwsock32

*/

#include<io.h>
#include<stdio.h>
#include<winsock2.h>
 
#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFSIZE 1500 
#define PORT 5001

int fileindex = 0;
FILE *pJPEG = NULL;
void handle_data(unsigned char *buf, int n) {
    if (n > 0) {
        if (pJPEG == NULL) {
            /* it comes a valid data but the file has not been created */
            char filename[20];
            sprintf(filename, "%04d.jpeg", fileindex);
            fileindex++;
            pJPEG = fopen(filename,"wb");
            printf("Create file \"%s\"\n", filename);
        }
        fwrite(buf, sizeof(unsigned char), n, pJPEG);
    } else {
        if (pJPEG != NULL) {
            /* it comes a invalid data (maybe socket close), so we close the file */
            fclose(pJPEG);
            pJPEG = NULL;
            printf("Close file\n");
        } else {
            /* it comes a invalid data and file is not opend. nop */
        }
    }
}

unsigned char buf[BUFSIZE];

int main(int argc , char *argv[])
{
    WSADATA wsa;
    SOCKET s , new_socket;
    struct sockaddr_in serveraddr , clientaddr;
    int addrlen;
    int n;
 
    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("Failed. Error Code : %d",WSAGetLastError());
        return 1;
    }
    printf("Winsock initialised.\n");
     
    //Create a socket
    if((s = socket(AF_INET , SOCK_STREAM , 0 )) == INVALID_SOCKET) {
        printf("Could not create socket : %d" , WSAGetLastError());
    }
     printf("Socket created.\n");
     
    //Prepare the sockaddr_in structure
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons( PORT );

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

        do {
            n = recvfrom(new_socket, buf, BUFSIZE, 0, (struct sockaddr *) &clientaddr, &addrlen);
            handle_data(buf, n);
        } while (n > 0);
    }
     
    if (new_socket == INVALID_SOCKET) {
        printf("accept failed with error code : %d" , WSAGetLastError());
        return 1;
    }
 
    closesocket(s);
    WSACleanup();
     
    return 0;
}