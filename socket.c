/*
 * Establish client socket connection to url underlined server
 *
 * Reference : Lecture Slide - WK3-LEC2-Sockets Flow Ctrl
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <regex.h>

#include "parser.h"
#include "socket.h"
/*-----------------------------------------------------*/

/*
 * socket establishment
 */
int connect_socket(char* start_url, char* response, struct queue_t* queue){

    int client_socket;
    struct sockaddr_in server_addr;
    struct hostent *host;
    char recvBuff[BUFFER_SIZE];
    char* host_str;

    // create socket
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        error("Failed to create socket\n");
    }

    // make a copy of the input
    char* input = strdup(start_url);

    // parse url into: http, host, path
    struct url_t url = parse_url(input);

    // host checking
    host_str = url.hostname;
    if ((host = gethostbyname(host_str)) == NULL) {
        error("Host is not found\n");
    }

    // specify the address for connection
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *) host->h_addr, (char *) &server_addr.sin_addr.s_addr, host->h_length);
    server_addr.sin_port = htons(PORTNO);

    // connect the socket
    if (connect(client_socket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        error("Connection failed");
    }

    // send request
    char* req = generate_request(url.hostname, url.path, -1);
    write(client_socket, req, strlen(req));

    // read response data
    recv(client_socket, recvBuff, sizeof(recvBuff), 0);
    strcat(response, recvBuff);
    bzero(recvBuff, sizeof(recvBuff));


    int status_code = -1;

    // Only parse pages that have MIME-type text/html or it is a 301 page which needs redirecting
    if(strstr(strdup(response), "Content-Type: text/html") == NULL && (atoi(strstr(strdup(response), "HTTP/1.1 ") + 9) != 301)){
        return -1;
    }

    // to find status code
    if(strstr(response, "HTTP/1.1 ")!= NULL) {

        status_code = atoi(strstr(strdup(response), "HTTP/1.1 ") + 9);

        // 401: resend request with Authorization header and encoded base64 id:password
        //      reset received response and buffer
        if(status_code == 401) {

            req = generate_request(url.hostname, url.path, 401);
            write(client_socket, req, strlen(req));
            bzero(recvBuff, sizeof(recvBuff));

            // read authorised response
            recv(client_socket, recvBuff, sizeof(recvBuff), 0);
            strcat(response, recvBuff);
            bzero(recvBuff, sizeof(recvBuff));
        }

        // 503: simply re-establish the socket connection and resend the request
        else if(status_code == 503 || status_code == 504) {

            bzero(recvBuff, sizeof(recvBuff));
            close(client_socket);
            return 503;
        }

        // 301: find the redirecting url and enqueue that to the "waiting list" for fetching
        //      at the same time, do not parse this current 301 page
        else if(status_code == 301){

            if(strstr(response, "Location: ") != NULL){

                enqueue(queue, strdup(strstr(response, "Location: ")+10), start_url);
                bzero(recvBuff, sizeof(recvBuff));
                close(client_socket);
                return -1;
            }
        }

        // 200: do nothing really, just happily accept the response and parse it
        else if(status_code == 200) {
            bzero(recvBuff, sizeof(recvBuff));
        }

        // 404/414/410: no need to parse them since:
        //         404: not found
        //         414: url is too long for the server to process
        //         410: the page requested is no longer available
        else{
            bzero(recvBuff, sizeof(recvBuff));
            close(client_socket);
            return -1;
        }
    }

    close(client_socket);

    // Omits truncated page: if received data != content length
    if(strstr(response, "Content-Length") != NULL && strstr(response, "\r\n\r\n") != NULL){
        if(atoi(strstr(response, "Content-Length: ") + 16) == (int)strlen(strstr(response, "\r\n\r\n") + 4)){
            return 1;
        }else{
            return -1;
        }
    }else{
        return -1;
    }
}

/*
 * for generating request message
 */
char* generate_request(char* host, char* path, int status){
    char* request = "GET ";
    request = concat(request, path);
    request = concat(request, " HTTP/1.1\r\nHost: ");
    request = concat(request, host);

    if(status == 401){
        request = concat(request, "\r\nUser-Agent: yusy\r\n");
        request = concat(request, "Authorization: Basic eXVzeTpwYXNzd29yZA==\r\n\r\n");

    }else{
        request = concat(request, "\r\nUser-Agent: yusy\r\n\r\n");
    }
    return request;
}


/*
 * append string2 to string1
 */
char* concat(const char *s1, const char *s2)
{
    char *result = malloc(strlen(s1) + strlen(s2) + 1);
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

/*
 * just to make sure the program exits
 */
void error(char* message){
    printf("%s", message);
    exit(0);
}





