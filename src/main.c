#include <stdio.h>
#include <stdlib.h>
#include "../include/radix.h"


int main(int argc, char** argv) {
    RadixNode* node = radix_node_create();

    for(int i = 0; i < 100; i++) {
        radix_edge_vector_push(node->edges, radix_edge_create("cool!", NULL));
        radix_edge_vector_push(node->edges, radix_edge_create("wow!", NULL));
        radix_edge_vector_push(node->edges, radix_edge_create("no!", NULL));


        radix_edge_vector_push(node->edges, radix_edge_create("cool!", NULL));
        radix_edge_vector_push(node->edges, radix_edge_create("wow!", NULL));
        radix_edge_vector_push(node->edges, radix_edge_create("no!", NULL));


        radix_edge_vector_push(node->edges, radix_edge_create("123", NULL));
        radix_edge_vector_push(node->edges, radix_edge_create("345", NULL));
        radix_edge_vector_push(node->edges, radix_edge_create("19283", NULL));
    }

    radix_edge_vector_push(node->edges, radix_edge_create("nice!", NULL));
    radix_edge_vector_remove(node->edges, radix_edge_vector_size(node->edges) - 1);

    size_t amount = radix_edge_vector_size(node->edges);
    for(int i = 0; i < amount; i++) {
        RadixEdge* curr_edge = radix_edge_vector_at(node->edges, i);
        printf("i: [%d]\t edge_key: %s\n", i, curr_edge->key);
    }

    radix_node_free(node);
    return 0;
}