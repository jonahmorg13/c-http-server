#pragma once

typedef struct RadixEdge RadixEdge;
typedef struct RadixNode RadixNode;
typedef struct RadixEdgeVector Vector_RadixEdge;

struct RadixEdge {
    char* key;
    RadixNode* node;
};

struct RadixEdgeVector {
    RadixEdge* items;
    int capacity;
    int size;
};

struct RadixNode {
    //function goes here
    
    //vector of radixedges
    RadixEdgeVector edges;
};

RadixNode* radix_node_create(void);
void radix_edge_vector_push(RadixEdge* edge);

