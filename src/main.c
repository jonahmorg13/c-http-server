#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include "radix.h"
#include "utils.h"
#include "server.h"
#include "middleware.h"
#include "handlers.h"

void run_tests() {
    run_prefix_tests();
    run_common_prefix_tests();
    run_radix_edge_vector_tests();
}

int main(int argc, char* argv[]) {
    run_tests();
    signal(SIGPIPE, SIG_IGN);

    HttpServer* server = http_server_create(SERVER_PORT);

    // todo: make it so you can use multiple static file directories
    // todo: and make it recursive
    http_server_use_static_files(server);
    {
        RouteGroup* g1 = http_server_create_group(server, "/api");
        route_group_use(g1, logging_middleware);
        route_group_use(g1, auth_middleware);
        route_group_map_endpoint(g1, "GET", "/students", students_handler);
    }
    {
        RouteGroup* g2 = http_server_create_group(server, "/api");
        route_group_map_endpoint(g2, "POST", "/login", login_handler);
    }
    http_server_print_endpoints(server);

    // where the magic happens...
    http_server_run(server);

    http_server_delete(server);

    // i learned that some c devs know that "the exit will handle memory deallocations"
    return EXIT_SUCCESS;
}