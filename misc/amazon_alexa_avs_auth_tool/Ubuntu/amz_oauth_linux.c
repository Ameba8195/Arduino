/*

Compile command:
gcc -o amz_oauth_linux amz_oauth_linux.c -lssl -lcrypto

*/
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <openssl/x509_vfy.h>

char device_id[128];
char device_dsn[32];
char client_id[128];
char client_secret[128];
char allowed_origins[128];
char allowed_return_urls[128];
int server_port = -1;

char code[32];
char refresh_token[1024];

void init_openssl() {
    SSL_load_error_strings();   
    OpenSSL_add_ssl_algorithms();
}

int create_server_socket(int port) {
    int sockfd;
    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Unable to create socket\r\n");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(char){ 1 }, sizeof(int)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed\r\n");
    }

#if 0
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT, &(int){ 1 }, sizeof(int)) < 0) {
        perror("setsockopt(SO_REUSEPORT) failed\r\n");
    }
#endif

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
       perror("Unable to bind\r\n");
       exit(EXIT_FAILURE);
    }

    if (listen(sockfd, 5) < 0) {
       perror("Unable to listen\r\n");
       exit(EXIT_FAILURE);
    }

    return sockfd;
}

int create_client_socket(const char *hostname, const int port) {
    int sockfd;
    struct hostent *host;
    struct sockaddr_in dest_addr;

    if ( (host = gethostbyname(hostname)) == NULL ) {
        perror("cannot resolve hostname\r\n");
        abort();
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    memset( &dest_addr, 0, sizeof(struct sockaddr_in) );
    dest_addr.sin_family=AF_INET;
    dest_addr.sin_port=htons(port);
    dest_addr.sin_addr.s_addr = *(long*)(host->h_addr);

    if ( connect(sockfd, (struct sockaddr *) &dest_addr, sizeof(struct sockaddr)) == -1 ) {
        perror("fail to connect\r\n");
    }

    return sockfd;
}

void cleanup_openssl() {
    EVP_cleanup();
}

SSL_CTX *init_server_context() {
    SSL_CTX *ctx;

    ctx = SSL_CTX_new(SSLv23_server_method());
    if (!ctx) {
        perror("Unable to create SSL server context");
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    SSL_CTX_set_ecdh_auto(ctx, 1);

    if ( SSL_CTX_use_certificate_file(ctx, "ca.crt", SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if ( SSL_CTX_use_PrivateKey_file(ctx, "ca.key", SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    return ctx;
}

#define REFRESH_TOKEN_BEGIN "\"refresh_token\":\""
#define REFRESH_TOKEN_END   "\",\"token_type\""

void reqauth() {
    SSL_CTX *ctx = NULL;
    SSL *ssl = NULL;
    int sock = -1;

    const char *hostname = "api.amazon.com";
    const int port = 443;

    int n;
    char *p;
    char *pend;
    char writebuf[4096];
    char readbuf[4096];

    if (strlen(code) > 0) {
        do {
            if ( (ctx = SSL_CTX_new(SSLv23_client_method())) == NULL ) {
                perror("Unable to create SSL client context\r\n");
                break;
            }
            SSL_CTX_set_options(ctx, SSL_OP_ALL | SSL_OP_NO_SSLv2);
            SSL_CTX_set_mode(ctx, SSL_MODE_AUTO_RETRY);
            SSL_CTX_set_mode(ctx, SSL_MODE_RELEASE_BUFFERS);

            ssl = SSL_new(ctx);

            if ( (sock = create_client_socket(hostname, port)) < 0 ) {
                perror("fail to create socket\r\n");
                break;
            }

            SSL_set_fd(ssl, sock);

            if ( SSL_connect(ssl) != 1 ) {
                perror("Unable to build ssl client session\r\n");
                break;
            }

            // calculate content length first
            int content_len = 0;
            sprintf(writebuf, "grant_type=authorization_code&code=%s&redirect_uri=%s&client_id=%s&client_secret=%s\r\n",
                code,
                allowed_return_urls,
                client_id,
                client_secret
            );
            content_len = strlen(writebuf);

            p = writebuf;
            p += sprintf(p, "POST /auth/O2/token HTTP/1.1\r\n");
            p += sprintf(p, "Host: api.amazon.com\r\n");
            p += sprintf(p, "Content-Length: %d\r\n", content_len);
            p += sprintf(p, "Content-Type: application/x-www-form-urlencoded;charset=UTF-8\r\n");
            p += sprintf(p, "\r\n");
            p += sprintf(p, "grant_type=authorization_code&code=%s&redirect_uri=%s&client_id=%s&client_secret=%s\r\n",
                code,
                allowed_return_urls,
                client_id,
                client_secret
            );
            SSL_write(ssl, writebuf, strlen(writebuf));

            n = SSL_read(ssl, readbuf, sizeof(readbuf));
            if (n > 0) {
                if ( (p = strstr(readbuf, REFRESH_TOKEN_BEGIN)) != NULL ) {
                    p += strlen(REFRESH_TOKEN_BEGIN);
                    if ( (pend = strstr(p, REFRESH_TOKEN_END)) != NULL ) {
                        strncpy(refresh_token, p, pend - p);
                        refresh_token[pend - p + 1] = '\0';
printf("refresh_token:\r\n%s\r\n\r\n", refresh_token);
                    }
                }
            }

        } while (0);

        if (ssl != NULL) SSL_free(ssl);
        if (sock >= 0) close(sock);
        if (ctx != NULL) SSL_CTX_free(ctx);
    }
}

#define AUTHRESPONSE_BEGIN "GET /authresponse?code="
#define AUTHRESPONSE_END   "&scope="

void iotask(SSL *ssl) {
    int n;
    char *p;
    char *pend;
    char readbuf[4096];
    char writebuf[4096];

    while ( (n = SSL_read(ssl, readbuf, sizeof(readbuf))) > 0 ) {
        if ( (p = strstr(readbuf, AUTHRESPONSE_BEGIN)) != NULL ) {
            p += strlen(AUTHRESPONSE_BEGIN);
            if ( (pend = strstr(p, AUTHRESPONSE_END)) != NULL ) {
                strncpy(code, p, pend - p);
                code[pend - p + 1] = '\0';

                memset(refresh_token, 0, sizeof(refresh_token));
                reqauth();

                if (strlen(refresh_token) > 0) {
                    p = writebuf;
                    p += sprintf(p, "HTTP/1.1 200 OK\r\n\r\n");
                    p += sprintf(p, "<HTML><BODY>");
                    p += sprintf(p, "<head><meta http-equiv=\"refresh\" content=\"30\" /></head>");
                    p += sprintf(p, "<h1>Authentication completed.<br/></h1><h5>Refresh Token: <br/>%s<br/></h5>", refresh_token);
                    p += sprintf(p, "</BODY></HTML>");
                    SSL_write(ssl, writebuf, strlen(writebuf));
                } else {
                    p = writebuf;
                    p += sprintf(p, "HTTP/1.1 200 OK\r\n\r\n");
                    p += sprintf(p, "<HTML><BODY>");
                    p += sprintf(p, "<head><meta http-equiv=\"refresh\" content=\"30\" /></head>");
                    p += sprintf(p, "<h1>Authentication failed</h1>");
                    p += sprintf(p, "</BODY></HTML>");
                    SSL_write(ssl, writebuf, strlen(writebuf));                    
                }

            }
        }
    }

}

void init_amazon_data() {
    printf("Please enter device id: ");
    scanf("%s", device_id);

    printf("Please enter device dsn: ");
    scanf("%s", device_dsn);

    printf("Please enter client id: ");
    scanf("%s", client_id);

    printf("Please enter client secret: ");
    scanf("%s", client_secret);

    printf("Please enter allowed origins: ");
    scanf("%s", allowed_origins);

    printf("Please enter allowed return urls: ");
    scanf("%s", allowed_return_urls);

    printf("Please enter server port: ");
    scanf("%d", &server_port);

    printf("\r\n");
    printf("Please open below link to authenticate:\r\n");

    printf("https://amazon.com/ap/oa?client_id=%s", client_id);
    printf("&response_type=code&redirect_uri=%s", allowed_return_urls);
    printf("&scope=alexa%%3Aall&state=%s", "64a34e9b-b5f9-4258-89a7-2ec7e0d1f85a");
    printf("&scope_data=%%7B%%22alexa%%3Aall%%22%%3A%%7B%%22productID%%22%%3A%%22%s%%22%%2C%%22productInstanceAttributes%%22%%3A%%7B%%22deviceSerialNumber%%22%%3A%%22%s%%22%%7D%%7D%%7D\r\n",
        device_id,
        device_dsn
    );
    printf("Note: if received 'SSL ERROR CODE 46', please try another browser and choose to trust the certificate when asked\r\n");
}

int main(int argc, char **argv) {
    struct sockaddr_in client_addr;
    uint addrlen = sizeof(struct sockaddr_in);

    int server_sock;
    int client_sock;
    SSL_CTX *ctx;
    SSL *ssl;


    init_amazon_data();

    init_openssl();
    ctx = init_server_context();
    server_sock = create_server_socket(server_port);

    /* Handle connections */
    while(1) {
        if ( (client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addrlen)) < 0 ) {
            perror("Unable to accept");
            exit(EXIT_FAILURE);
        }

        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client_sock);

        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
        } else {
            iotask(ssl);
        }

        SSL_free(ssl);
        close(client_sock);
    }

    close(server_sock);
    SSL_CTX_free(ctx);
    cleanup_openssl();
}