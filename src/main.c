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

int main(int argc, char* argv[]) {
    // ALWAYS run the test before running the server!!!!
    run_tests();

    HttpServer* server = http_server_create();
    http_server_delete(server);
    return EXIT_SUCCESS;
}