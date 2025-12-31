#include <assert.h>
#include <stdlib.h>

#include "../include/radix.h"
#include "../include/utils.h"
#include "../include/server.h"

void run_tests() {
    run_prefix_tests();
    run_common_prefix_tests();
    run_radix_edge_vector_tests();
    radix_tree_tests();
}

int print_function_handler(int a, int b) {
    printf("[Server] Printing [%d] and [%d]\n", a, b);
    return a + b;
}

int main(int argc, char* argv[]) {
    // ALWAYS run the test before running the server!!!!
    run_tests();

    HttpServer* server = http_server_create();

    http_server_map_endpoint(server, "/print-add", print_function_handler);
    http_server_map_endpoint(server, "/subtract", test_func_handler2);
    http_server_map_endpoint(server, "/", test_func_handler);
    http_server_print_endpoints(server);

    printf("[Server] Running \"/print-add\"...\n");
    func_handler_t func1 = http_server_get_function_handler(server, "/");
    printf("[Server] Result: [%d]\n", func1(10, 20));

    http_server_delete(server);
    return EXIT_SUCCESS;
}