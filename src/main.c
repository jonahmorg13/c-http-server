#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include "../include/radix.h"
#include "../include/utils.h"

void run_prefix_test(char* left, char* right, bool expected) {
    bool result = is_prefix(left, right);
    printf("Prefix: \"%15s\" | String: \"%15s\" |\t Expected: %s |\t Result: %s\n",
           left, right, 
           expected ? "TRUE " : "FALSE", 
           (result == expected) ? "PASS" : "FAIL");
}

void prefix_tests() {
    run_prefix_test("abc", "abcdef", true);
    run_prefix_test("abc", "abdghi", false);
    run_prefix_test("", "hello", true);
    run_prefix_test("hello", "hell", false);
    run_prefix_test("Case", "case sensitive", false);
}

void radix_tree_insert_tests() {
    //TODO: write the test cases from wikipedia
    RadixTree* tree = radix_tree_create();
    radix_edge_vector_push(tree->root->edges, radix_edge_create("/test", NULL));
    radix_edge_vector_push(tree->root->edges, radix_edge_create("/slow", NULL));
    radix_tree_insert(tree, "/water", test_func_handler);


    RadixTree* treeTwo = radix_tree_create();
    radix_tree_insert(tree, "/", test_func_handler);
    radix_tree_insert(tree, "/students", test_func_handler2);
    radix_tree_insert(tree, "/users", test_func_handler3);
    radix_tree_insert(tree, "/study", test_func_handler4);

    RadixNode* nodeOne = radix_tree_get_func_handler(treeTwo, "/study");
    RadixNode* nodeTwo = radix_tree_get_func_handler(treeTwo, "/");
    assert(nodeOne != NULL);
    assert(nodeOne!= NULL);
}

int main(int argc, char** argv) {
    prefix_tests();
    radix_tree_insert_tests();

    RadixNode* node = radix_node_create();
    int (*func_ptr)(int,int) = &test_func_handler;
    int sum = (*func_ptr)(2, 3);
    printf("sum: %d\n", sum);

    for(int i = 0; i < 1; i++) {
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