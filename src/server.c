#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <stdint.h>

#include "server.h"
#include "radix.h"

char* not_found_response = 
    "HTTP/1.1 404 Not Found\r\n"       
    "Content-Type: text/plain\r\n"     
    "Content-Length: 18\r\n"           
    "Connection: close\r\n"            
    "\r\n"                             
    "404 Not Found\n";                 

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
    while(1) {
        size_t buffer_size = 16;
        char buffer[buffer_size];

        char header_buf[4096];
        char body_buf[4096];

        HttpRequest* req = http_request_create(); 
        HttpResponse* res = http_response_create();

        int recv_length = recv(sockfd, buffer, buffer_size, 0);
        int search_idx = 0;
        char search[] = {'\r', '\n', '\r', '\n'};
        int offset = 0;
        int end_offset = 0;
        bool header_buffer_fill_finished = false;
        while(recv_length > 0) {
            for(int i = 0; i < recv_length; i++) {
                if(buffer[i] != search[search_idx]) {
                    search_idx = 0;
                } 
                else {
                    search_idx++;
                    if(search_idx >= 4) {
                        end_offset = i + 1;
                        strncpy(header_buf + offset, buffer, end_offset);
                        header_buf[offset + end_offset] = '\0';
                        header_buffer_fill_finished = true;
                        break;
                    }
                }
            }
            if(header_buffer_fill_finished) break;
            strncpy(header_buf + offset, buffer, recv_length);
            offset += recv_length;
            recv_length = recv(sockfd, buffer, buffer_size, 0);
        }
        int body_offset = 0;
        if(end_offset != recv_length) {
            int length = buffer_size - end_offset;
            strncpy(body_buf, buffer + end_offset, length);
            body_offset = length;
        }
        recv_length = recv(sockfd, buffer, buffer_size, 0);
        while(recv_length > 0) {
            strncpy(body_buf + body_offset, buffer, recv_length);
            body_offset += recv_length;

            if(recv_length < buffer_size) break;

            recv_length = recv(sockfd, buffer, buffer_size, 0);
        }

        printf("HEADER: %s\n", header_buf);
        printf("END HEADER\n");

        printf("BODY: %s\n", body_buf);
        printf("END BODY\n");

        parse_http_header(req->header, header_buf, strlen);
        printf("method: %s\n", req->header->method);
        printf("path: %s\n", req->header->path);
        printf("version: %s\n", req->header->version);

        if(req->header->content_length > 0) {
            printf("receiving...\n");
            recv_length = recv(sockfd, buffer, buffer_size, 0);
            if(recv_length <= 0){
                fprintf(stderr, "Error receiving the header");
                break;
            }
            parse_http_data(req->data, buffer, recv_length);
        }

        func_handler_t func_handler = http_server_get_function_handler(server, req->header->path);
        if(func_handler == NULL) {
            printf("%s", "sending back error resposne\n");
            res->body = strdup(not_found_response);
            res->length = strlen(res->body);
            if(send(sockfd, res->body, res->length, 0) == -1) {
                fprintf(stderr, "Error sending our resposne\n");
            }
        }
        else {
            printf("%s", "running our function handler\n");
            func_handler(req, res);
            if(send(sockfd, res->body, res->length, 0) == -1) {
                fprintf(stderr, "Error sending our response\n");
            }
        }
        http_request_delete(req);
        http_response_delete(res);
    }
    close(sockfd);
}

void parse_http_header(HttpHeader* header, char* buffer, size_t length) {
    char curr_line[1028];

    int idx = 0;
    while(idx < length && buffer[idx] != ' ') idx++;

    header->method = (char*)malloc(sizeof(char) * idx + 1);
    if(header->method) {
        strncpy(header->method, buffer, idx);
        header->method[idx] = '\0';
    }

    buffer += idx + 1;
    length -= idx + 1;


    idx = 0;
    while(idx < length && buffer[idx] != ' ') idx++;

    header->path = (char*)malloc(sizeof(char) * idx + 1);
    if(header->path) {
        strncpy(header->path, buffer, idx);
        header->path[idx] = '\0';
    }

    buffer += idx + 1;
    length -= idx + 1;

    idx = 0;
    while(idx < length && buffer[idx] != '\n') idx++;

    header->version = (char*)malloc(sizeof(char) * idx + 1);
    if(header->version) {
        strncpy(header->version, buffer, idx);
        header->version[idx] = '\0';
    }

    //handle the other headers later??
    header->content_length = 0;

    return;
}


void parse_http_data(uint8_t* data, char* buffer, size_t length) {
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

HttpResponse* http_response_create(void) {
    HttpResponse* res = (HttpResponse*)malloc(sizeof(HttpResponse));
    if(res == NULL) {
        fprintf(stderr, "Error mallocing http response");
        exit(EXIT_FAILURE);
    }
    return res;
}

void http_response_delete(HttpResponse* res) {
    free(res->body);
    free(res);
}