#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>
#include "hash.h"
#include "routes.h"
#include "db.h"
#include "ArrayList.h"
#include "grupos_service.h"

// Variable estática para la memoria local del módulo (Caché RAM)
static ArrayList* pListGruposLocal = NULL;

// ===================================================================================================================================== //
// 🔒 STORED PROCEDURES: Prepared Statements (Grupos)
// ===================================================================================================================================== //
static int sp_insertar_grupo(const char* nombre, int id_creador) {
    MYSQL_STMT *stmt;
    MYSQL_BIND bind_param[2];
    MYSQL_BIND bind_result[1];
    int nuevo_id = 0;
    const char *query = "CALL sp_insertar_grupo(?, ?)";

    MYSQL *conn = connect_db();
    if (!conn) return 0;

    stmt = mysql_stmt_init(conn);
    if (!stmt || mysql_stmt_prepare(stmt, query, strlen(query))) {
        if (stmt) mysql_stmt_close(stmt);
        mysql_close(conn);
        return 0;
    }

    memset(bind_param, 0, sizeof(bind_param));

    // Parámetro 1: Nombre
    bind_param[0].buffer_type = MYSQL_TYPE_STRING;
    bind_param[0].buffer = (char *)nombre;
    bind_param[0].buffer_length = strlen(nombre);

    // Parámetro 2: ID Creador
    bind_param[1].buffer_type = MYSQL_TYPE_LONG;
    bind_param[1].buffer = (void *)&id_creador;
    bind_param[1].is_unsigned = 0;

    if (mysql_stmt_bind_param(stmt, bind_param) || mysql_stmt_execute(stmt)) {
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return 0;
    }

    memset(bind_result, 0, sizeof(bind_result));
    bind_result[0].buffer_type = MYSQL_TYPE_LONG;
    bind_result[0].buffer = &nuevo_id;

    if (mysql_stmt_bind_result(stmt, bind_result)) {
        mysql_stmt_close(stmt);
        mysql_close(conn);
        return 0;
    }

    mysql_stmt_fetch(stmt);
    mysql_stmt_free_result(stmt);
    while (!mysql_stmt_next_result(stmt)) mysql_stmt_free_result(stmt);

    mysql_stmt_close(stmt);
    mysql_close(conn);
    return nuevo_id;
}

static int sp_editar_grupo(int id, const char* nombre) {
    MYSQL_STMT *stmt;
    MYSQL_BIND bind_param[2];
    const char *query = "CALL sp_editar_grupo(?, ?)";

    MYSQL *conn = connect_db();
    if (!conn) return 0;

    stmt = mysql_stmt_init(conn);
    if (!stmt || mysql_stmt_prepare(stmt, query, strlen(query))) {
        if (stmt) mysql_stmt_close(stmt);
        mysql_close(conn);
        return 0;
    }

    memset(bind_param, 0, sizeof(bind_param));

    // Parámetro 1: ID
    bind_param[0].buffer_type = MYSQL_TYPE_LONG;
    bind_param[0].buffer = (void *)&id;
    bind_param[0].is_unsigned = 0;

    // Parámetro 2: Nombre
    bind_param[1].buffer_type = MYSQL_TYPE_STRING;
    bind_param[1].buffer = (char *)nombre;
    bind_param[1].buffer_length = strlen(nombre);

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
// CONSTRUCTOR
// ===================================================================================================================================== //
Grupo* newGrupo() {
    Grupo* oneGrupo = (Grupo*)malloc(sizeof(Grupo));
    if (oneGrupo != NULL) {
        memset(oneGrupo, 0, sizeof(Grupo));
    }
    return oneGrupo;
}

// ===================================================================================================================================== //
// INICIALIZACIÓN DE CACHÉ
// ===================================================================================================================================== //
void grupos_init_cache(ArrayList* alistGrupos) {
    if(alistGrupos != NULL) {
        pListGruposLocal = alistGrupos;
        printf("===================================================================================\n");
        printf("✅ Negociando espacio en memoria para el servicio de [ Grupos ].\n");
    } else {
        printf("===================================================================================\n");
        printf("⚠️ Advertencia: Se intentó inicializar la caché de [ Grupos ] con NULL.\n");
    }
}

// ===================================================================================================================================== //
// CARGAR DATOS EN ARRAYLIST DESDE BD
// ===================================================================================================================================== //
void grupos_load_storage(ArrayList* alistGrupos) {
    if(alistGrupos == NULL) return;

    DBResult *res = db_query("SELECT id, nombre, id_creador, fecha_creacion FROM cc_grupos");
    if (!res) return;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Grupo* nGrupo = newGrupo();
        if (nGrupo != NULL) {
            nGrupo->id = atoi(row[0]);
            strncpy(nGrupo->nombre, row[1] ? row[1] : "", 99);
            nGrupo->id_creador = atoi(row[2]);
            strncpy(nGrupo->fecha_creacion, row[3] ? row[3] : "", 19);

            alistGrupos->add(alistGrupos, nGrupo);
        }
    }

    db_free_result(res);
    printf("===================================================================================\n");
    printf("📊 Memoria: %d GRUPOS cargados. Espacio reservado: %d slots.\n", alistGrupos->len(alistGrupos), alistGrupos->reservedSize);
}

