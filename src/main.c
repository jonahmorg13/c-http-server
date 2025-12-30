#include <assert.h>
#include <stdlib.h>
#include "../include/radix.h"
#include "../include/utils.h"

int main(int argc, char* argv[]) {
    run_prefix_tests();
    run_common_prefix_tests();
    run_radix_edge_vector_tests();
    radix_tree_tests();

    return EXIT_SUCCESS;
}