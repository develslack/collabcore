#ifndef INTEGRANTES_SERVICE_H
#define INTEGRANTES_SERVICE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ArrayList.h"

// ====================================================================== //
// ESTRUCTURA DE LA CACHÉ RAM
// ====================================================================== //
// Nota: Solo mantenemos en RAM a los miembros activos para ahorrar memoria.
typedef struct {
    int id;
    int id_grupo;
    int id_usuario;
    int es_coordinador;
    char fecha_ingreso[20];
    char nombre_usuario[100];
    char nombre_grupo[100];
} Miembro;

// ====================================================================== //
// PROTOTIPOS DE FUNCIONES (Memoria y Arranque)
// ====================================================================== //
Miembro* newMiembro();
void integrantes_init_cache(ArrayList* alistIntegrantes);
void integrantes_load_storage(ArrayList* alistIntegrantes);

// ====================================================================== //
// PROTOTIPOS DE SERVICIOS (Lógica de Negocios)
// ====================================================================== //
// Requieren el id_usuario_peticion para validar la barrera de seguridad internamente
int integrantes_service_add(const char *body, int id_usuario_peticion, char *error_msg, int error_size);
int integrantes_service_remove(const char *body, int id_usuario_peticion, char *error_msg, int error_size);

// ====================================================================== //
// ENRUTADOR
// ====================================================================== //
void init_integrantes_routes();

#endif
