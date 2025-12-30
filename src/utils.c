#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../include/utils.h"

bool is_prefix(char* left, char* right) {
    size_t left_len = strlen(left);
    size_t right_len = strlen(right);

    if(left_len > right_len) return false;

    unsigned int curr_idx = 0;
    while(curr_idx < left_len) {
        if(right[curr_idx] - left[curr_idx] != 0) return false;
        curr_idx++;
    }

    return true;
}

size_t has_common_prefix(char* left, char* right) {
    size_t left_len = strlen(left);
    size_t right_len = strlen(right);

    size_t res = 0;

    res = 0;
    while(res < left_len && res < right_len) {
        if(right[res] - left[res] != 0) break;
        res++;
    }

    return res;
}

////////////
// TESTS
////////////
void run_prefix_test(char* left, char* right, bool expected) {
    bool result = is_prefix(left, right);
    printf("Prefix: \"%15s\" | String: \"%15s\" |\t Expected: %s |\t Result: %s\n",
           left, right, 
           expected ? "TRUE " : "FALSE", 
           (result == expected) ? "PASS" : "FAIL");
}

void run_prefix_tests() {
    run_prefix_test("abc", "abcdef", true);
    run_prefix_test("abc", "abdghi", false);
    run_prefix_test("", "hello", true);
    run_prefix_test("hello", "hell", false);
    run_prefix_test("Case", "case sensitive", false);
}

void run_common_prefix_test(char* left, char* right, size_t expected) {
    size_t result = has_common_prefix(left, right);
    printf("Left: \"%10s\" | Right: \"%10s\" |\t Expected: %zu |\t Result: %s\n",
           left, right, 
           expected, 
           (result == expected) ? "PASS" : "FAIL");
}

void run_common_prefix_tests() {
    printf("--- Running Common Prefix Tests ---\n");
    run_common_prefix_test("flower", "flow", 4);
    run_common_prefix_test("apple", "apple", 5);
    run_common_prefix_test("dog", "racecar", 0);
    run_common_prefix_test("Context", "context", 0);
    run_common_prefix_test("", "something", 0);
    run_common_prefix_test("bright", "brighter", 6);
    run_common_prefix_test("", "", 0);
}