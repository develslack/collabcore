#ifndef COMMONLIB_H
#define COMMONLIB_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>

// Helper para extraer datos de un POST normal
void get_body_value(const char *body, const char *key, char *out, size_t out_size);

// Motor Universal de Subida de Archivos (Estilo $_FILES de PHP - Seguro para Binarios)
// Retorna 1 si tuvo éxito, 0 si falló.
int capturar_archivo_upload(const char *body, size_t body_len, const char *directorio_destino, char *nombre_archivo_salida, size_t max_len);

#endif
