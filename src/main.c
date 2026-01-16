#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include "radix.h"
#include "utils.h"
#include "server.h"
#include "middleware.h"
#include <dirent.h>

void run_tests() {
    run_prefix_tests();
    run_common_prefix_tests();
    run_radix_edge_vector_tests();
}

void students_handler(HttpRequest* req, HttpResponse* res) {
    char *response = 
    "HTTP/1.1 200 OK\r\n"
    "Server: jonahsServer/1.0\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Content-Length: 177\r\n"
    "Connection: close\r\n"
    "\r\n"
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head><title>My C Server</title></head>\n"
    "<body>\n"
    "    <h1>Student 1</h1>\n"
    "    <p>Name: Jonah</p>\n"
    "    <h1>Student 2</h1>\n"
    "    <p>Name: Jada</p>\n"
    "</body>\n"
    "</html>\n";

    res->body = strdup(response);
    res->length = strlen(res->body);
}

void login_handler(HttpRequest* req, HttpResponse* res) {
    printf("we have hit our login function handler!\n");

    char *response = 
    "HTTP/1.1 200 OK\r\n"
    "Server: jonahsServer/1.0\r\n"
    "Content-Type: text/plain; charset=UTF-8\r\n"
    "Content-Length: 28\r\n"
    "Connection: close\r\n"
    "\r\n"
    "Password is SECRET_PASSWORD\n";

    res->body = strdup(response);
    res->length = strlen(res->body);
}

void  http_server_use_static_files(HttpServer* server) {
    RouteGroup* static_files_group = http_server_create_group(server, "/");

    DIR *d;
    struct dirent *dir;
    d = opendir(STATIC_FILE_DIR);
    if(d) {
        while((dir = readdir(d)) != NULL) {
            char* res = strstr(dir->d_name, ".");
            if(strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
                continue;

            route_group_map_static_file(static_files_group, dir->d_name);
        }
    }
    else {
        fprintf(stderr, "Could not open directory: %s", STATIC_FILE_DIR);
        exit(-1);
    }
}

//todo: fix memory leaks
int main(int argc, char* argv[]) {
    run_tests();
    signal(SIGPIPE, SIG_IGN);

    HttpServer* server = http_server_create(60223);

    http_server_use_static_files(server);

    RouteGroup* g1 = http_server_create_group(server, "/api");
    route_group_use(g1, logging_middleware);
    route_group_use(g1, auth_middleware);
    route_group_map_endpoint(g1, "GET", "/students", students_handler);

    RouteGroup* g2 = http_server_create_group(server, "/api");
    route_group_map_endpoint(g2, "POST", "/login", login_handler);

    http_server_print_endpoints(server);
    http_server_run(server);
    http_server_delete(server);
    return EXIT_SUCCESS;
}