#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <stdint.h>

#include "../include/server.h"
#include "../include/radix.h"

HttpServer* http_server_create(uint16_t port) {
    HttpServer* server = (HttpServer*)malloc(sizeof(HttpServer));
    if(server == NULL) {
        fprintf(stderr, "Error creating space on the heap for the server.");
        exit(EXIT_FAILURE);
    }

    server->tree = radix_tree_create();
    assert(server->tree != NULL);
    assert(server->tree->root != NULL);

    server->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(server->sockfd == -1) {
        fprintf(stderr, "Error creating file descriptor for the server.");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    if(bind(server->sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1 ) {
        fprintf(stderr, "Error binding server socket");
        exit(EXIT_FAILURE);
    }

    return server;
}

void http_server_run(HttpServer* server) {
    if(listen(server->sockfd, 1) == -1) {
        fprintf(stderr, "Error listening on the server socket.");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in client_addr;
    socklen_t sin_size;
    int new_sockd;
    char buffer[24000];

    while(true) {
        sin_size = sizeof(struct sockaddr_in);
        new_sockd = accept(server->sockfd, (struct sockaddr*)&client_addr, &sin_size);
        if(new_sockd == -1) {
            fprintf(stderr, "Error accepting connection.");
            exit(EXIT_FAILURE);
        }

        int recv_length = recv(new_sockd, &buffer, 1024, 0);
        while(recv_length > 0) {
            for(int i = 0; i < recv_length; i++) {
                putchar(buffer[i]);
            }
            recv_length = recv(new_sockd, &buffer, 1024, 0);
        }
    }
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