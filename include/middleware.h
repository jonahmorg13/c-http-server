#pragma once
#include "server.h"

void auth_middleware(RequestHandlingContext* ctx, void* next_arg);
void logging_middleware(RequestHandlingContext* ctx, void* next_arg);