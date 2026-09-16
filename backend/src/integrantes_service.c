#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>
#include "hash.h"
#include "routes.h"
#include "db.h"
#include "ArrayList.h"
#include "integrantes_service.h"
#include "grupos_service.h" // Necesario para obtener el dueño del grupo

// Variable estática para la memoria local del módulo (Caché RAM)
static ArrayList* pListIntegrantesLocal = NULL;

// ===================================================================================================================================== //
// 🔒 STORED PROCEDURES: Prepared Statements (Integrantes)
// ===================================================================================================================================== //
static int sp_c_insertar_miembro(int id_grupo, int id_usuario, int es_coordinador) {
    MYSQL_STMT *stmt;
    MYSQL_BIND bind_param[3];
    const char *query = "CALL sp_insertar_miembro(?, ?, ?)";

    MYSQL *conn = connect_db();
    if (!conn) return 0;

    stmt = mysql_stmt_init(conn);
    if (!stmt || mysql_stmt_prepare(stmt, query, strlen(query))) {
        if (stmt) mysql_stmt_close(stmt);
        mysql_close(conn);
        return 0;
    }

    memset(bind_param, 0, sizeof(bind_param));

    bind_param[0].buffer_type = MYSQL_TYPE_LONG;
    bind_param[0].buffer = (void *)&id_grupo;
    bind_param[0].is_unsigned = 0;

    bind_param[1].buffer_type = MYSQL_TYPE_LONG;
    bind_param[1].buffer = (void *)&id_usuario;
    bind_param[1].is_unsigned = 0;

    bind_param[2].buffer_type = MYSQL_TYPE_LONG;
    bind_param[2].buffer = (void *)&es_coordinador;
    bind_param[2].is_unsigned = 0;

    if (mysql_stmt_bind_param(stmt, bind_param) || mysql_stmt_execute(stmt)) {
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return 0;
    }

    while (!mysql_stmt_next_result(stmt)) mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);
    mysql_close(conn);
    return 1;
}

static int sp_c_baja_miembro(int id_grupo, int id_usuario) {
    MYSQL_STMT *stmt;
    MYSQL_BIND bind_param[2];
    const char *query = "CALL sp_baja_miembro(?, ?)";

    MYSQL *conn = connect_db();
    if (!conn) return 0;

    stmt = mysql_stmt_init(conn);
    if (!stmt || mysql_stmt_prepare(stmt, query, strlen(query))) {
        if (stmt) mysql_stmt_close(stmt);
        mysql_close(conn);
        return 0;
    }

    memset(bind_param, 0, sizeof(bind_param));

    bind_param[0].buffer_type = MYSQL_TYPE_LONG;
    bind_param[0].buffer = (void *)&id_grupo;
    bind_param[0].is_unsigned = 0;

    bind_param[1].buffer_type = MYSQL_TYPE_LONG;
    bind_param[1].buffer = (void *)&id_usuario;
    bind_param[1].is_unsigned = 0;

    if (mysql_stmt_bind_param(stmt, bind_param) || mysql_stmt_execute(stmt)) {
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return 0;
    }

    while (!mysql_stmt_next_result(stmt)) mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt);
    mysql_close(conn);
    return 1;
}

// ===================================================================================================================================== //
// CONSTRUCTOR Y CACHÉ
// ===================================================================================================================================== //
Miembro* newMiembro() {
    Miembro* oneMiembro = (Miembro*)malloc(sizeof(Miembro));
    if (oneMiembro != NULL) {
        memset(oneMiembro, 0, sizeof(Miembro));
    }
    return oneMiembro;
}

void integrantes_init_cache(ArrayList* alistIntegrantes) {
    if(alistIntegrantes != NULL) {
        pListIntegrantesLocal = alistIntegrantes;
        printf("===================================================================================\n");
        printf("✅ Negociando espacio en memoria para el servicio de [ Integrantes ].\n");
    } else {
        printf("===================================================================================\n");
        printf("⚠️ Advertencia: Caché de [ Integrantes ] NULL.\n");
    }
}

void integrantes_load_storage(ArrayList* alistIntegrantes) {

    if(alistIntegrantes == NULL) return;

    // Modificamos el query para traer los nombres usando JOINs
    const char* query = "SELECT m.id_grupo, m.id_usuario, m.es_coordinador, m.fecha_ingreso, "
                        "u.nombre AS nombre_usuario, g.nombre AS nombre_grupo "
                        "FROM cc_miembros_grupo m "
                        "JOIN cc_usuarios u ON m.id_usuario = u.id "
                        "JOIN cc_grupos g ON m.id_grupo = g.id "
                        "WHERE m.fecha_baja IS NULL";

    DBResult *res = db_query(query);
    if (!res) return;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Miembro* nM = newMiembro();
        if (nM != NULL) {
            nM->id_grupo = atoi(row[0]);
            nM->id_usuario = atoi(row[1]);
            nM->es_coordinador = atoi(row[2]);
            strncpy(nM->fecha_ingreso, row[3] ? row[3] : "", 19);
            strncpy(nM->nombre_usuario, row[4] ? row[4] : "", 99); // NUEVO
            strncpy(nM->nombre_grupo, row[5] ? row[5] : "", 99);   // NUEVO

            alistIntegrantes->add(alistIntegrantes, nM);
        }
    }
    db_free_result(res);
    printf("===================================================================================\n");
    printf("📊 Memoria: %d INTEGRANTES ACTIVOS cargados.\n", alistIntegrantes->len(alistIntegrantes));
}

