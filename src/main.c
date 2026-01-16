#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include "radix.h"
#include "utils.h"
#include "server.h"

void run_tests() {
    run_prefix_tests();
    run_common_prefix_tests();
    run_radix_edge_vector_tests();
    //radix_tree_tests();
}

void show_sizes() {
    printf("sizeof [RadixTree]: %lu\n", sizeof(RadixTree));
    printf("sizeof [RadixNode]: %lu\n", sizeof(RadixNode));
    printf("sizeof [func_handler_t]: %lu\n", sizeof(func_handler_t));
    printf("sizeof [RadixEdgeVector]: %lu\n", sizeof(RadixEdgeVector));
    printf("sizeof [size_t]: %lu\n", sizeof(size_t));
    printf("sizeof [RadixEdge]: %lu\n", sizeof(RadixEdge));
}

int print_function_handler(HttpRequest* a, HttpResponse* b) {
    printf("[Server] Printing [%d] and [%d]\n", (int)(intptr_t)a, (int)(intptr_t)b);
    return (int)(intptr_t)a + (int)(intptr_t)b;
}

int index_handler(HttpRequest* req, HttpResponse* res) {
    printf("we have hit our index function handler!\n");

    char *response = 
    "HTTP/1.1 200 OK\r\n"
    "Server: jonahsServer/1.0\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Content-Length: 159\r\n"
    "Connection: close\r\n"
    "\r\n"
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head><title>My C Server</title></head>\n"
    "<body>\n"
    "    <h1>Hello Pimps & Scholars</h1>\n"
    "    <p>You thought I was feeling you?</p>"
    "</body>\n"
    "</html>\n";

    res->body = strdup(response);
    res->length = strlen(res->body);
    return 0;
}

int login_handler(HttpRequest* req, HttpResponse* res) {
    index_handler(req, res);
}


// void submit_handler(request_ctx_t* ctx) {

// }

void auth_middleware(int ctx, void(*next)()) {
    printf("[MIDDLEWARE - Auth]: Start");
    next();
    printf("[MIDDLEWARE - Auth]: End");
}

int main(int argc, char* argv[]) {
    signal(SIGPIPE, SIG_IGN);

    // ALWAYS run the test before running the server!!!!
    run_tests();
    show_sizes();

    HttpServer* server = http_server_create(60223);

    RouteGroup* g1 = http_server_create_group(server, "/api");
    route_group_use(g1, auth_middleware);
    {
        route_group_map_endpoint(g1, "GET", "/login", login_handler);
        route_group_map_endpoint(g1, "GET", "/submit", login_handler);
        route_group_map_endpoint(g1, "GET", "/students", login_handler);
        route_group_map_endpoint(g1, "GET", "/sterling_silver", login_handler);
    }
    {
        route_group_map_endpoint(g1, "POST", "/login", login_handler);
        route_group_map_endpoint(g1, "POST", "/test", login_handler);
        route_group_map_endpoint(g1, "POST", "/test123", login_handler);
        route_group_map_endpoint(g1, "POST", "/test1234", login_handler);
    }
    http_server_print_endpoints(server);
    http_server_run(server);
    http_server_delete(server);
    return EXIT_SUCCESS;
}