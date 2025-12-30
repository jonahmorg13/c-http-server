#include "../include/radix.h"

typedef struct HttpServer {
    RadixTree* tree;
} HttpServer;


HttpServer* http_server_create(void);
void http_server_delete(HttpServer* server);