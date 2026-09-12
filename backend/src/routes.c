#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <stddef.h>
#include "routes.h"
#include "auth.h"
#include "users.h"
#include "users_service.h"
#include "roles_service.h"
#include "session_manager.h"


#define MAX_ROUTES 500
#define BUFFER_SIZE 4096

typedef struct {
    char method[8];
    char path[128];
    RouteHandler handler;
} Route;

static Route routes[MAX_ROUTES];
static int route_count = 0;

// =========================================================================
// NUEVO: ESTADO GLOBAL PARA EL TAMAÑO DEL BODY (No rompe las firmas actuales)
// =========================================================================
static size_t g_current_body_len = 0;

size_t get_current_body_len() {
    return g_current_body_len;
}
// =========================================================================

// Helpers comunes
void send_response(int client, const char *status, const char *content_type, const char *body) {

    char header[BUFFER_SIZE];
    snprintf(header, sizeof(header),
             "HTTP/1.1 %s\r\n"
             "Access-Control-Allow-Origin: *\r\n"
             "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
             "Access-Control-Allow-Headers: Content-Type\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %zu\r\n"
             "\r\n",
             status, content_type, strlen(body));

    write(client, header, strlen(header));
    write(client, body, strlen(body));


}


// ===================================================================================================================== //
// FUNCIÓN CENTRAL DE LOGUEO (Escribe en archivo y en consola)
// ===================================================================================================================== //
void write_log(const char *level, const char *message) {
    // Abrimos en modo "append" (a-añadir) para no pisar los registros anteriores
    FILE *log_file = fopen("server.log", "a");
    if (log_file == NULL) {
        printf("🚨 [ERROR]: No se pudo abrir o crear el archivo server.log\n");
        return;
    }

    // Obtenemos la hora del sistema para el timestamp
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);

    // Escribimos en el archivo de texto
    fprintf(log_file, "[%s] [%s]: %s\n", timestamp, level, message);
    fclose(log_file);

    // También lo mostramos en la consola para desarrollo visual
    if (strcmp(level, "ERROR") == 0 || strcmp(level, "CRITICAL") == 0) {
        printf("🚨 [%s]: %s\n", level, message);
    } else {
        printf("ℹ️ [%s]: %s\n", level, message);
    }
}

// ===================================================================================================================== //
// REPORTE FINAL DE RUTAS CARGADAS (El Dump)
// ===================================================================================================================== //
void dump_routes_to_log() {
    char buffer[256];

    write_log("INFO", "=== INICIANDO DETALLE DE MAPEO DE ENTRADAS (ROUTER) ===");

    for (int i = 0; i < route_count; i++) {
        snprintf(buffer, sizeof(buffer), "Slot #%03d -> Método: [%-6s] | Path: [%s]",
                 i + 1, routes[i].method, routes[i].path);
        write_log("SUCCESS", buffer);
    }

    snprintf(buffer, sizeof(buffer), "=== TOTAL DE RUTAS MAPEADAS EN MEMORIA: [%d/%d] ===", route_count, MAX_ROUTES);
    write_log("INFO", buffer);
}

void add_route(const char *method, const char *path, RouteHandler handler) {

    if (route_count < MAX_ROUTES) {
        // Copia el método limitando al tamaño de method (8 bytes)
        strncpy(routes[route_count].method, method, sizeof(routes[route_count].method) - 1);
        routes[route_count].method[sizeof(routes[route_count].method) - 1] = '\0'; // Asegurar cierre

        // 🔥 CORREGIDO: Ahora limitamos al tamaño real de path (128 bytes)
        strncpy(routes[route_count].path, path, sizeof(routes[route_count].path) - 1);
        routes[route_count].path[sizeof(routes[route_count].path) - 1] = '\0'; // Asegurar cierre

        routes[route_count].handler = handler;
        route_count++;
    }
    else {
        printf("🚨 [ALERTA CRÍTICA]: Se superó MAX_ROUTES (%d). No se pudo registrar %s %s\n", MAX_ROUTES, method, path);
    }
}

// Inicializa todas las rutas de los módulos
void init_routes() {

    write_log("INFO", "Iniciando secuencia de vinculación de módulos...");

    init_session_routes();
    init_auth_routes();
    init_users_routes();
    init_user_routes();
    //init_login_routes();
    init_roles_routes();
    // HASTA AQUI RUTAS BÁSICAS DEL SISTEMA



    // 🔥 EJECUTAMOS EL REPORTE: Una vez que todos los módulos se registraron, volcamos el estado
    dump_routes_to_log();

}

// Router central
// Router central (AHORA RECIBE EL body_len)
void handle_request(int client, const char *method, const char *path, const char *body, size_t body_len) {

    // Guardamos la longitud exacta para cualquier módulo binario que la pida
    g_current_body_len = body_len;

    // --- Soporte global para CORS y método OPTIONS ---
    if (strcmp(method, "OPTIONS") == 0) {
            char *options_response =
            "HTTP/1.1 204 No Content\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n"
            "Access-Control-Allow-Headers: Content-Type\r\n"
            "\r\n";
        write(client, options_response, strlen(options_response));
        close(client);
        return;
    }
    // -------------------------------------------------- //

    for (int i = 0; i < route_count; i++) {
        if (strcmp(method, routes[i].method) == 0 && strcmp(path, routes[i].path) == 0) {
            // ¡MAGIA!: Se sigue llamando a las rutas de la misma forma de siempre. No se rompe nada.
            routes[i].handler(client, body);
            return;
        }
    }

    send_response(client, "404 Not Found", "application/json", "{ \"error\": \"Ruta no encontrada\" }");

} // END OF FUNCTION
