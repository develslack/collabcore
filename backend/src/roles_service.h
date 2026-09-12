#ifndef ROLES_SERVICE_H
#define ROLES_SERVICE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ArrayList.h"

typedef struct{
    int id;
    char descripcion[100];
}Roles;

// CONSTRUCTOR
Roles* new_role();

// static void get_roles_value(const char *body, const char *key, char *out, size_t out_size);

// FIRMAS DE LOGICA
void roles_init_cache(ArrayList* alist_roles);
void roles_load_storage(ArrayList* alist_roles);
int roles_service_register(const char *body, char *error_msg, int error_size);
int roles_service_edit(const char *body, char *error_msg, int error_size);
int get_roles_service_id(const char *body, char *json_out, int out_size);


// FIRMAS DE ROUTES
// static void route_get_roles_list(int client, const char *body);
// static void route_post_roles_edit(int client, const char *body);
// static void route_get_roles_by_id(int client, const char *body);

// FIRMA ROUTES GENERALES
void init_roles_routes();

#endif

