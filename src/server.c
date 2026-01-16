#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <dirent.h>

#include <sys/time.h>

#include <netinet/in.h>
#include <sys/socket.h>

#include "server.h"
#include "radix.h"

char* not_found_response = 
    "HTTP/1.1 404 Not Found\r\n"       
    "Content-Type: text/plain\r\n"     
    "Content-Length: 14\r\n"           
    "Connection: close\r\n"            
    "\r\n"                             
    "404 Not Found\n";                 

HttpServer* http_server_create(uint16_t port) {
    HttpServer* server = (HttpServer*)malloc(sizeof(HttpServer));
    if(server == NULL) {
        fprintf(stderr, "Error creating space on the heap for the server.");
        exit(EXIT_FAILURE);
    }

    server->tree = radix_tree_create();
    assert(server->tree != NULL);
    assert(server->tree->root != NULL);

    server->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(server->sockfd == -1) {
        fprintf(stderr, "Error creating file descriptor for the server.");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server->sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);
    servAddr.sin_addr.s_addr = INADDR_ANY;

    if(bind(server->sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1 ) {
        fprintf(stderr, "Error binding server socket");
        exit(EXIT_FAILURE);
    }

    return server;
}

void http_server_run(HttpServer* server) {
    if(listen(server->sockfd, 1000) == -1) {
        fprintf(stderr, "Error listening on the server socket.");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in client_addr;
    socklen_t sin_size;
    int new_sockd;

    while(true) {
        sin_size = sizeof(struct sockaddr_in);
        new_sockd = accept(server->sockfd, (struct sockaddr*)&client_addr, &sin_size);
        if(new_sockd == -1) {
            fprintf(stderr, "Error accepting connection.");
            exit(EXIT_FAILURE);
        }

        handle_connection(server, new_sockd);
    }
}

void http_server_print_endpoints(HttpServer* server) {
    radix_tree_print(server->tree);
}

void http_server_delete(HttpServer* server) {
    radix_tree_free(server->tree);
    free(server);
}

void handle_connection(HttpServer* server, int sockfd)
{
    // only wait for 500ms on this connection
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 500000;

    if(setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
        perror("Error setting timeout");
    }

    size_t buffer_size = 16;
    char buffer[buffer_size];

    char header_buf[4096];

    HttpRequest* req = http_request_create(); 
    HttpResponse* res = http_response_create();

    // read the header from the socket
    int recv_length = recv(sockfd, buffer, buffer_size, 0);
    int search_idx = 0;
    char search[] = {'\r', '\n', '\r', '\n'};
    int offset = 0;
    int end_offset = 0;
    bool header_buffer_fill_finished = false;
    while(recv_length > 0) {
        for(int i = 0; i < recv_length; i++) {
            if(buffer[i] != search[search_idx]) {
                search_idx = 0;
            } 
            else {
                search_idx++;
                if(search_idx >= 4) {
                    end_offset = i + 1;
                    strncpy(header_buf + offset, buffer, end_offset);
                    header_buf[offset + end_offset] = '\0';
                    header_buffer_fill_finished = true;
                    break;
                }
            }
        }
        if(header_buffer_fill_finished) break;
        strncpy(header_buf + offset, buffer, recv_length);
        offset += recv_length;
        recv_length = recv(sockfd, buffer, buffer_size, 0);
    }

    parse_http_header(req->header, header_buf, strlen(header_buf));

    // read the body from the socket
    size_t amount_to_read_left = req->header->content_length;
    req->data = (uint8_t*)malloc(sizeof(uint8_t) * amount_to_read_left);
    if(amount_to_read_left > 0) {
        // ok now that we're parsing the http header, lets read it's body
        int body_offset = 0;
        if(end_offset != recv_length) {
            int length = buffer_size - end_offset;
            strncpy(req->data, buffer + end_offset, length);
            body_offset = length;
            amount_to_read_left -= length;
        }
        recv_length = recv(sockfd, buffer, buffer_size, 0);
        if(amount_to_read_left > 0) {
            while(recv_length > 0) {
                strncpy(req->data + body_offset, buffer, recv_length);
                body_offset += recv_length;
                amount_to_read_left -= recv_length;

                if(amount_to_read_left <= 0) break;

                recv_length = recv(sockfd, buffer, buffer_size, 0);
            }
        }

        printf("Body: %s\n", req->data);
        printf("End Body");
    }

    size_t full_search_len = strlen(req->header->method) + strlen(req->header->path) + 1;
    char* full_search_path = (char*)malloc(full_search_len);

    snprintf(full_search_path, full_search_len, "%s%s", req->header->method, req->header->path);

    RadixTreeSearchResult* endpoint_search_result = (RadixTreeSearchResult*)http_server_endpoint_search(server, full_search_path);
    free(full_search_path);

    if(endpoint_search_result == NULL) {
        printf("%s", "sending back error resposne\n");
        res->body = strdup(not_found_response);
        res->length = strlen(res->body);
        if(send(sockfd, res->body, res->length, 0) == -1) {
            fprintf(stderr, "Error sending our resposne\n");
        }
    }
    else {
        run_middleware_and_func_handler(req, res, endpoint_search_result->group->middleware, endpoint_search_result->func);
        if(send(sockfd, res->body, res->length, 0) == -1) {
            fprintf(stderr, "Error sending our response\n");
        }
    }

    http_request_delete(req);
    http_response_delete(res);

    close(sockfd);
}

void parse_http_header(HttpHeader* header, char* buffer, size_t length) {
    char method[16], path[1024], version[16];
    if(sscanf(buffer, "%15s %1023s %15s", method, path, version) == 3) {
        header->method = strdup(method);
        header->path = strdup(path);
        header->version = strdup(version);
    }

    char* content_length = strstr(buffer, "Content-Length:");
    if(content_length == NULL)
        header->content_length = 0;
    else 
        header->content_length = atoi(content_length+15);

    char* authorization = strstr(buffer, "Authorization:");
    if(authorization != NULL) {
        authorization += 15;
        char* end = strstr(authorization, "\r\n");
        if(end != NULL) {
            size_t length = end - authorization;
            header->authorization = malloc(length + 1);
            memcpy(header->authorization, authorization, length);
            header->authorization[length] = '\0';
        }
    }
    else
        header->authorization = strdup("");

    char* connection = strstr(buffer, "Connection:");
    if(connection == NULL) {
        header->connection = strdup("close");
    }
    else {
        connection = connection + 12;
        if(strncasecmp(connection, "keep-alive", 10) == 0)
            header->connection = strdup("keep-alive");
        else
            header->connection = strdup("close");
    }

    return;
}

HttpRequest* http_request_create(void) {
    HttpRequest* req = (HttpRequest*)malloc(sizeof(HttpRequest));
    if(req == NULL) {
        fprintf(stderr, "Error mallocing http request");
        exit(EXIT_FAILURE);
    }

    req->header = (HttpHeader*)malloc((sizeof(HttpHeader)));
    if(req->header == NULL) {
        fprintf(stderr, "Error allocating the http header");
        exit(EXIT_FAILURE);
    }

    req->data = NULL;
    return req;
}

void http_request_delete(HttpRequest* req) {
    free(req->header);
    if(req->data != NULL) {
        free(req->data);
    }
    free(req);
}

HttpResponse* http_response_create(void) {
    HttpResponse* res = (HttpResponse*)malloc(sizeof(HttpResponse));
    if(res == NULL) {
        fprintf(stderr, "Error mallocing http response");
        exit(EXIT_FAILURE);
    }
    res->body = NULL;
    res->length = 0;
    return res;
}

void http_response_delete(HttpResponse* res) {
    if(res->body != NULL) {
        free(res->body);
    }
    free(res);
}

MiddlewareFunctionsVec* create_middleware_functions_vec(void) {
    MiddlewareFunctionsVec* vec = (MiddlewareFunctionsVec*)malloc(sizeof(MiddlewareFunctionsVec));
    if(vec == NULL) {
        perror("Error allocating memory");
        exit(-1);
    }
    vec->funcs = NULL;
    vec->cap = 0;
    vec->size = 0;

    return vec;
}

void middleware_functions_vec_push(MiddlewareFunctionsVec* vec, middleware_func_t func) {
    if(vec->funcs == NULL) {
        vec->cap = 4;
        vec->funcs = (middleware_func_t*)malloc(sizeof(middleware_func_t) * vec->cap);
        if(vec->funcs == NULL) {
            perror("Error allocating memory");
            exit(-1);
        }
    }
    if(vec->size >= vec->cap) {
        vec->cap *= 2;
        vec->funcs = (middleware_func_t*)realloc(vec->funcs, sizeof(middleware_func_t) * vec->cap);
        if(vec->funcs == NULL) {
            perror("Error allocating memory");
            exit(-1);
        }
    }
    vec->funcs[vec->size++] = func;
}

RouteGroup* create_route_group(HttpServer* server, char* prefix) {
    RouteGroup* group = (RouteGroup*)malloc(sizeof(RouteGroup));
    if(group == NULL) {
        perror("Error allocating route group");
        exit(-1);
    }


    group->middleware = create_middleware_functions_vec();
    group->server = server;
    group->prefix = prefix;

    return group;
}

void middleware_functions_free(MiddlewareFunctionsVec* vec) {
    free(vec->funcs);
}

RouteGroup* http_server_create_group(HttpServer* server, char* prefix) {
    RouteGroup* group = create_route_group(server, prefix);

    //todo add the group to a ds that the server can  hold onto

    return group;
}

void route_group_free(RouteGroup* group) {
    middleware_functions_free(group->middleware);
}

void route_group_map_endpoint(RouteGroup* group, char* method, char* endpoint, func_handler_t func) {
    const char* allowedMethods[] = {"GET", "POST", "DELETE", "PUT", "PATCH"};
    int methodCount = sizeof(allowedMethods) / sizeof(allowedMethods[0]);
    bool allowed = false;
    for(int i = 0; i < methodCount; i++) {
        if(strcmp(allowedMethods[i], method) == 0) {
            allowed = true;
            break;
        }
    }
    if(!allowed) {
        fprintf(stderr, "Not allowed to map %s\n", method);
        exit(-1);
    }

    size_t total_len = strlen(method) + strlen(group->prefix) + strlen(endpoint) + 1;
    char* full_path = malloc(total_len);
    if (!full_path) {
        perror("Failed to allocate path buffer");
        return; 
    }

    RadixTreeSearchResult* result = (RadixTreeSearchResult*)malloc(sizeof(RadixTreeSearchResult));
    if(result == NULL) {
        perror("Error allocating search result");
        exit(-1);
    }
    result->func = func;
    result->group = group;

    snprintf(full_path, total_len, "%s%s%s", method, group->prefix, endpoint);
    radix_tree_insert(group->server->tree, full_path, result);

    free(full_path);
}


radix_tree_element_t* http_server_endpoint_search(HttpServer* server, char* path) {
    RadixNode* node = radix_tree_get_node(server->tree, path);
    if(node == NULL || node->element == NULL) {
        return NULL;
    }

    return node->element; 
}

void route_group_use(RouteGroup* group, middleware_func_t func) {
    middleware_functions_vec_push(group->middleware, func);
}

void dispatch_handler(RequestHandlingContext* ctx, void* next) {
    if(ctx->curr_mw_idx < ctx->middleware_size) {
        middleware_func_t curr_middleware_func = ctx->middleware->funcs[ctx->curr_mw_idx++];
        curr_middleware_func(ctx, dispatch_handler);
    }
    else {
        ctx->func_handler(ctx->req, ctx->res);
    }
}

void run_middleware_and_func_handler(HttpRequest* req, HttpResponse* res, MiddlewareFunctionsVec* middleware, func_handler_t func) {
    RequestHandlingContext ctx = {
        .req = req,
        .res = res,
        .curr_mw_idx = 0,
        .middleware_size = middleware->size,
        .middleware = middleware,
        .func_handler = func
    };
    dispatch_handler(&ctx, NULL);
}