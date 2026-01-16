#include <handlers.h>
#include <server.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void students_handler(HttpRequest* req, HttpResponse* res) {
    char *response = 
    "HTTP/1.1 200 OK\r\n"
    "Server: jonahsServer/1.0\r\n"
    "Content-Type: text/html; charset=UTF-8\r\n"
    "Content-Length: 177\r\n"
    "Connection: close\r\n"
    "\r\n"
    "<!DOCTYPE html>\n"
    "<html>\n"
    "<head><title>My C Server</title></head>\n"
    "<body>\n"
    "    <h1>Student 1</h1>\n"
    "    <p>Name: Jonah</p>\n"
    "    <h1>Student 2</h1>\n"
    "    <p>Name: Jada</p>\n"
    "</body>\n"
    "</html>\n";

    res->body = strdup(response);
    res->length = strlen(res->body);
}

void login_handler(HttpRequest* req, HttpResponse* res) {
    printf("we have hit our login function handler!\n");

    char *response = 
    "HTTP/1.1 200 OK\r\n"
    "Server: jonahsServer/1.0\r\n"
    "Content-Type: text/plain; charset=UTF-8\r\n"
    "Content-Length: 28\r\n"
    "Connection: close\r\n"
    "\r\n"
    "Password is SECRET_PASSWORD\n";

    res->body = strdup(response);
    res->length = strlen(res->body);
}

void static_file_handler(HttpRequest* req, HttpResponse* res) {
    size_t path_len = strlen(STATIC_FILE_DIR) + strlen(req->header->path) + 1;
    char* full_path = (char*)malloc(path_len);
    if(full_path == NULL) return;

    snprintf(full_path, path_len, "%s%s", STATIC_FILE_DIR, req->header->path);

    int fd = open(full_path, O_RDONLY);
    if(fd == -1) {
        res->body = strdup("HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n");
        res->length = strlen(res->body);
        free(full_path);
        return;
    }

    struct stat st;
    fstat(fd, &st);
    off_t file_size = st.st_size;

    char* file_content = (char*)malloc(file_size + 1);
    if(file_content == NULL) {
        close(fd);
        free(full_path);
        return;
    }

    ssize_t bytes_read = read(fd, file_content, file_size);
    if(bytes_read >= 0) {
        file_content[bytes_read] = '\0';
    }

    size_t full_response_len = file_size + 256;
    res->body = malloc(full_response_len);
    
    if(res->body != NULL) {
        int written = snprintf(res->body, full_response_len,
            "HTTP/1.1 200 OK\r\n"
            "Server: jonahsServer/1.0\r\n"
            "Content-Type: text/html; charset=UTF-8\r\n"
            "Content-Length: %ld\r\n"  // Use %ld for off_t
            "Connection: close\r\n"
            "\r\n"
            "%s",
            file_size, file_content);
        
        res->length = written;
    }

    free(file_content);
    free(full_path);
    close(fd);
}