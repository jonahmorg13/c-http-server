#pragma once
#include <stdbool.h>
#include <stddef.h>

bool is_prefix(char* left, char* right);
size_t has_common_prefix(char* left, char* right);

////////////
// TESTS
////////////
void run_prefix_test(char* left, char* right, bool expected);
void run_prefix_tests();

void run_common_prefix_test(char* left, char* right, size_t expected);
void run_common_prefix_tests();