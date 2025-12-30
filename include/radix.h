#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct RadixEdge RadixEdge;
typedef struct RadixNode RadixNode;
typedef struct RadixEdgeVector RadixEdgeVector ;
typedef struct RadixTree RadixTree;

typedef int(*func_handler_t)(int, int);

extern int test_func_handler(int a, int b);
extern int test_func_handler2(int a, int b);
extern int test_func_handler3(int a, int b);
extern int test_func_handler4(int a, int b);



//////////////////////////////////
// Radix Tree
////////////////////////////////////////
// used for endpoint mapping on my http server
////////////////////////////////////////
struct RadixTree {
    struct RadixNode* root;
};

// create
RadixTree* radix_tree_create(void);
// insert
void radix_tree_insert(RadixTree* tree, char* key, func_handler_t func_handler);
// get func handler
RadixNode* radix_tree_get_node(RadixTree* tree, char* search_key);

// free
void radix_tree_free(RadixTree* tree);

// Helper function for recursion
void radix_print_recursive(RadixNode* node, int depth);
void radix_tree_print(RadixTree* tree);

///////////////////////
// Radix Node 
///////////////////////
struct RadixNode {
    //function goes here
    func_handler_t func_handler;
    
    //vector of radixedges
    RadixEdgeVector* edges;
};

// create
RadixNode* radix_node_create(func_handler_t func_handler);
// is leaf
bool radix_node_is_leaf(RadixNode* node);
// free
void radix_node_free(RadixNode* node);


///////////////////////
// Radix Edge Vector
///////////////////////
struct RadixEdgeVector {
    RadixEdge* items;
    size_t capacity;
    size_t size;
};

// push
void radix_edge_vector_push(RadixEdgeVector* vector, RadixEdge edge);
// remove
void radix_edge_vector_remove(RadixEdgeVector* vector, unsigned int idx);
// at
RadixEdge* radix_edge_vector_at(RadixEdgeVector* vector, unsigned int idx);
// is empty
bool radix_edge_vector_is_empty(RadixEdgeVector* vector);
// size
size_t radix_edge_vector_size(RadixEdgeVector* vector);



///////////////////////
// Radix Edge
///////////////////////
struct RadixEdge {
    char* key;
    RadixNode* node;
};

RadixEdge radix_edge_create(char* key, RadixNode* node_ptr);


////////////
// TESTS
////////////
void radix_tree_tests();
void run_radix_edge_vector_tests();
void test_vector_growth_and_capacity();
void test_vector_removal_boundaries();
void test_null_and_empty_keys();
void test_complex_data_payloads();