#ifndef ROUTES_H
#define ROUTES_H

#include <stddef.h>

typedef void (*RouteHandler)(int client, const char *body);

void write_log(const char *level, const char *message);
void dump_routes_to_log();

// Inicializa todas las rutas (llamado desde server.c)
void init_routes();

// Despacha la request a la ruta correspondiente
//void handle_request(int client, const char *method, const char *path, const char *body);
// Actualizar esta firma para que reciba el body_len
void handle_request(int client, const char *method, const char *path, const char *body, size_t body_len);

// Nueva función para que los módulos que suben archivos puedan consultar el tamaño
size_t get_current_body_len();

// Helpers globales para módulos
void send_response(int client, const char *status, const char *content_type, const char *body);
void add_route(const char *method, const char *path, RouteHandler handler);

#endif
