#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include "radix.h"
#include "utils.h"
#include "server.h"
#include "middleware.h"

void run_tests() {
    run_prefix_tests();
    run_common_prefix_tests();
    run_radix_edge_vector_tests();
}

int index_handler(HttpRequest* req, HttpResponse* res) {
    printf("we have hit our index function handler!\n");

    char *response = 
    "HTTP/1.1 200 OK\r\n"
    "Server: jonahsServer/1.0\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Content-Length: 163\r\n"
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
    return index_handler(req, res);
}

//todo: fix memory leaks
int main(int argc, char* argv[]) {
    run_tests();
    signal(SIGPIPE, SIG_IGN);

    HttpServer* server = http_server_create(60223);

    RouteGroup* g1 = http_server_create_group(server, "/api");
    route_group_use(g1, logging_middleware);
    route_group_use(g1, auth_middleware);
    {
        route_group_map_endpoint(g1, "GET", "/login", login_handler);
        route_group_map_endpoint(g1, "GET", "/submit", login_handler);
        route_group_map_endpoint(g1, "GET", "/students", login_handler);
        route_group_map_endpoint(g1, "GET", "/sterling_silver", login_handler);

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