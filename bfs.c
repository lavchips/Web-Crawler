/*
 * Using Breadth First Search to 'recursively' crawl the input URL
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "parser.h"
#include "socket.h"
#include "queue.h"
/*-----------------------------------------------------*/
#define MAX_URLS 100
/*-----------------------------------------------------*/

char* complete_url(char* current, char* source);

int main(int argc, char** argv){

    struct queue_t* fetched = create_queue();
    struct queue_t* queue = create_queue();

    int total_urls = 0;
    int isfirst = 1;

    // enqueue the input url
    enqueue(queue, argv[1], argv[1]);

    while(queue->top != NULL){

        char response[RESPONSE_SIZE];
        struct pair_t* pair = (struct pair_t*)malloc(sizeof(struct pair_t));

        // make a copy of the popped url struct (current_url + relative_url)
        memcpy(pair, dequeue(queue), sizeof(*pair));

        char* current_url = pair->curr_url;
        char* actual = strdup(current_url);
        char* source_url = pair->src_url;


        // parse url
        actual = complete_url(actual, source_url);

        // if the rightmost n-1 components of hosts do not match
        if(strcmp(strstr(parse_url(strdup(actual)).hostname,".") ,strstr(parse_url(source_url).hostname,"."))!= 0){
            continue;
        }

        if(isfirst){

            isfirst = 0;

            enqueue(fetched, actual, actual);
            total_urls++;

            int status = connect_socket(strdup(actual), response, queue);

            if(status == 503){

                connect_socket(strdup(actual), response, queue);

            }else if(status== -1){

                bzero(response, sizeof(response));

                continue;
            }

            // parse html
            parse_html(response, queue, actual);
            bzero(response, sizeof(response));

        }else{

            int crawled = 0;
            struct node_t *curr = fetched->top;

            // check if the url has been fetched
            while(curr != NULL){
                if(strcmp(curr->data->curr_url,strdup(actual)) == 0){
                    crawled = 1;
                    break;
                }else{
                    curr = curr->next;
                }
            }

            // if the page has not been fetched, fetch it
            if(crawled){
                bzero(response, sizeof(response));
                continue;
            }else{

                // check if the maximum urls fetched
                if(total_urls < MAX_URLS){
                    enqueue(fetched, actual, actual);
                    total_urls++;
                }else{
                    break;
                }

                int status = connect_socket(strdup(actual), response, queue);

                if(status==503){
                    connect_socket(strdup(actual), response, queue);
                }else if(status== -1){
                    bzero(response, sizeof(response));
                    continue;
                }

                parse_html(response, queue, actual);
                bzero(response, sizeof(response));
            }

            free(curr);
        }
    }

    struct node_t *pos = fetched->top;
    while(pos != NULL){
        printf("%s\n", pos->data->curr_url);
        pos = pos->next;
    }

    free_queue(fetched);
    free_queue(queue);

    (void)argc;
    return 0;
}



/* the following functions below are for bfs */

struct node_t* create_node(char* curr_url, char* src_url)
{
    struct node_t* temp = (struct node_t*)malloc(sizeof(struct node_t));
    temp->data = (struct pair_t*)malloc(sizeof(struct pair_t));
    temp->data->curr_url = curr_url;
    temp->data->src_url = src_url;
    temp->next = NULL;
    return temp;
}

struct queue_t* create_queue()
{
    struct queue_t* queue = (struct queue_t*)malloc(sizeof(struct queue_t));
    queue->top = queue->bottom = NULL;
    return queue;
}

void enqueue(struct queue_t* queue, char* curr_url, char* src_url)
{
    struct node_t* temp = create_node(curr_url, src_url);

    if (queue->bottom == NULL) {
        queue->top = queue->bottom = temp;
        return;
    }
    queue->bottom->next = temp;
    queue->bottom = temp;
}

struct pair_t* dequeue(struct queue_t* queue)
{
    struct pair_t* urls;
    assert(queue->top != NULL);

    struct node_t* temp = queue->top;
    urls = temp->data;

    queue->top = queue->top->next;

    if (queue->top == NULL)
        queue->bottom = NULL;

    free(temp);
    return urls;
}

void free_queue(struct queue_t *queue){
    struct node_t *curr, *prev;
    curr = queue->top;
    assert(queue != NULL);
    while(curr){
        prev = curr;
        curr = curr -> next;
        free(prev);
    }
    free(queue);
}

