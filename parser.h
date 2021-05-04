#ifndef HTML_PARSING_H
#define HTML_PARSING_H

#define MAX_URL 100

# include <regex.h>
# include "queue.h"

struct url_t
{
    char* http;
    char* hostname;
    char* path;
};

char* str_slice(const char* original, size_t s_index, size_t length);

void parse_html(char* html, struct queue_t* queue, char* relative_url);

void match(regex_t *regex, char *str, struct queue_t* queue, char* relative_url);

char* complete_url(char* current, char* source);

struct url_t parse_url(char* src);

char* parse_pattern(char* path, char* pattern, int offset);

#endif