static void get_integrante_value(const char *body, const char *key, char *out, size_t out_size) {
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

// ===================================================================================================================================== //
// BARRERA DE SEGURIDAD (Validación interna)
// ===================================================================================================================================== //
static int tiene_permisos_admin(int id_grupo, int id_usuario_peticion) {
    // 1. Verificar si es el Creador (Root del grupo)
    int id_creador_real = grupos_service_get_creador(id_grupo); // Función expuesta en grupos_service.h
    if (id_creador_real == id_usuario_peticion) {
        return 1;
    }

    // 2. Si no es el creador, verificar si es Coordinador en RAM
    if (pListIntegrantesLocal != NULL) {
        for (int i = 0; i < pListIntegrantesLocal->len(pListIntegrantesLocal); i++) {
            Miembro* m = (Miembro*) pListIntegrantesLocal->get(pListIntegrantesLocal, i);
            if (m->id_grupo == id_grupo && m->id_usuario == id_usuario_peticion && m->es_coordinador == 1) {
                return 1;
            }
        }
    }
    return 0;
}

// ===================================================================================================================================== //
// SERVICIO: AGREGAR INTEGRANTE
// ===================================================================================================================================== //
int integrantes_service_add(const char *body, int id_usuario_peticion, char *error_msg, int error_size) {
    char id_grupo_str[32], id_nuevo_usuario_str[32], es_coordinador_str[10];

    get_integrante_value(body, "id_grupo", id_grupo_str, sizeof(id_grupo_str));
    get_integrante_value(body, "id_usuario", id_nuevo_usuario_str, sizeof(id_nuevo_usuario_str));
    get_integrante_value(body, "es_coordinador", es_coordinador_str, sizeof(es_coordinador_str));

    int id_grupo = atoi(id_grupo_str);
    int id_nuevo_usuario = atoi(id_nuevo_usuario_str);
    int es_coordinador = atoi(es_coordinador_str);

    if (id_grupo <= 0 || id_nuevo_usuario <= 0) {
        snprintf(error_msg, error_size, "Faltan datos para agregar al integrante.");
        return 0;
    }

    // Validación Unix-like: ¿Quién ejecuta el comando tiene sudo en este contexto?
    if (!tiene_permisos_admin(id_grupo, id_usuario_peticion)) {
        snprintf(error_msg, error_size, "Acceso denegado: No tienes privilegios en este grupo.");
        return 0;
    }

    // Validar duplicados en RAM
    if (pListIntegrantesLocal != NULL) {
        for (int i = 0; i < pListIntegrantesLocal->len(pListIntegrantesLocal); i++) {
            Miembro* m = (Miembro*) pListIntegrantesLocal->get(pListIntegrantesLocal, i);
            if (m->id_grupo == id_grupo && m->id_usuario == id_nuevo_usuario) {
                snprintf(error_msg, error_size, "El usuario ya es integrante del grupo.");
                return 0;
            }
        }
    }

    // Inserción BD
    if (sp_c_insertar_miembro(id_grupo, id_nuevo_usuario, es_coordinador) == 0) {
        snprintf(error_msg, error_size, "Error interno de base de datos.");
        return 0;
    }

    // Sincronizar Caché
    Miembro* nuevo_m = newMiembro();
    if (nuevo_m) {
        nuevo_m->id_grupo = id_grupo;
        nuevo_m->id_usuario = id_nuevo_usuario;
        nuevo_m->es_coordinador = es_coordinador;
        pListIntegrantesLocal->add(pListIntegrantesLocal, nuevo_m);
        printf("✅ Memoria sincronizada: Usuario %d añadido al Grupo %d.\n", id_nuevo_usuario, id_grupo);
    }

    return 1;
}

// ===================================================================================================================================== //
// SERVICIO: DAR DE BAJA A UN INTEGRANTE (Soft Delete)
// ===================================================================================================================================== //
int integrantes_service_remove(const char *body, int id_usuario_peticion, char *error_msg, int error_size) {
    char id_grupo_str[32], id_eliminar_str[32];

    get_integrante_value(body, "id_grupo", id_grupo_str, sizeof(id_grupo_str));
    get_integrante_value(body, "id_usuario", id_eliminar_str, sizeof(id_eliminar_str));

    int id_grupo = atoi(id_grupo_str);
    int id_eliminar = atoi(id_eliminar_str);

    if (id_grupo <= 0 || id_eliminar <= 0) {
        snprintf(error_msg, error_size, "Faltan parámetros de eliminación.");
        return 0;
    }

    // Validar permisos
    if (!tiene_permisos_admin(id_grupo, id_usuario_peticion)) {
        snprintf(error_msg, error_size, "Acceso denegado: No tienes privilegios para expulsar miembros.");
        return 0;
    }

    // Ejecutar Baja Lógica en BD
    if (sp_c_baja_miembro(id_grupo, id_eliminar) == 0) {
        snprintf(error_msg, error_size, "Error en BD al procesar la baja.");
        return 0;
    }

    // Eliminar de la RAM para mantener limpieza y velocidad
    if (pListIntegrantesLocal != NULL) {
        for (int i = 0; i < pListIntegrantesLocal->len(pListIntegrantesLocal); i++) {
            Miembro* m = (Miembro*) pListIntegrantesLocal->get(pListIntegrantesLocal, i);
            if (m->id_grupo == id_grupo && m->id_usuario == id_eliminar) {
                // Suponiendo que tu ArrayList tiene una función de remoción (remove)
                // Si no la tiene, se puede hacer un shift de memoria, o marcarlo internamente
                pListIntegrantesLocal->remove(pListIntegrantesLocal, i);
                free(m);
                printf("✅ Memoria sincronizada: Usuario %d expulsado del Grupo %d.\n", id_eliminar, id_grupo);
                break;
            }
        }
    }

    return 1;
}

// ===================================================================================================================================== //
// ROUTES HANDLERS
// ===================================================================================================================================== //
// Nota: Aquí se asume que tu enrutador ya extrajo el ID del usuario del token JWT o Sesión.
// Por ejemplo, `client_user_id` viene de la capa de autenticación.

static void route_post_integrante_add(int client, const char *body) {
    char error_msg[256];
    int client_user_id = 1; // ⚠️ SUSTITUIR POR LA LECTURA REAL DEL TOKEN/SESIÓN

    if (integrantes_service_add(body, client_user_id, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Integrante añadido exitosamente\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "403 Forbidden", "application/json", response);
    }
}

static void route_post_integrante_remove(int client, const char *body) {
    char error_msg[256];
    int client_user_id = 1; // ⚠️ SUSTITUIR POR LA LECTURA REAL DEL TOKEN/SESIÓN

    if (integrantes_service_remove(body, client_user_id, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Integrante dado de baja\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "403 Forbidden", "application/json", response);
    }
}


static void route_post_integrantes_list(int client, const char *body) {

    char id_grupo_str[32];
    get_integrante_value(body, "id_grupo", id_grupo_str, sizeof(id_grupo_str));
    int target_id_grupo = atoi(id_grupo_str);

    if (target_id_grupo <= 0) {
        send_response(client, "400 Bad Request", "application/json", "{\"error\":\"Falta el parámetro id_grupo\"}");
        return;
    }

    if (pListIntegrantesLocal == NULL) {
        send_response(client, "500 Internal Error", "application/json", "{\"error\":\"Caché de integrantes no inicializada\"}");
        return;
    }

    // Calculamos el tamaño del buffer dinámicamente según la cantidad total de miembros en RAM
    size_t total_miembros = pListIntegrantesLocal->len(pListIntegrantesLocal);
    size_t buffer_size = (total_miembros * 350) + 512;
    char *json = (char*) calloc(1, buffer_size);

    if (json == NULL) {
        send_response(client, "500 Internal Server Error", "text/plain", "Error de asignación de memoria");
        return;
    }

    strcpy(json, "[");
    int count = 0;

    // Filtramos solo los miembros que pertenecen al grupo solicitado
    for (int i = 0; i < total_miembros; i++) {
        Miembro* m = (Miembro*) pListIntegrantesLocal->get(pListIntegrantesLocal, i);

        if (m->id_grupo == target_id_grupo) {
            char item[1024];

            // Lógica de traducción de Rol
            const char* rol_texto = (m->es_coordinador == 1) ? "Coordinador" : "Usuario Colaborador";

            snprintf(item, sizeof(item),
                "%s{\"id_grupo\": %d, \"nombre_grupo\": \"%s\", \"id_usuario\": %d, \"nombre_usuario\": \"%s\", \"es_coordinador\": %ds, \"rol\": \"%s\", \"fecha_ingreso\": \"%s\"}",
                (count > 0) ? "," : "", // Agrega la coma si no es el primer elemento
                m->id_grupo,
                m->nombre_grupo,
                m->id_usuario,
                m->nombre_usuario,
                m->es_coordinador,
                rol_texto,
                m->fecha_ingreso);

            strcat(json, item);
            count++;
        }
    }
    strcat(json, "]");

    send_response(client, "200 OK", "application/json", json);
    free(json);
}


// ===================================================================================================================================== //
// INIT ALL ROUTES INTEGRANTES
// ===================================================================================================================================== //
void init_integrantes_routes() {
    add_route("POST", "/integrantes/add", route_post_integrante_add);
    add_route("POST", "/integrantes/remove", route_post_integrante_remove);
    add_route("POST", "/integrantes/list", route_post_integrantes_list); // Para poblar la tabla del frontend
}
