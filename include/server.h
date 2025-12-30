#include "../include/radix.h"

typedef struct HttpServer {
    RadixTree* tree;
} HttpServer;

HttpServer* http_server_create(void);
void http_server_map_endpoint(HttpServer* server, char* endpoint, func_handler_t func_handler);
func_handler_t http_server_get_function_handler(HttpServer* server, char* endpoint);
void http_server_print_endpoints(HttpServer* server);
void http_server_delete(HttpServer* server);