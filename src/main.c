#include <stdio.h>
#include <stdlib.h>
#include "../include/radix.h"


int main(int argc, char** argv) {
    RadixNode* node = radix_node_create();

    RadixEdge edge = radix_edge_create("Hello", NULL);
    RadixEdge edge2 = radix_edge_create("world", NULL);
    RadixEdge edge3 = radix_edge_create("!", NULL);
    RadixEdge edge4 = radix_edge_create("  cool!", NULL);

    radix_edge_vector_push(node->edges, edge);
    radix_edge_vector_push(node->edges, edge2);
    radix_edge_vector_push(node->edges, edge3);
    radix_edge_vector_push(node->edges, edge4);

    radix_node_free(node);
    return 0;
}