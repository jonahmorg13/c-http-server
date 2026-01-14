#pragma once

#include "radix.h"
#include <sys/types.h>
#include <stdint.h>

typedef struct HttpServer {
    RadixTree* tree;
    int sockfd;
} HttpServer;

typedef struct HttpHeader {
    char* method;
    char* path;
    char* version;
    char* host;
    char* content_type;
    char* accept;
    char* connection;
    size_t content_length;
} HttpHeader;

typedef struct HttpRequest {
    HttpHeader* header;
    uint8_t* data;
} HttpRequest;

typedef struct HttpResponse {
    char* body;
    size_t length;
} HttpResponse;


HttpServer* http_server_create(uint16_t port);
void http_server_run(HttpServer* server);
void http_server_map_endpoint(HttpServer* server, char* endpoint, func_handler_t func_handler);
void http_server_map_static_files_in_dir(HttpServer* server, char* dir, char* prefix_endpoint);

func_handler_t http_server_get_function_handler(HttpServer* server, char* endpoint);
void http_server_print_endpoints(HttpServer* server);
void http_server_delete(HttpServer* server);

void handle_connection(HttpServer* server, int sockfd);

void parse_http_header(HttpHeader* header, char* buffer, size_t length);
void parse_http_data(uint8_t* data, char* buffer, size_t length);

HttpRequest* http_request_create(void);
void http_request_delete(HttpRequest* req);

HttpResponse* http_response_create(void);
void http_response_delete(HttpResponse* res);