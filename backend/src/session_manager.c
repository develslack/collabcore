#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "session_manager.h"
#include "ArrayList.h"
#include "hash.h"
#include "routes.h"

// Puntero estático al ArrayList local de sesiones activas en RAM
static ArrayList* alist_sesiones_activas = NULL;

// Helper para limpiar espacios en blanco, saltos de línea y tabulaciones (Trimming)
static void trim_string(char *str) {
    if (!str) return;
    char *end;
    while(isspace((unsigned char)*str)) str++;
    if(*str == 0) return;
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
}

// Helper para extraer llaves de formularios POST en bruto
static void session_get_value(const char *body, const char *key, char *out, size_t out_size) {
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

// ===================================================================================================================== //
// GENERADOR DE TOKENS SEGUROS (Solo caracteres A-Z, a-z, 0-9)
// Evita bytes binarios, nulos y caracteres corruptos ()
// ===================================================================================================================== //
void generar_token_seguro(char *out_token, size_t length) {
    static int rand_inicializado = 0;
    if (!rand_inicializado) {
        srand((unsigned int)time(NULL));
        rand_inicializado = 1;
    }

    const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    size_t charset_size = sizeof(charset) - 1;

    for (size_t n = 0; n < length; n++) {
        int key = rand() % charset_size;
        out_token[n] = charset[key];
    }
    out_token[length] = '\0'; // Cierre limpio de cadena en C
}

// ===================================================================================================================== //
// ARRANQUE DEL KERNEL DE SESIONES
// ===================================================================================================================== //
void session_manager_init(void) {
    if (alist_sesiones_activas == NULL) {
        alist_sesiones_activas = al_newArrayList();
        printf("===================================================================================\n");
        printf("⚙️  [KERNEL]: session_manager inicializado en RAM (ArrayList listo para operar).\n");
    }
}

// ===================================================================================================================== //
// CREACIÓN DE SESIÓN POST-LOGIN (Inyección en RAM del Kernel)
// ===================================================================================================================== //
UserSession* session_manager_crear(const char* nombre, int rol_id, const char* token_login) {
    if (alist_sesiones_activas == NULL) return NULL;

    UserSession* nueva_sesion = (UserSession*)malloc(sizeof(UserSession));
    if (nueva_sesion == NULL) return NULL;

    // Vinculamos y saneamos el token
    strncpy(nueva_sesion->token, token_login, sizeof(nueva_sesion->token) - 1);
    nueva_sesion->token[sizeof(nueva_sesion->token) - 1] = '\0';
    trim_string(nueva_sesion->token);

    strncpy(nueva_sesion->nombre, nombre, sizeof(nueva_sesion->nombre) - 1);
    nueva_sesion->nombre[sizeof(nueva_sesion->nombre) - 1] = '\0';
    trim_string(nueva_sesion->nombre);

    nueva_sesion->rol_id = rol_id;

    // Configuración horizontal base del usuario
    nueva_sesion->ultimo_acceso = (long)time(NULL);

    // Guardamos de forma inmediata en nuestro ArrayList residente en RAM
    alist_sesiones_activas->add(alist_sesiones_activas, nueva_sesion);

    printf("🔑 [KERNEL - SESION]: Registro en RAM Exitoso -> Usuario: [%s] | Rol: [%d]\n",
           nueva_sesion->nombre, nueva_sesion->rol_id);

    return nueva_sesion;
}

// ===================================================================================================================== //
// ADUANA CENTRAL DE ACCESO (Validación ultra veloz contra RAM)
// ===================================================================================================================== //
UserSession* session_manager_validar(const char* token) {
    if (alist_sesiones_activas == NULL || token == NULL) return NULL;

    int total_usuarios = alist_sesiones_activas->len(alist_sesiones_activas);

    for (int i = 0; i < total_usuarios; i++) {
        UserSession* s = (UserSession*)alist_sesiones_activas->get(alist_sesiones_activas, i);

        if (strcmp(s->token, token) == 0) {
            s->ultimo_acceso = (long)time(NULL); // Actualizamos marca de actividad
            return s;
        }
    }
    return NULL; // Intrusión o sesión muerta
}

// ===================================================================================================================== //
// CONMUTACIÓN DE CONTEXTO ACTIVO (Sintonizador de Organismo, Lote y Período)
// ===================================================================================================================== //
/*
int session_manager_set_context(const char* token, const char* cod_inst_nuevo, const char* nro_lote_nuevo, const char* per_lote_nuevo) {
    UserSession* s = session_manager_validar(token);
    if (!s) {
        printf("❌ [KERNEL]: Falla de validacion de Token en RAM. Token buscando: [%s]\n", token);
        return 0;
    }

    // 🛡️ CERROJO DE SEGURIDAD HORIZONTAL: Si NO es administrador, el sistema le bloquea el cambio de organismo
    if (strcasecmp(s->cod_inst_base, "ALL") != 0) {
        strncpy(s->cod_inst_activo, s->cod_inst_base, sizeof(s->cod_inst_activo) - 1);
    } else if (cod_inst_nuevo != NULL && strlen(cod_inst_nuevo) > 0) {
        strncpy(s->cod_inst_activo, cod_inst_nuevo, sizeof(s->cod_inst_activo) - 1);
        s->cod_inst_activo[sizeof(s->cod_inst_activo) - 1] = '\0';
        trim_string(s->cod_inst_activo);
    }

    // Seteamos el lote activo
    if (nro_lote_nuevo != NULL && strlen(nro_lote_nuevo) > 0) {
        strncpy(s->nro_lote_activo, nro_lote_nuevo, sizeof(s->nro_lote_activo) - 1);
        s->nro_lote_activo[sizeof(s->nro_lote_activo) - 1] = '\0';
        trim_string(s->nro_lote_activo);
    }

    // Seteamos el período activo del lote
    if (per_lote_nuevo != NULL && strlen(per_lote_nuevo) > 0) {
        strncpy(s->per_lote_activo, per_lote_nuevo, sizeof(s->per_lote_activo) - 1);
        s->per_lote_activo[sizeof(s->per_lote_activo) - 1] = '\0';
        trim_string(s->per_lote_activo);
    }

    printf("🔄 [KERNEL - SESION]: Contexto actualizado -> User: [%s] | Org: [%s] | Lote: [%s] | Período: [%s]\n",
           s->nombre, s->cod_inst_activo, s->nro_lote_activo, s->per_lote_activo);

    return 1;
}
*/

// ===================================================================================================================== //
// LOGOUT (Destrucción y limpieza de RAM)
// ===================================================================================================================== //
void session_manager_destruir(const char* token) {
    if (alist_sesiones_activas == NULL || token == NULL) return;

    int total_usuarios = alist_sesiones_activas->len(alist_sesiones_activas);

    for (int i = 0; i < total_usuarios; i++) {
        UserSession* s = (UserSession*)alist_sesiones_activas->get(alist_sesiones_activas, i);

        if (strcmp(s->token, token) == 0) {
            alist_sesiones_activas->remove(alist_sesiones_activas, i);
            printf("🚪 [KERNEL - SESION]: Sesión liberada de RAM para el usuario [%s].\n", s->nombre);
            free(s);
            return;
        }
    }
}

// ===================================================================================================================== //
// ENDPOINT EXPUESTO AL FRONTEND PARA CONMUTAR CONTEXTOS
// ===================================================================================================================== //
/*
static void route_post_set_session_context(int client, const char* body) {
    char token[256], d_token[256];
    char cod_inst_nuevo[32], d_cod_inst_nuevo[32];
    char nro_lote_nuevo[32], d_nro_lote_nuevo[32];
    char per_lote_nuevo[32], d_per_lote_nuevo[32];

    session_get_value(body, "token", token, sizeof(token));
    url_decode(d_token, token);
    trim_string(d_token); // 🛡️ Saneamos espacios o saltos de línea al final del token

    session_get_value(body, "cod_inst", cod_inst_nuevo, sizeof(cod_inst_nuevo));
    url_decode(d_cod_inst_nuevo, cod_inst_nuevo);
    trim_string(d_cod_inst_nuevo);

    session_get_value(body, "nro_lote", nro_lote_nuevo, sizeof(nro_lote_nuevo));
    url_decode(d_nro_lote_nuevo, nro_lote_nuevo);
    trim_string(d_nro_lote_nuevo);

    session_get_value(body, "per_lote", per_lote_nuevo, sizeof(per_lote_nuevo));
    url_decode(d_per_lote_nuevo, per_lote_nuevo);
    trim_string(d_per_lote_nuevo);

    if (strlen(d_token) == 0) {
        send_response(client, "400 Bad Request", "application/json", "{\"status\":\"error\",\"message\":\"Token de sesión inválido o vacío\"}");
        return;
    }

    if (session_manager_set_context(d_token, d_cod_inst_nuevo, d_nro_lote_nuevo, d_per_lote_nuevo)) {
        // Devolvemos el objeto completo con la tripleta de datos y credenciales
        UserSession* s = session_manager_validar(d_token);
        char response_json[512];
        snprintf(response_json, sizeof(response_json),
                 "{\"status\":\"ok\",\"message\":\"Contexto de sesión actualizado en RAM\",\"user\":{"
                 "\"nombre\":\"%s\",\"token\":\"%s\",\"cod_inst\":\"%s\",\"cod_inst_activo\":\"%s\",\"lote_activo\":\"%s\",\"periodo_activo\":\"%s\"}}",
                 s->nombre, s->token, s->cod_inst_base, s->cod_inst_activo, s->nro_lote_activo, s->per_lote_activo);

        send_response(client, "200 OK", "application/json", response_json);
    } else {
        send_response(client, "401 Unauthorized", "application/json", "{\"status\":\"error\",\"message\":\"No se pudo actualizar el contexto (Sesión no encontrada en RAM)\"}");
    }
}
*/


// ===================================================================================================================== //
// ENDPOINT DE LOGOUT
// ===================================================================================================================== //
static void route_post_logout_session(int client, const char* body) {
    char token[256], d_token[256];

    session_get_value(body, "token", token, sizeof(token));
    url_decode(d_token, token);
    trim_string(d_token);

    if (strlen(d_token) > 0) {
        session_manager_destruir(d_token);
    }
    send_response(client, "200 OK", "application/json", "{\"status\":\"ok\",\"message\":\"Sesión de RAM destruida\"}");
}

// ===================================================================================================================== //
// REGISTRO DE RUTAS NATIVAS DE SESIÓN
// ===================================================================================================================== //
void init_session_routes(void) {
    //add_route("POST", "/session/set-context", route_post_set_session_context);
    add_route("POST", "/session/logout", route_post_logout_session);
}