// ===================================================================================================================================== //
// FUNCIÓN AUXILIAR: PARSEO DEL BODY
// ===================================================================================================================================== //
static void get_grupo_value(const char *body, const char *key, char *out, size_t out_size) {
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
// SERVICIO: REGISTRAR NUEVO GRUPO
// ===================================================================================================================================== //
int grupos_service_register(const char *body, char *error_msg, int error_size) {
    char nombre[100];
    char d_nombre[100];
    char id_creador_str[32];

    get_grupo_value(body, "nombre", nombre, sizeof(nombre));
    url_decode(d_nombre, nombre);

    get_grupo_value(body, "id_creador", id_creador_str, sizeof(id_creador_str));
    int id_creador = atoi(id_creador_str);

    if (strlen(d_nombre) == 0 || id_creador <= 0) {
        snprintf(error_msg, error_size, "Hay campos sin completar o ID inválido.");
        return 0;
    }

    // 1. Verificación de duplicados en memoria RAM
    if (pListGruposLocal != NULL) {
        for (int i = 0; i < pListGruposLocal->len(pListGruposLocal); i++) {
            Grupo* g = (Grupo*) pListGruposLocal->get(pListGruposLocal, i);
            if (g->id_creador == id_creador && strcasecmp(g->nombre, d_nombre) == 0) {
                snprintf(error_msg, error_size, "Error: Ya tienes un grupo llamado '%s'.", d_nombre);
                return 0;
            }
        }
    }

    // 2. Inserción en Base de Datos vía SP
    int nuevo_id = sp_insertar_grupo(d_nombre, id_creador);

    if (nuevo_id <= 0) {
        snprintf(error_msg, error_size, "Error interno al guardar en la base de datos.");
        return 0;
    }

    // 3. Sincronización en ArrayList
    Grupo* nuevoGrupo = newGrupo();
    if (nuevoGrupo) {
        nuevoGrupo->id = nuevo_id;
        strncpy(nuevoGrupo->nombre, d_nombre, 99);
        nuevoGrupo->id_creador = id_creador;
        // La fecha_creacion se omitirá en RAM temporalmente hasta el reinicio

        pListGruposLocal->add(pListGruposLocal, nuevoGrupo);
        printf("✅ Sincronización exitosa: Grupo '%s' (ID: %d) añadido a RAM.\n", d_nombre, nuevo_id);
    }

    return 1;
}

// ===================================================================================================================================== //
// SERVICIO: EDITAR GRUPO
// ===================================================================================================================================== //
int grupos_service_edit(const char *body, char *error_msg, int error_size) {
    char id_str[32];
    char nombre[100];
    char d_nombre[100];

    get_grupo_value(body, "id", id_str, sizeof(id_str));
    get_grupo_value(body, "nombre", nombre, sizeof(nombre));
    url_decode(d_nombre, nombre);

    int id_a_editar = atoi(id_str);

    if (id_a_editar <= 0 || strlen(d_nombre) == 0) {
        snprintf(error_msg, error_size, "ID o Nombre no completados");
        return 0;
    }

    // 1. Verificación en memoria
    if (pListGruposLocal != NULL) {
        for (int i = 0; i < pListGruposLocal->len(pListGruposLocal); i++) {
            Grupo* g = (Grupo*) pListGruposLocal->get(pListGruposLocal, i);
            if (g->id != id_a_editar && strcasecmp(g->nombre, d_nombre) == 0 && g->id_creador == id_a_editar /* asume contexto de creador */) {
                snprintf(error_msg, error_size, "Error: Nombre de grupo existente.");
                return 0;
            }
        }
    }

    // 2. Actualizar en Base de Datos vía SP
    if (sp_editar_grupo(id_a_editar, d_nombre) == 0) {
        snprintf(error_msg, error_size, "Error al actualizar en la base de datos.");
        return 0;
    }

    // 3. Actualizar en Memoria
    if (pListGruposLocal != NULL) {
        for (int i = 0; i < pListGruposLocal->len(pListGruposLocal); i++) {
            Grupo* g = (Grupo*) pListGruposLocal->get(pListGruposLocal, i);
            if (g->id == id_a_editar) {
                strncpy(g->nombre, d_nombre, 99);
                printf("✅ Memoria sincronizada: Grupo ID %d actualizado a '%s'.\n", id_a_editar, d_nombre);
                break;
            }
        }
    }

    return 1;
}

// ===================================================================================================================================== //
// SERVICIO: OBTENER UN REGISTRO POR ID
// ===================================================================================================================================== //
int get_grupos_service_id(const char *body, char *json_out, int out_size) {
    char id_str[10];
    get_grupo_value(body, "id", id_str, sizeof(id_str));

    if (strlen(id_str) == 0) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"ID no provisto\" }");
        return 0;
    }

    char query[256];
    snprintf(query, sizeof(query), "SELECT id, nombre, id_creador, fecha_creacion FROM cc_grupos WHERE id = %s LIMIT 1;", id_str);

    DBResult *res = db_query(query);
    MYSQL_ROW row;

    if (!res || !(row = mysql_fetch_row(res))) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"Grupo no encontrado\" }");
        if (res) db_free_result(res);
        return 0;
    }

    snprintf(json_out, out_size,
             "{ \"id\": %s, \"nombre\": \"%s\", \"id_creador\": %s, \"fecha_creacion\": \"%s\" }",
             row[0] ? row[0] : "0",
             row[1] ? row[1] : "",
             row[2] ? row[2] : "0",
             row[3] ? row[3] : "");

    db_free_result(res);
    return 1;
}

