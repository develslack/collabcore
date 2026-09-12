#ifndef USERS_SERVICE_H
#define USERS_SERVICE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define TOKEN_LENGTH 128

typedef struct {
    int  id;
    char nombre[128];
    char email[128];
    int  rol_id;
    char* session_token;
} UserData;


void get_value(const char *body, const char *key, char *out, size_t out_size);

// Devuelve 1 si se registró OK, 0 si falló
// error_msg puede contener el motivo en caso de fallo
int users_service_register(const char *body, char *error_msg, int error_size);
int users_service_login(const char *email, const char *password, char *error_msg, UserData *user_out);
//int users_service_login(const char *email, const char *password, char *error_msg);
int users_service_password(const char *body, char *error_msg, int error_size);
int users_service_get_rol(int client, const char *body); // buscamos un usuario por id y devuelve el rol
int users_service_update_rol(const char* body, char* error_msg, int error_size); // se actualiza el rol de un usuario por id
int get_user_service_name(const char *body, char *json_out, int out_size); // buscamos un registro por id en la tabla dm_usuarios
int get_user_service_id(const char *body, char *json_out, int out_size); // buscamos un usuario por id

// static void route_get_users_list(int client, const char *body);
// static void route_get_users_rol(int client, const char *body);
// static void route_users_update_rol(int client, const char* body);
// static void users_service_list(int client, const char *body);
// static void route_get_user_by_name(int client, const char *body);
// static void route_get_user_by_id(int client, const char *body);
//
// static void append_str(char **dest, size_t *size, const char *src);
// static char *json_escape(const char *src);

// FUNCIONES PARA LOGIN

char* tokenGenerator();

// static void route_get_login(int client, const char *body);
// static void route_post_login(int client, const char *body);

void init_users_routes();

#endif

