#pragma once
#include <stdbool.h>

bool is_prefix(char* left, char* right);

////////////
// TESTS
////////////
void run_prefix_test(char* left, char* right, bool expected);
void run_prefix_tests();