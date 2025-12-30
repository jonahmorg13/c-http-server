#include <stdbool.h>
#include <string.h>
#include <assert.h>

#include "../include/radix.h"
#include "../include/utils.h"

int test_func_handler(int a, int b) {
    return a + b;
}

int test_func_handler2(int a, int b) {
    return a - b;
}

int test_func_handler3(int a, int b) {
    return a * b;
}

int test_func_handler4(int a, int b) {
    return a / b;
}

RadixTree* radix_tree_create(void) {
    RadixTree* tree = (RadixTree*)malloc(sizeof(RadixTree));
    if(tree == NULL) {
        fprintf(stderr, "Error allocating the radix tree");
        exit(-1);
    }
    tree->root = NULL;

    return tree;
}

void radix_tree_insert(RadixTree* tree, char* key, func_handler_t func_handler) {

}

RadixNode* radix_tree_get_func_handler(RadixTree* tree, char* search_key) {
    unsigned int curr_search_key_idx = 0;
    RadixNode* curr_node = tree->root;
    size_t search_key_len = strlen(search_key);

    while(curr_node != NULL && !radix_node_is_leaf(curr_node) && curr_search_key_idx < search_key_len) {
        RadixEdgeVector* edges_to_search = curr_node->edges;
        size_t edge_vector_size = radix_edge_vector_size(edges_to_search);

        for(int i = 0; i < edge_vector_size; i++) {
            RadixEdge* curr_edge = radix_edge_vector_at(edges_to_search, i);
            char* curr_key = curr_edge->key;
            if(is_prefix(curr_key, &search_key[curr_search_key_idx])) {
                curr_node = curr_edge->node;
                curr_search_key_idx += strlen(curr_key);
                break;
            }
        }
    }

    if(curr_node != NULL && curr_search_key_idx == search_key_len)
        return curr_node;

    return NULL;
}

void radix_tree_free(RadixTree* tree) {
    radix_node_free(tree->root);
    free(tree);
}

RadixNode* radix_node_create(void) {
    RadixNode* node = (RadixNode*)malloc(sizeof(RadixNode));
    if(node == NULL) {
        fprintf(stderr, "Error allocating the radix node");
        exit(-1);
    }

    RadixEdgeVector* vector = (RadixEdgeVector*)malloc(sizeof(RadixEdgeVector));
    node->edges = vector;
    node->edges->items = NULL;
    node->edges->capacity = 0;
    node->edges->size = 0;

    return node;
}

bool radix_node_is_leaf(RadixNode* node) {
    return radix_edge_vector_is_empty(node->edges);
}

RadixEdge radix_edge_create(char* key, RadixNode* node_ptr) {
    RadixEdge edge;
    edge.key = key;
    edge.node = node_ptr;
    return edge;
}

void radix_edge_vector_push(RadixEdgeVector* vector, RadixEdge edge) {
    if(vector->items == NULL) {
        vector->items = (RadixEdge*)malloc(sizeof(RadixEdge));
        if(vector->items == NULL) {
            fprintf(stderr, "Error allocating the vector");
            exit(-1);
        }
        vector->capacity = 1;
    }
    if(vector->size >= vector->capacity) {
        vector->capacity *= 2;
        vector->items = (RadixEdge*)realloc(vector->items, sizeof(RadixEdge) * vector->capacity);
    }
    vector->items[vector->size] = edge;
    vector->size++;
} 

bool radix_edge_vector_is_empty(RadixEdgeVector* vector) {
    return vector->size == 0;
}

size_t radix_edge_vector_size(RadixEdgeVector* vector) {
    return vector->size;
}

RadixEdge* radix_edge_vector_at(RadixEdgeVector* vector, unsigned int idx) {
    if(idx >= radix_edge_vector_size(vector) || idx < 0) {
        fprintf(stderr, "Attempt to access vector outside of bounds.");
        fprintf(stderr, "Vector [%p]\nSize: [%d]\n Idx: [%d]\n", &vector, (int)vector->size, idx);
        return NULL;
    }

    return &vector->items[idx];
}

