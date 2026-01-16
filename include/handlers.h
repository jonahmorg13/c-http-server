#pragma once
#include "server.h"

void students_handler(HttpRequest* req, HttpResponse* res);
void login_handler(HttpRequest* req, HttpResponse* res);
void static_file_handler(HttpRequest* req, HttpResponse* res);