#include "../include/utils.h"
#include <stdbool.h>
#include <string.h>

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