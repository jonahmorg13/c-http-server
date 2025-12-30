#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../include/server.h"
#include "../include/radix.h"

HttpServer* http_server_create(void) {
    HttpServer* server = (HttpServer*)malloc(sizeof(HttpServer));
    if(server == NULL) {
        fprintf(stderr, "Error creating space on the heap for the server.");
        exit(EXIT_FAILURE);
    }

    server->tree = radix_tree_create();
    assert(server->tree != NULL);
    assert(server->tree->root != NULL);

    return server;
}

void http_server_map_endpoint(HttpServer* server, char* endpoint, func_handler_t func_handler) {
    radix_tree_insert(server->tree, endpoint, func_handler);
}

func_handler_t http_server_get_function_handler(HttpServer* server, char* endpoint) {
    RadixNode* node = radix_tree_get_node(server->tree, endpoint);
    if(node == NULL || node->func_handler == NULL) {
        return NULL;
    }

    return node->func_handler; 
}

void http_server_print_endpoints(HttpServer* server) {
    radix_tree_print(server->tree);
}

void http_server_delete(HttpServer* server) {
    radix_tree_free(server->tree);
    free(server);
}