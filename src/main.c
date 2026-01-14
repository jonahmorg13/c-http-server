#include <assert.h>
#include <stdlib.h>

#include "radix.h"
#include "utils.h"
#include "server.h"

void run_tests() {
    run_prefix_tests();
    run_common_prefix_tests();
    run_radix_edge_vector_tests();
    radix_tree_tests();
}

void show_sizes() {
    printf("sizeof [RadixTree]: %lu\n", sizeof(RadixTree));
    printf("sizeof [RadixNode]: %lu\n", sizeof(RadixNode));
    printf("sizeof [func_handler_t]: %lu\n", sizeof(func_handler_t));
    printf("sizeof [RadixEdgeVector]: %lu\n", sizeof(RadixEdgeVector));
    printf("sizeof [size_t]: %lu\n", sizeof(size_t));
    printf("sizeof [RadixEdge]: %lu\n", sizeof(RadixEdge));
}

int print_function_handler(int a, int b) {
    printf("[Server] Printing [%d] and [%d]\n", a, b);
    return a + b;
}

int main(int argc, char* argv[]) {
    // ALWAYS run the test before running the server!!!!
    run_tests();
    show_sizes();

    HttpServer* server = http_server_create(60223);

    http_server_map_endpoint(server, "/print-add", print_function_handler);
    http_server_map_endpoint(server, "/subtract", test_func_handler2);
    http_server_map_endpoint(server, "/", test_func_handler);
    http_server_print_endpoints(server);

    printf("[Server] Running \"/print-add\"...\n");
    func_handler_t func1 = http_server_get_function_handler(server, "/");
    printf("[Server] Result: [%d]\n", func1(10, 20));

    http_server_run(server);

    http_server_delete(server);
    return EXIT_SUCCESS;
}