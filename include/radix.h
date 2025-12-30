#pragma once

#include <stdio.h>
#include <stdlib.h>

typedef struct RadixEdge RadixEdge;
typedef struct RadixNode RadixNode;
typedef struct RadixEdgeVector RadixEdgeVector ;
typedef struct RadixTree RadixTree;

//////////////////////////////////
// Radix Tree
////////////////////////////////////////
// used for endpoint mapping on my http server
////////////////////////////////////////
struct RadixTree {
    RadixNode* root;
};

// create
RadixTree* radix_tree_create(void);


///////////////////////
// Radix Node 
///////////////////////
struct RadixNode {
    //function goes here
    
    //vector of radixedges
    RadixEdgeVector* edges;
};

// create
RadixNode* radix_node_create(void);
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