// ===================================================================================================================================== //
// ROUTES HANDLERS
// ===================================================================================================================================== //
static void route_post_grupo(int client, const char *body) {
    char error_msg[256];
    if (grupos_service_register(body, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Grupo creado y caché actualizada\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "400 Bad Request", "application/json", response);
    }
}

static void route_post_grupo_edit(int client, const char *body) {
    char error_msg[256];
    if (grupos_service_edit(body, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Grupo actualizado correctamente\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "400 Bad Request", "application/json", response);
    }
}

static void route_get_grupos_list(int client, const char *body) {
    (void)body;

    if (pListGruposLocal == NULL) {
        printf("❌ Error crítico: pListGruposLocal sigue siendo NULL en el handler.\n");
        send_response(client, "500 Internal Error", "application/json", "{\"error\":\"Error de vinculación de memoria\"}");
        return;
    }

    size_t total_grupos = pListGruposLocal->len(pListGruposLocal);
    size_t buffer_size = (total_grupos * 250) + 512;
    char *json = (char*) calloc(1, buffer_size);

    if (json == NULL) {
        send_response(client, "500 Internal Server Error", "text/plain", "Error de memoria");
        return;
    }

    strcpy(json, "[");

    for (int i = 0; i < total_grupos; i++) {
        Grupo* oneGrupo = (Grupo*) pListGruposLocal->get(pListGruposLocal, i);
        char item[250];

        snprintf(item, sizeof(item),
            "{\"id\": %d, \"nombre\": \"%s\" , \"id_creador\": %d}%s",
            oneGrupo->id, oneGrupo->nombre, oneGrupo->id_creador, (i < total_grupos - 1) ? "," : "");

        strcat(json, item);
    }
    strcat(json, "]");

    send_response(client, "200 OK", "application/json", json);
    free(json);
}

static void route_get_grupo_by_id(int client, const char *body) {
    char response_json[1024];
    if (get_grupos_service_id(body, response_json, sizeof(response_json))) {
        send_response(client, "200 OK", "application/json", response_json);
    } else {
        send_response(client, "404 Not Found", "application/json", response_json);
    }
}

// ===================================================================================================================================== //
// INIT ALL ROUTES GRUPOS
// ===================================================================================================================================== //
void init_grupos_routes() {
    add_route("GET", "/grupos/list", route_get_grupos_list);
    add_route("POST", "/grupos/add", route_post_grupo);
    add_route("POST", "/grupos/edit", route_post_grupo_edit);
    add_route("POST", "/grupos/get", route_get_grupo_by_id);
}