void radix_edge_vector_remove(RadixEdgeVector* vector, unsigned int idx) {
    if(idx >= radix_edge_vector_size(vector) || idx < 0) {
        fprintf(stderr, "Attempt to access vector outside of bounds.");
        fprintf(stderr, "Vector [%p]\nSize: [%d]\n Idx: [%d]\n", &vector, (int)vector->size, idx);
        return;
    }

    if(radix_edge_vector_is_empty(vector)) {
        fprintf(stderr, "Attempt to remove edge from empty vector");
        return;
    }

    while(idx < vector->size - 1) {
        RadixEdge next = vector->items[idx + 1];
        vector->items[idx] = next;
        idx++;
    }

    vector->size--;
}

void radix_edge_vector_free(RadixEdgeVector* vector) {
    free(vector->items);
    free(vector);
}

void radix_node_free(RadixNode* node) {
    if(node == NULL) return;

    for (size_t i = 0; i < node->edges->size; i++) {
        radix_node_free(node->edges->items[i].node); 
    }

    radix_edge_vector_free(node->edges);
    free(node);
}



////////////
// TESTS
////////////
void radix_tree_tests() {
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


void run_radix_edge_vector_tests() {
    test_vector_growth_and_capacity();
    test_vector_removal_boundaries();
    test_null_and_empty_keys();
}

void test_vector_growth_and_capacity() {
    printf("Running: test_vector_growth_and_capacity\n");
    RadixNode* node = radix_node_create();
    
    // Test large number of insertions to trigger multiple reallocations
    for (int i = 0; i < 1000; i++) {
        char key[20];
        sprintf(key, "key_%d", i);
        radix_edge_vector_push(node->edges, radix_edge_create(key, NULL));
    }
    
    assert(radix_edge_vector_size(node->edges) == 1000);
    assert(strcmp(radix_edge_vector_at(node->edges, 999)->key, "key_999") == 0);
    
    radix_node_free(node);
    printf("PASSED: test_vector_growth_and_capacity\n");
}

void test_vector_removal_boundaries() {
    printf("Running: test_vector_removal_boundaries\n");
    RadixNode* node = radix_node_create();

    assert(radix_edge_vector_is_empty(node->edges));
    radix_edge_vector_push(node->edges, radix_edge_create("first", NULL));
    assert(!radix_edge_vector_is_empty(node->edges));
    radix_edge_vector_push(node->edges, radix_edge_create("middle", NULL));
    radix_edge_vector_push(node->edges, radix_edge_create("last", NULL));

    // Remove from the middle (tests shifting logic)
    radix_edge_vector_remove(node->edges, 1); 
    assert(radix_edge_vector_size(node->edges) == 2);
    assert(strcmp(radix_edge_vector_at(node->edges, 1)->key, "last") == 0);

    // Remove the new last
    radix_edge_vector_remove(node->edges, 1);
    assert(radix_edge_vector_size(node->edges) == 1);
    assert(strcmp(radix_edge_vector_at(node->edges, 0)->key, "first") == 0);

    // Remove the final element (empty state)
    radix_edge_vector_remove(node->edges, 0);
    assert(radix_edge_vector_size(node->edges) == 0);

    radix_node_free(node);
    printf("PASSED: test_vector_removal_boundaries\n");
}

void test_null_and_empty_keys() {
    printf("Running: test_null_and_empty_keys\n");
    RadixNode* node = radix_node_create();

    // Testing empty strings as keys
    radix_edge_vector_push(node->edges, radix_edge_create("", NULL));
    assert(radix_edge_vector_size(node->edges) == 1);
    assert(strlen(radix_edge_vector_at(node->edges, 0)->key) == 0);
    assert(!radix_edge_vector_is_empty(node->edges));

    radix_node_free(node);
    printf("PASSED: test_null_and_empty_keys\n");
}