#include "string.h"
#include "stdio.h"
#include "server.h"

void auth_middleware(RequestHandlingContext* ctx, void* next_arg) {
    middleware_func_t next = (middleware_func_t)next_arg;
    printf("[MIDDLEWARE - Auth]: Start\n");

    char* invalid = 
    "HTTP/1.1 401 Unauthorized\r\n"
    "Content-Type: text/plain\r\n"
    "Content-Length: 25\r\n"
    "\r\n"
    "Error: access is denied.\n";

    if(strcmp(ctx->req->header->authorization, "SECRET_PASSWORD") != 0) {
        ctx->res->body = strdup(invalid);
        ctx->res->length = strlen(invalid);
        return;
    }

    if (next) next(ctx, NULL);
    printf("[MIDDLEWARE - Auth]: End\n");
}

void logging_middleware(RequestHandlingContext* ctx, void* next_arg) {
    middleware_func_t next = (middleware_func_t)next_arg;
    printf("[MIDDLEWARE - Logging]: Start\n");
    printf("Authorization: %s\n", ctx->req->header->authorization);
    if (next) next(ctx, NULL);
    printf("[MIDDLEWARE - Logging]: End\n");
}

