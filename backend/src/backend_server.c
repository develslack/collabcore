#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "routes.h"
#include "backend_server.h"


void start_backend(int port){

    int server_fd, client;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    char buffer[BUFFER_SIZE];

    // socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(1);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(1);
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        exit(1);
    }

    printf(BACKEND_AMARILLO "[ BACKEND SERVER ]" BACKEND_NORMAL BACKEND_VERDE " Listen on http://localhost:%d\n"BACKEND_NORMAL, PORT);

    init_routes();

    while (1) {
        client = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (client < 0) {
            perror("accept failed");
            continue;
        }

        memset(buffer, 0, sizeof(buffer));

        // 1. Leemos el primer fragmento
        int bytes_read = read(client, buffer, sizeof(buffer) - 1);
        if (bytes_read <= 0) {
            close(client);
            continue;
        }

        char method[8] = {0}, path[128] = {0};
        sscanf(buffer, "%7s %127s", method, path);

        char *body_start = strstr(buffer, "\r\n\r\n");

        if (body_start) {
            body_start += 4; // Nos movemos exactamente donde empieza el body

            int content_length = 0;
            char *cl_ptr = strstr(buffer, "Content-Length: ");
            if (cl_ptr) {
                content_length = atoi(cl_ptr + 16);
            }

            int header_len = body_start - buffer;
            int initial_body_read = bytes_read - header_len;

            if (content_length > 0) {
                char *full_body = (char *)malloc(content_length + 1);

                if (full_body) {
                    memcpy(full_body, body_start, initial_body_read);
                    int total_read = initial_body_read;

                    while (total_read < content_length) {
                        int r = read(client, full_body + total_read, content_length - total_read);
                        if (r <= 0) break;
                        total_read += r;
                    }
                    full_body[total_read] = '\0';

                    // 🚀 NUEVO: Pasamos content_length al enrutador
                    handle_request(client, method, path, full_body, content_length);

                    free(full_body);
                } else {
                    // Fallback sin memoria
                    handle_request(client, method, path, "", 0);
                }
            } else {
                // 🚀 NUEVO: Peticiones normales que caben en el buffer inicial
                handle_request(client, method, path, body_start, initial_body_read);
            }
        } else {
            // Petición GET vacía
            handle_request(client, method, path, "", 0);
        }

        close(client);
    }

} // END OF FUNCTION

