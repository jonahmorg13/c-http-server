#pragma once

#include "radix.h"
#include <sys/types.h>
#include <stdint.h>

#define SERVER_PORT 60223
#define STATIC_FILE_DIR "todo-app"
 
// SECTION
// Structs
//

//
// MIDDLEWARE
//
typedef struct RequestHandlingContext RequestHandlingContext;

typedef void(*middleware_func_t)(RequestHandlingContext* ctx, void* next);

typedef struct MiddlewareFunctionsVec {
    middleware_func_t* funcs;
    int size;
    int cap;
} MiddlewareFunctionsVec;

struct RequestHandlingContext {
    HttpRequest* req;
    HttpResponse* res;
    int curr_mw_idx;
    int middleware_size;
    MiddlewareFunctionsVec* middleware;
    func_handler_t func_handler;
};
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
    char* authorization;
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
void  http_server_use_static_files(HttpServer* server);
void http_server_print_endpoints(HttpServer* server);
void http_server_delete(HttpServer* server);

//
// connection
//
void handle_connection(HttpServer* server, int sockfd);
void parse_http_header(HttpHeader* header, char* buffer, size_t length);

HttpRequest* http_request_create(void);
void http_request_delete(HttpRequest* req);

HttpResponse* http_response_create(void);
void http_response_delete(HttpResponse* res);

void run_middleware_and_func_handler(HttpRequest* req, HttpResponse* res, MiddlewareFunctionsVec* middleware, func_handler_t func);

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
void route_group_map_static_file(RouteGroup* group, char* filepath);