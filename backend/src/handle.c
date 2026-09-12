#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "handle.h"



void handle_request(int new_socket) {
    char buffer[BUFFER_SIZE];
    int valread = read(new_socket, buffer, BUFFER_SIZE - 1);
    if (valread <= 0) return;
    buffer[valread] = '\0';

    // Solo acepta POST /login
    if (strstr(buffer, "POST /login") != NULL) {
        char *json_start = strstr(buffer, "\r\n\r\n");
        if (!json_start) return;

        json_start += 4; // Saltar "\r\n\r\n"
        const char *response = auth_handle_login(json_start);
        char header[256];
        snprintf(header, sizeof(header),
                 "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: %ld\r\n\r\n",
                 strlen(response));
        write(new_socket, header, strlen(header));
        write(new_socket, response, strlen(response));
    } else {
        const char *not_found = "HTTP/1.1 404 Not Found\r\n\r\nRuta no encontrada\n";
        write(new_socket, not_found, strlen(not_found));
    }
}
