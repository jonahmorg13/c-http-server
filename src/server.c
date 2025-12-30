#include "../include/server.h"
#include <stdio.h>
#include <stdlib.h>

HttpServer* http_server_create(void) {
    HttpServer* server = (HttpServer*)malloc(sizeof(HttpServer));
    if(server == NULL) {
        fprintf(stderr, "Error creating space on the heap for the server.");
        exit(EXIT_FAILURE);
    }

    server->tree = radix_tree_create();
}


void http_server_delete(HttpServer* server) {
    radix_tree_free(server->tree);
    free(server);
}