#ifndef SOCKET_H
#define SOCKET_H

#define PORTNO 80
#define BUFFER_SIZE 100000
#define RESPONSE_SIZE 100000
/*-----------------------------------------------------*/

int connect_socket(char* start_url, char* response, struct queue_t* queue);

//char* extract_addr(char* original, char* substr);
void read_response(int client_socket, char* recvBuff, char* response);

char* generate_request(char* host, char* path, int status);

char* concat(const char *s1, const char *s2);

void error(char* message);

/*-----------------------------------------------------*/

#endif
