#include "telemetry.h"
#include "generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <sys/time.h>

extern GeneratorState gen_state;
extern pthread_mutex_t lock;
extern int keep_running;

static int server_fd = -1;
static pthread_t telemetry_thread;

void* telemetry_worker(void* arg) {
    (void)arg;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    while (keep_running) {
        int client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_socket < 0) {
            usleep(100000);
            continue;
        }

        // Set 1-second timeout to prevent socket hanging
        struct timeval tv;
        tv.tv_sec = 1;
        tv.tv_usec = 0;
        setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

        char request_buffer[1024] = {0};
        read(client_socket, request_buffer, sizeof(request_buffer) - 1);

        // Serve HTML UI on root or explicit request
        if (strstr(request_buffer, "GET / ") != NULL || strstr(request_buffer, "GET /Natska_PPC.html") != NULL) {
            FILE *f = fopen("web/Natska_PPC.html", "r");
            if (f != NULL) {
                fseek(f, 0, SEEK_END);
                long fsize = ftell(f);
                fseek(f, 0, SEEK_SET);

                char *html_content = malloc(fsize + 1);
                if (html_content != NULL) {
                    fread(html_content, 1, fsize, f);
                    html_content[fsize] = '\0';

                    char http_header[512];
                    int header_len = snprintf(http_header, sizeof(http_header),
                        "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n"
                        "Content-Length: %ld\r\n"
                        "Connection: close\r\n\r\n", fsize);

                    write(client_socket, http_header, header_len);
                    write(client_socket, html_content, fsize);
                    free(html_content);
                }
                fclose(f);
            } else {
                char *not_found = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nError: web/Natska_PPC.html not found relative to execution directory.";
                write(client_socket, not_found, strlen(not_found));
            }
        } else {
            // Serve JSON Telemetry for background fetch polling
            char json_buf[512];
            pthread_mutex_lock(&lock);
            generator_to_json(&gen_state, json_buf, sizeof(json_buf));
            pthread_mutex_unlock(&lock);

            char http_response[1024];
            snprintf(http_response, sizeof(http_response),
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: application/json\r\n"
                "Access-Control-Allow-Origin: *\r\n"
                "Connection: close\r\n\r\n"
                "%s", json_buf);

            write(client_socket, http_response, strlen(http_response));
        }

        close(client_socket);
    }
    return NULL;
}

int telemetry_server_init(int port) {
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) return -1;

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 5) < 0) {
        close(server_fd);
        return -1;
    }

    pthread_create(&telemetry_thread, NULL, telemetry_worker, NULL);
    return 0;
}

void telemetry_server_stop(void) {
    if (server_fd >= 0) {
        close(server_fd);
        server_fd = -1;
    }
    pthread_join(telemetry_thread, NULL);
}