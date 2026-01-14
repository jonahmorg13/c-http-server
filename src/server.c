#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <stdint.h>

#include "server.h"
#include "radix.h"

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

    int opt = 1;
    if (setsockopt(server->sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
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
    if(listen(server->sockfd, 1000) == -1) {
        fprintf(stderr, "Error listening on the server socket.");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in client_addr;
    socklen_t sin_size;
    int new_sockd;

    while(true) {
        sin_size = sizeof(struct sockaddr_in);
        new_sockd = accept(server->sockfd, (struct sockaddr*)&client_addr, &sin_size);
        if(new_sockd == -1) {
            fprintf(stderr, "Error accepting connection.");
            exit(EXIT_FAILURE);
        }

        handle_connection(server, new_sockd);
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

void handle_connection(HttpServer* server, int sockfd)
{
    size_t buffer_size = 4096;
    char buffer[buffer_size];

    HttpRequest* req = http_request_create(); 

    int recv_length = recv(sockfd, buffer, buffer_size, 0);
    if(recv_length < 0){
        fprintf(stderr, "Error receiving the header");
        exit(EXIT_FAILURE);
    }

    parse_http_header(req->header, buffer, recv_length);

    if(req->header->content_length > 0) {
        recv_length = recv(sockfd, buffer, buffer_size, 0);
        if(recv_length < 0){
            fprintf(stderr, "Error receiving the header");
            exit(EXIT_FAILURE);
        }
        parse_http_data(req->data, buffer, recv_length);
    }

    func_handler_t func_handler = http_server_get_function_handler(server, req->header->path);
    if(func_handler == NULL) {
        // write back a 404
    }
    else {
        // run the function. allowing it to write to the response!
    }

    http_request_delete(req);
    close(sockfd);
}

void parse_http_header(HttpHeader* header, char* buffer, size_t recv_length) {
    return;
}

void parse_http_data(uint8_t* data, char* buffer, size_t recv_length) {
    return;
}

HttpRequest* http_request_create(void) {
    HttpRequest* req = (HttpRequest*)malloc(sizeof(HttpRequest));
    if(req == NULL) {
        fprintf(stderr, "Error mallocing http request");
        exit(EXIT_FAILURE);
    }

    req->header = (HttpHeader*)malloc((sizeof(HttpHeader)));
    if(req->header == NULL) {
        fprintf(stderr, "Error allocating the http header");
        exit(EXIT_FAILURE);
    }

    req->data = NULL;
    return req;
}

void http_request_delete(HttpRequest* req) {
    free(req->header);
    if(req->data != NULL) {
        free(req->data);
    }
    free(req);
}