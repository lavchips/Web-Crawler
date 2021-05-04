#ifndef QUEUE_H
#define QUEUE_H

#include <assert.h>

struct pair_t
{
    char* src_url;
    char* curr_url;
};

struct node_t {
    struct pair_t* data;
    struct node_t* next;
};

struct queue_t {
    struct node_t *top;
    struct node_t *bottom;
};


struct node_t* create_node(char* curr_url, char* src_url);

struct queue_t* create_queue();

void enqueue(struct queue_t* queue, char* curr_url, char* src_url);

struct pair_t* dequeue(struct queue_t* queue);

void free_queue(struct queue_t *queue);

#endif



