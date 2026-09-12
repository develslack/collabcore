#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "frontend_server.h"

#define BUFFER_SIZE 4096

static char frontend_root[512]; // ruta global donde están los HTML

// Función auxiliar para servir archivos estáticos
void serve_file(int client_socket, const char *file_path) {

    char full_path[1024];

    // Normalizar concatenación sin duplicar slashes
    if (frontend_root[strlen(frontend_root) - 1] == '/') {
        snprintf(full_path, sizeof(full_path), "%s%s", frontend_root, file_path);
    } else {
        snprintf(full_path, sizeof(full_path), "%s/%s", frontend_root, file_path);
    }

    printf("[DEBUG] Buscando archivo: %s\n", full_path);

    // Intentar abrir el archivo solicitado
    FILE *file = fopen(full_path, "rb");

    // Si no lo encuentra, intentar buscar dentro de frontend_root padre (p.ej. skeleton/js)
    if (!file) {
        snprintf(full_path, sizeof(full_path), "%s/../%s", frontend_root, file_path);
        file = fopen(full_path, "rb");
    }

    if (!file) {
        printf("[ERROR] No se pudo abrir %s\n", full_path);

        const char *not_found =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n\r\n"
            "404 Not Found";
        write(client_socket, not_found, strlen(not_found));
        close(client_socket);
        return;
    }

    // Detectar tipo MIME por extensión
    const char *ext = strrchr(file_path, '.');
    const char *mime = "text/plain";

    if (ext) {

        if(strcmp(ext, ".html") == 0){
          mime = "text/html";
        }else if(strcmp(ext, ".css") == 0){
            mime = "text/css";
        }else if(strcmp(ext, ".js") == 0){
            mime = "application/javascript";
        }else if(strcmp(ext, ".png") == 0){
            mime = "image/png";
        }else if(strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0){
            mime = "image/jpeg";
        }else if(strcmp(ext, ".ico") == 0){
            mime = "image/x-icon";
        }
    }

    // Enviar cabecera HTTP
    char response_header[256];
    snprintf(response_header, sizeof(response_header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: %s\r\n"
             "Connection: close\r\n\r\n", mime);

    write(client_socket, response_header, strlen(response_header));

    // Enviar contenido del archivo
    char buffer[BUFFER_SIZE];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        write(client_socket, buffer, bytes);
    }

    fclose(file);
    close(client_socket);

}

// Handler de cada cliente
void *handle_client(void *arg) {

    int client_socket = *(int *)arg;
    free(arg);

    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    read(client_socket, buffer, sizeof(buffer) - 1);

    // Obtener la ruta solicitada
    char method[8], path[256];
    sscanf(buffer, "%s %s", method, path);

    printf("[FRONTEND] Request: %s %s\n", method, path);

    // Si pide raíz o /index.html => servir index.html
    if (strcmp(path, "/") == 0 || strstr(path, "index.html")) {
        serve_file(client_socket, "index.html");
    }
    // Si pide /dashboard.html => servir dashboard.html
    else if (strstr(path, "dashboard.html")) {
        serve_file(client_socket, "dashboard.html");
    }
    else {
        // Para cualquier otro recurso (js/css/img)
        // eliminar el '/' inicial y servir relativo al frontend_root
        const char *relative_path = path[0] == '/' ? path + 1 : path;
        serve_file(client_socket, relative_path);
    }

    return NULL;

}

// Servidor web del frontend
void *frontend_server_thread(void *arg) {

    int port = *(int *)arg;

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server_addr;

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_socket, 10);

    printf(FRONTEND_AZUL "[ FRONTEND SERVER ]" FRONTEND_NORMAL FRONTEND_VERDE " Listen on http://localhost:%d\n"FRONTEND_NORMAL, port);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int *client_socket = malloc(sizeof(int));
        *client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, client_socket);
        pthread_detach(tid);
    }

    close(server_socket);
    return NULL;
}

// 🔹 Inicializador del frontend (llamado desde main.c)
void start_frontend_server(int port, const char *root_path) {

    strncpy(frontend_root, root_path, sizeof(frontend_root) - 1);
    frontend_root[sizeof(frontend_root) - 1] = '\0';

    pthread_t tid;
    pthread_create(&tid, NULL, frontend_server_thread, &port);
    pthread_detach(tid);
}
