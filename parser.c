/*
 * parsing html to get the link after href in anchor tags
 *
 * Reference : http://man7.org/linux/man-pages/man3/regexec.3.html
 */
#include <sys/types.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "queue.h"
#include "socket.h"
/*-----------------------------------------------------*/

/*
 * find every string that matches this regex
 */
void parse_html(char* html, struct queue_t* queue, char* relative_url) {

    regex_t regex;

    if ((regcomp(&regex, "<a[[:space:]]+[^>]*href[[:space:]]?=[[:space:]]?\"([^\">]*)", REG_EXTENDED | REG_ICASE)) != 0) {
        printf("Regcomp() compilation failed\n");
        exit(0);
    }

    match(&regex, html, queue, relative_url);
    regfree(&regex);
}

/*
 * enqueue all the parsed.found.matches into our "waiting list" for fetching
 */
void match(regex_t *regex, char *str, struct queue_t* queue, char* relative_url) {

    regmatch_t pmatch[2];
    int cflags = 0;

    size_t offset = 0;
    size_t length = strlen(str);

    char* url;

    // while looping, find the current matched url and move on to the next match,
    //                parse every found url so that all the enqueued urls are perfectly formatted
    while (regexec(regex, str + offset, 2, pmatch, cflags) == 0) {

        char* returned = "";

        cflags = REG_NOTBOL;

        url = str_slice(str, offset + pmatch[1].rm_so, offset + pmatch[1].rm_eo);
        returned = concat(returned, parse_url(url).http);
        returned = concat(returned, parse_url(url).hostname);
        returned = concat(returned, parse_url(url).path);
        enqueue(queue, returned, relative_url);

        offset += pmatch[0].rm_eo;

        if (pmatch[0].rm_so == pmatch[0].rm_eo) {
            offset += 1;
        }

        if (offset > length) {
            break;
        }
    }
}

/*
 * string slicing
 */
char* str_slice(const char* original, size_t s_index, size_t length){

    return strndup(original+s_index, length - s_index);
}

/*
 * fill in the missing components of an url based on its parent url
 */
char* complete_url(char* current, char* source) {

    // if there's // before hostname
    if((strstr(current, "//") != NULL) && strlen(parse_url(strstr(current,"/")+2).hostname) != 0 && strlen(parse_url(current).hostname) == 0){

        current = concat("http:", strstr(strdup(current), "//"));

    }
        // if there's / before hostname
    else if((strstr(current, "/") != NULL) && strlen(parse_url(strstr(current,"/")+1).hostname) != 0 && strlen(parse_url(current).hostname) == 0){

        current = concat("http:/", strstr(strdup(current), "/"));

    }
        // if both protocol and hostname are missing
    else if(strlen(parse_url(current).http) == 0 && strlen(parse_url(current).hostname) == 0){

        // if there is /
        if(strstr(current, "/") != NULL){

            // if the slash is not at index 0
            if(strlen(current) - strlen(strstr(current, "/")) != 0) {
                char *pos = strdup(source);
                while (strstr(pos, "/") != NULL) {
                    pos++;
                }
                pos = strndup(source, strlen(source) - strlen(pos));
                current = concat(pos, current);

            }else{
                current = concat(strdup(parse_url(source).hostname), current);
                current = concat("http://", current);

            }
            // if there is no slash
        }else{
            char *pos = strdup(source);
            while (strstr(pos, "/") != NULL) {
                pos++;
            }
            pos = strndup(source, strlen(source) - strlen(pos));
            current = concat(pos, current);

        }
    }

    // if only the protocol is missing
    if(strlen(parse_url(current).http) == 0 && strlen(parse_url(current).hostname) != 0){
        current = concat("http://", current);
    }
    return strdup(current);
}


/*
 * divide the given url into: protocol, hostname, path
 */
struct url_t parse_url(char* src){
    int res;
    struct url_t url;
    const char* pattern = "(http://)?([^/][^/]{0,62}(\\.[^/][^/]{0,62})+)?([-a-zA-Z0-9()@:%_\\\\\\+\\.~#?&//=]*)";
    regex_t regex;

    regmatch_t pmatch[5];

    if( (res = regcomp(&regex, pattern, REG_EXTENDED)) != 0)
    {
        error("regcomp() compilation failed\n");
    }

    res = regexec(&regex, src, 5, pmatch, REG_NOTBOL);

    if(res == REG_NOMATCH)
    {
        error("no url component matches found \n");
    }

    url.http = strndup(src + pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
    url.hostname = strndup(src + pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so);
    url.path = strndup(src + pmatch[4].rm_so, pmatch[4].rm_eo - pmatch[4].rm_so);

    // x.html accidentally got matched into hostname
    if(strstr(url.hostname, ".html") != NULL){
        url.path = url.hostname;
        url.hostname = "";
    }

    // path cannot be empty in the request message
    if(strlen(url.path) == 0){
        url.path = "/";
    }

    // remove the redundant slash
    url.path = parse_pattern(url.path, "//", 1);
    regfree(&regex);

    return url;
}


/*
 * mainly used for removing double slashes in a given path
 * e.g. //a.html -> /a.html => offset =  1
 */
char* parse_pattern(char* path, char* pattern, int offset){
    const char *substr = strstr(path, pattern);
    char *parsed_str = NULL;
    if(substr != NULL){
        size_t len = substr - path;
        parsed_str = malloc(len + 1);
        if(parsed_str != NULL){
            memcpy(parsed_str, path, len);
            parsed_str[len] = 0;
        }
        parsed_str = concat(parsed_str, substr + offset);
    }else{
        parsed_str = path;
    }

    return parsed_str;
}