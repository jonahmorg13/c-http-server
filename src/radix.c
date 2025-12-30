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
    tree->root = radix_node_create(NULL);

    return tree;
}

void radix_tree_insert(RadixTree* tree, char* new_key, func_handler_t func_handler) {
    unsigned int new_key_idx = 0;
    RadixNode* curr_node = tree->root;
    size_t new_key_len = strlen(new_key);

    while(curr_node != NULL && new_key_idx < new_key_len) {
        RadixEdgeVector* edges_to_search = curr_node->edges;
        size_t edge_vector_size = radix_edge_vector_size(edges_to_search);

        bool match_found = false;
        for(int i = 0; i < edge_vector_size; i++) {
            RadixEdge* curr_edge = radix_edge_vector_at(edges_to_search, i);
            char* curr_key = curr_edge->key;
            size_t curr_key_len = strlen(curr_key);

            size_t common_prefix_amount = has_common_prefix(curr_key, &new_key[new_key_idx]);
            if(common_prefix_amount == curr_key_len) {
                curr_node = curr_edge->node;
                new_key_idx += curr_key_len;
                match_found = true;
                break;
            }

            if(common_prefix_amount > 0 && common_prefix_amount < curr_key_len) {
                char* prefix = strndup(curr_key, common_prefix_amount);
                char* suffix = strdup(&curr_key[common_prefix_amount]);
                
                int new_key_ended_here = (new_key_idx + common_prefix_amount == new_key_len);
                RadixNode* split_node = radix_node_create(new_key_ended_here ? func_handler : NULL);

                RadixEdge new_edge = radix_edge_create(prefix, split_node);

                // this creates a shallow copy of the radix edge object
                // this shallow object is on the stack
                RadixEdge moved_edge = *curr_edge; 
                // when you assign the new key here, it doesn't break curr_edge because curr_edge still points to the other one
                moved_edge.key = suffix;
                // we are pushing a new value from the stack into 
                radix_edge_vector_push(split_node->edges, moved_edge);

                if (!new_key_ended_here) {
                    char* divergence_key = strdup(&new_key[new_key_idx + common_prefix_amount]);
                    radix_edge_vector_push(split_node->edges, 
                        radix_edge_create(divergence_key, radix_node_create(func_handler)));
                } 

                free(curr_edge->key);
                edges_to_search->items[i] = new_edge;

                return;
            }
        }
        if(match_found == false) break;
    }

    if(new_key_idx == new_key_len) {
        fprintf(stderr, "'radix_tree_insert': node already exists");
        return;
    }

    char* new_edge_key = strdup(&new_key[new_key_idx]);
    radix_edge_vector_push(curr_node->edges, radix_edge_create(new_edge_key, radix_node_create(func_handler)));
}

RadixNode* radix_tree_get_node(RadixTree* tree, char* search_key) {
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

// Helper function for recursion
void radix_print_recursive(RadixNode* node, int depth) {
    if (node == NULL || node->edges == NULL) return;

    size_t edge_count = radix_edge_vector_size(node->edges);

    for (size_t i = 0; i < edge_count; i++) {
        RadixEdge* edge = radix_edge_vector_at(node->edges, i);
        
        // 1. Print Indentation (2 spaces per depth level)
        for (int j = 0; j < depth; j++) {
            printf("  ");
        }

        // 2. Print the tree structure and the edge key
        printf("|-- %s", edge->key);

        // 3. Check if the node this edge points to is a valid key end
        // (Assuming a non-NULL handler means a key ends here)
        if (edge->node->func_handler != NULL) {
            printf("  [KEY]"); 
        }
        
        printf("\n");

        // 4. Recurse down
        radix_print_recursive(edge->node, depth + 1);
    }
}

// Main print function
void radix_tree_print(RadixTree* tree) {
    if (tree == NULL || tree->root == NULL) {
        printf("(Empty Tree)\n");
        return;
    }

    printf("ROOT");
    // Check if the root itself handles an empty string key (rare but possible)
    if (tree->root->func_handler != NULL) {
        printf(" [KEY]");
    }
    printf("\n");

    radix_print_recursive(tree->root, 0);
}

void radix_tree_free(RadixTree* tree) {
    radix_node_free(tree->root);
    free(tree);
}

RadixNode* radix_node_create(func_handler_t func_handler) {
    RadixNode* node = (RadixNode*)malloc(sizeof(RadixNode));
    node->func_handler = func_handler;
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
        free(node->edges->items[i].key);
        radix_node_free(node->edges->items[i].node); 
    }

    radix_edge_vector_free(node->edges);
    free(node);
}

////////////
// TESTS
////////////
void radix_tree_tests() {
    RadixTree* tree = radix_tree_create();
    radix_tree_insert(tree, "/slower", test_func_handler); 
    radix_tree_insert(tree, "/slot", test_func_handler);
    radix_tree_insert(tree, "/team", test_func_handler);
    radix_tree_insert(tree, "/te", test_func_handler);
    radix_tree_insert(tree, "/tester", test_func_handler);
    radix_tree_insert(tree, "/water", test_func_handler2);
    radix_tree_insert(tree, "/wat", test_func_handler); 
    radix_tree_insert(tree, "/waste", test_func_handler);
    radix_tree_insert(tree, "/api", test_func_handler);
    radix_tree_insert(tree, "/slow", test_func_handler);
    radix_tree_print(tree);
    assert(strcmp(radix_edge_vector_at(tree->root->edges, 0)->key, "/") == 0);
    RadixNode* indexNode = radix_edge_vector_at(tree->root->edges, 0)->node;

    RadixNode* node = radix_tree_get_node(tree, "/slot");
    RadixNode* waterNode = radix_tree_get_node(tree, "/water");

    assert(node != NULL);
    assert(node->func_handler(1,1) == 2);

    assert(waterNode != NULL);
    assert(waterNode->func_handler(1,1) == 0);

    radix_tree_free(tree);
}


void run_radix_edge_vector_tests() {
    test_vector_growth_and_capacity();
    test_vector_removal_boundaries();
    test_null_and_empty_keys();
}

void test_vector_growth_and_capacity() {
    printf("Running: test_vector_growth_and_capacity\n");
    RadixNode* node = radix_node_create(NULL);
    
    // Test large number of insertions to trigger multiple reallocations
    for (int i = 0; i < 1000; i++) {
        char buffer[20];
        sprintf(buffer, "key_%d", i);
        char* heap_key = strdup(buffer);
        radix_edge_vector_push(node->edges, radix_edge_create(heap_key, NULL));
    }
    
    assert(radix_edge_vector_size(node->edges) == 1000);
    assert(strcmp(radix_edge_vector_at(node->edges, 999)->key, "key_999") == 0);
    
    radix_node_free(node);
    printf("PASSED: test_vector_growth_and_capacity\n");
}

void test_vector_removal_boundaries() {
    printf("Running: test_vector_removal_boundaries\n");
    RadixNode* node = radix_node_create(NULL);

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
    RadixNode* node = radix_node_create(NULL);

    char* heap_key = (char*)malloc(sizeof(char));
    heap_key[0] = '\0';

    // Testing empty strings as keys
    radix_edge_vector_push(node->edges, radix_edge_create(heap_key, NULL));
    assert(radix_edge_vector_size(node->edges) == 1);
    assert(strlen(radix_edge_vector_at(node->edges, 0)->key) == 0);
    assert(!radix_edge_vector_is_empty(node->edges));

    radix_node_free(node);
    printf("PASSED: test_null_and_empty_keys\n");
}