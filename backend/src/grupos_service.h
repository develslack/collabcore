#ifndef GRUPOS_SERVICE_H
#define GRUPOS_SERVICE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ArrayList.h"

// Estructura para la caché en RAM
typedef struct {
    int id;
    char nombre[100];
    char nombre_directorio[100];
    int id_creador;
    char fecha_creacion[20]; // Formato YYYY-MM-DD HH:MM:SS
    char nombre_creador[100];
} Grupo;

Grupo* newGrupo();
void grupos_init_cache(ArrayList* alistGrupos);
void grupos_load_storage(ArrayList* alistGrupos);

int grupos_service_register(const char *body, char *error_msg, int error_size);
int grupos_service_edit(const char *body, char *error_msg, int error_size);
int get_grupos_service_id(const char *body, char *json_out, int out_size);

void init_grupos_routes();

#endif
