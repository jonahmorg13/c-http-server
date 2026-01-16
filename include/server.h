#pragma once

#include "radix.h"
#include <sys/types.h>
#include <stdint.h>
 
// SECTION
// Structs
//

//
// MIDDLEWARE
//
// todo: finish the typedef for this
typedef void(*middleware_func_t)(int ctx, void* next);

typedef struct MiddlewareFunctionsVec {
    middleware_func_t* funcs;
    int size;
    int cap;
} MiddlewareFunctionsVec;

//
// Server 
//
typedef struct HttpServer {
    RadixTree* tree;
    int sockfd;
} HttpServer;

//
// Route Groups
//
typedef struct RouteGroup {
    MiddlewareFunctionsVec* middleware;
    HttpServer* server;
    char* prefix;
} RouteGroup;

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


//
// server
//
HttpServer* http_server_create(uint16_t port);
RouteGroup* http_server_create_group(HttpServer* server, char* prefix);
void http_server_run(HttpServer* server);

RadixTreeSearchResult* http_server_endpoint_search(HttpServer* server, char* path);
void http_server_print_endpoints(HttpServer* server);
void http_server_delete(HttpServer* server);

//
// connection
//
void handle_connection(HttpServer* server, int sockfd);

void parse_http_header(HttpHeader* header, char* buffer, size_t length);
void parse_http_data(uint8_t* data, char* buffer, size_t length);

HttpRequest* http_request_create(void);
void http_request_delete(HttpRequest* req);

HttpResponse* http_response_create(void);
void http_response_delete(HttpResponse* res);

//
// middleware
//
MiddlewareFunctionsVec* create_middleware_functions_vec(void);
void middleware_functions_vec_push(MiddlewareFunctionsVec* vec, middleware_func_t func);
void middleware_functions_free(MiddlewareFunctionsVec* vec);

//
// route groups
//
RouteGroup* create_route_group(HttpServer* server, char* prefix);
RouteGroup* http_server_create_group(HttpServer* server, char* prefix);
void route_group_free(RouteGroup* group);
void route_group_map_endpoint(RouteGroup* group, char* method, char* endpoint, func_handler_t func);
void route_group_use(RouteGroup* group, middleware_func_t func);