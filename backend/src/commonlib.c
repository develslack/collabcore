#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include "commonlib.h"

// ========================================================================= //
// HELPER PRIVADO: Búsqueda binaria segura (No se detiene con bytes nulos \0)
// ========================================================================= //
static const char *mem_search(const char *haystack, size_t haystack_len, const char *needle, size_t needle_len) {
    if (needle_len == 0) return haystack;
    if (haystack_len < needle_len) return NULL;
    for (size_t i = 0; i <= haystack_len - needle_len; i++) {
        if (memcmp(haystack + i, needle, needle_len) == 0) {
            return haystack + i;
        }
    }
    return NULL;
}

// ========================================================================= //
// FUNCIÓN AUXILIAR: Obtiene valor de key=valor en el body
// ========================================================================= //
void get_body_value(const char *body, const char *key, char *out, size_t out_size) {
    char *pos = strstr(body, key);
    if (!pos) {
        out[0] = '\0';
        return;
    }
    pos += strlen(key);
    if (*pos == '=') pos++;
    const char *end = strchr(pos, '&');
    size_t len = end ? (size_t)(end - pos) : strlen(pos);
    if (len >= out_size) len = out_size - 1;
    strncpy(out, pos, len);
    out[len] = '\0';
}

// ========================================================================= //
// MOTOR UNIVERSAL DE ARCHIVOS: Soporta .txt, .sir, .png, .pdf, .jpg, etc.
// ========================================================================= //
int capturar_archivo_upload(const char *body, size_t body_len, const char *directorio_destino, char *nombre_archivo_salida, size_t max_len) {
    printf("\n--- 🔍 DEBUG UPLOAD MULTIPART ---\n");
    printf("📊 Bytes totales recibidos en el motor: %zu\n", body_len);

    if (!body || body_len == 0) {
        printf("❌ ERROR: El body es nulo o su longitud es 0.\n");
        return 0;
    }

    nombre_archivo_salida[0] = '\0';

    // 1. Extraer el nombre original
    const char *fn_pos = mem_search(body, body_len, "filename=\"", 10);
    if (fn_pos) {
        fn_pos += 10;
        const char *fn_end = mem_search(fn_pos, body_len - (fn_pos - body), "\"", 1);
        if (fn_end) {
            size_t l = fn_end - fn_pos;
            if (l >= max_len) l = max_len - 1;
            memcpy(nombre_archivo_salida, fn_pos, l);
            nombre_archivo_salida[l] = '\0';
            printf("📝 Nombre detectado: '%s'\n", nombre_archivo_salida);
        } else {
            printf("⚠️ ALERTA: Se encontró 'filename=\"' pero no las comillas de cierre.\n");
        }
    } else {
        printf("❌ ERROR: No se encontró la etiqueta 'filename=\"' en el body.\n");
        // Imprimir los primeros 200 caracteres para ver qué formato está llegando realmente
        printf("👀 Muestra del body recibido:\n%.200s\n", body);
        return 0;
    }

    if (strlen(nombre_archivo_salida) == 0) {
        printf("❌ ERROR: El nombre del archivo extraído está vacío.\n");
        return 0;
    }

    // 2. Aislar el contenido binario real (después del doble salto de línea HTTP)
    const char *data_start = mem_search(body, body_len, "\r\n\r\n", 4);
    if (data_start) {
        data_start += 4;
        printf("📍 Delimitador '\\r\\n\\r\\n' encontrado.\n");
    } else {
        data_start = mem_search(body, body_len, "\n\n", 2);
        if (data_start) {
            data_start += 2;
            printf("📍 Delimitador '\\n\\n' encontrado.\n");
        } else {
            printf("❌ ERROR: No se encontró el inicio de los datos binarios (doble salto de línea).\n");
            return 0;
        }
    }

    // 3. Determinar el "boundary" (El separador generado por el navegador)
    char boundary[128] = {0};
    const char *first_crlf = mem_search(body, body_len, "\r\n", 2);
    if (first_crlf) {
        size_t b_len = first_crlf - body;
        if (b_len < sizeof(boundary)) {
            memcpy(boundary, body, b_len);
            boundary[b_len] = '\0';
            printf("🔗 Boundary detectado: '%s'\n", boundary);
        }
    }

    // 4. Buscar dónde termina exactamente el archivo binario
    size_t payload_len = body_len - (data_start - body);

    if (strlen(boundary) > 0) {
        char end_marker[130];
        snprintf(end_marker, sizeof(end_marker), "\r\n%s", boundary);
        size_t marker_len = strlen(end_marker);

        const char *b_pos = mem_search(data_start, payload_len, end_marker, marker_len);
        if (b_pos) {
            payload_len = b_pos - data_start;
            printf("✂️ Recorte aplicado. Tamaño neto del binario: %zu bytes\n", payload_len);
        } else {
            printf("⚠️ ALERTA: No se encontró el boundary final. Guardando hasta el EOF (%zu bytes).\n", payload_len);
        }
    }

    // 5. Guardar físicamente en disco
    char path_final[512];
    snprintf(path_final, sizeof(path_final), "%s/%s", directorio_destino, nombre_archivo_salida);

    FILE *f = fopen(path_final, "wb");
    if (!f) {
        printf("❌ ERROR CRÍTICO: No se pudo abrir '%s' para escritura. Revisa permisos.\n", path_final);
        return 0;
    }

    size_t bytes_written = fwrite(data_start, 1, payload_len, f);
    fclose(f);

    if (bytes_written != payload_len) {
        printf("⚠️ ALERTA: Se intentaron escribir %zu bytes, pero solo se escribieron %zu.\n", payload_len, bytes_written);
    }

    printf("✅ [FRAMEWORK UPLOAD] Archivo guardado con éxito: %s (%zu bytes)\n", path_final, payload_len);
    printf("----------------------------------\n\n");
    return 1;
}
