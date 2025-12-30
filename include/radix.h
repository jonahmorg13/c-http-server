#pragma once

#include <stdio.h>
#include <stdlib.h>

typedef struct RadixEdge RadixEdge;
typedef struct RadixNode RadixNode;
typedef struct RadixEdgeVector RadixEdgeVector ;
typedef struct RadixTree RadixTree;

struct RadixEdge {
    char* key;
    RadixNode* node;
};

struct RadixEdgeVector {
    RadixEdge* items;
    size_t capacity;
    size_t size;
};

struct RadixNode {
    //function goes here
    
    //vector of radixedges
    RadixEdgeVector* edges;
};

struct RadixTree {
    RadixNode* root;
};


RadixTree* radix_tree_create(void) {
    RadixTree* tree = (RadixTree*)malloc(sizeof(RadixTree));
    if(tree == NULL) {
        fprintf(stderr, "Error allocating the radix tree");
        exit(-1);
    }
    tree->root = NULL;

    return tree;
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
        free(node->edges->items[i].key);  // if you own the key
        radix_node_free(node->edges->items[i].node);  // recursive free
    }

    radix_edge_vector_free(node->edges);
    free(node);
}