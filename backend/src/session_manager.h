#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <time.h>
#include "ArrayList.h"

// Estructura de control que reside exclusivamente en la RAM del servidor
typedef struct {

    char token[129];
    char nombre[100];
    int rol_id;
    long ultimo_acceso;       // Timestamp para control de inactividad

}UserSession;


void generar_token_seguro(char *out_token, size_t length);

static void trim_string(char *str);

/**
 * @brief Inicializa el ArrayList de sesiones activas en RAM.
 */
void session_manager_init(void);

/**
 * @brief Crea una nueva sesión en RAM tras un login exitoso y devuelve la estructura.
 */
UserSession* session_manager_crear(const char* nombre, int rol_id, const char* token_login);

/**
 * @brief Busca una sesión activa en el ArrayList mediante su token de 128 bytes.
 * @return Puntero a la sesión en RAM o NULL si no existe o expiró.
 */
UserSession* session_manager_validar(const char* token);

/**
 * @brief Modifica el contexto de trabajo dinámico (Organismo activo y Lote).
 */
//int session_manager_set_context(const char* token, const char* cod_inst_nuevo, const char* nro_lote_nuevo, const char* per_lote_nuevo);

/**
 * @brief Destruye la sesión en RAM (Logout).
 */
void session_manager_destruir(const char* token);

//static void route_post_set_session_context(int client, const char* body);
//static void route_post_logout_session(int client, const char* body);

/**
 * @brief Registra las rutas asociadas al session_manager en el enrutador central.
 */
void init_session_routes(void);

#endif // SESSION_MANAGER_H
