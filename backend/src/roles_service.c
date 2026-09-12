#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <mysql/mysql.h>
#include "hash.h"
#include "routes.h"
#include "db.h"
#include "ArrayList.h"
#include "roles_service.h"

// ===================================================================================================================================== //
// Variable local para que las rutas accedan a la memoria enviada desde el main
// ===================================================================================================================================== //
static ArrayList* pListRolesLocal = NULL;


// ===================================================================================================================================== //
// 🔒 STORED PROCEDURES: Prepared Statements (Roles)
// ===================================================================================================================================== //
static int sp_insertar_rol(const char* descripcion) {

    MYSQL_STMT *stmt; MYSQL_BIND bind_param[1]; MYSQL_BIND bind_result[1]; int nuevo_id = 0;
    const char *query = "CALL sp_insertar_rol(?)";
    MYSQL *conn = connect_db();
    if (!conn) return 0;

    stmt = mysql_stmt_init(conn);
    if (!stmt || mysql_stmt_prepare(stmt, query, strlen(query))) { if(stmt) mysql_stmt_close(stmt); mysql_close(conn); return 0; }

    memset(bind_param, 0, sizeof(bind_param));
    bind_param[0].buffer_type = MYSQL_TYPE_STRING; bind_param[0].buffer = (char *)descripcion; bind_param[0].buffer_length = strlen(descripcion);

    if (mysql_stmt_bind_param(stmt, bind_param) || mysql_stmt_execute(stmt)) { mysql_stmt_close(stmt); mysql_close(conn); return 0; }

    memset(bind_result, 0, sizeof(bind_result));
    bind_result[0].buffer_type = MYSQL_TYPE_LONG; bind_result[0].buffer = &nuevo_id;

    if (mysql_stmt_bind_result(stmt, bind_result)) { mysql_stmt_close(stmt); mysql_close(conn); return 0; }

    mysql_stmt_fetch(stmt); mysql_stmt_free_result(stmt);
    while (!mysql_stmt_next_result(stmt)) mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt); mysql_close(conn);
    return nuevo_id;

}


// ===================================================================================================================================== //


static int sp_editar_rol(int id, const char* descripcion) {

    MYSQL_STMT *stmt; MYSQL_BIND bind_param[2];
    const char *query = "CALL sp_editar_rol(?, ?)";
    MYSQL *conn = connect_db();
    if (!conn) return 0;

    stmt = mysql_stmt_init(conn);
    if (!stmt || mysql_stmt_prepare(stmt, query, strlen(query))) { if(stmt) mysql_stmt_close(stmt); mysql_close(conn); return 0; }

    memset(bind_param, 0, sizeof(bind_param));
    bind_param[0].buffer_type = MYSQL_TYPE_LONG; bind_param[0].buffer = (void *)&id; bind_param[0].is_unsigned = 0;
    bind_param[1].buffer_type = MYSQL_TYPE_STRING; bind_param[1].buffer = (char *)descripcion; bind_param[1].buffer_length = strlen(descripcion);

    if (mysql_stmt_bind_param(stmt, bind_param) || mysql_stmt_execute(stmt)) { mysql_stmt_close(stmt); mysql_close(conn); return 0; }
    while (!mysql_stmt_next_result(stmt)) mysql_stmt_free_result(stmt);
    mysql_stmt_close(stmt); mysql_close(conn);
    return 1;

}


// ===================================================================================================================================== //
// CONSTRUCTOR
// ===================================================================================================================================== //
Roles* new_role(){

    Roles* oneRole = (Roles*)malloc(sizeof(Roles));

    if(oneRole != NULL){
        memset(oneRole, 0, sizeof(Roles));
    }

    return oneRole;
}


// ===================================================================================================================================== //
// función auxiliar: obtiene valor de key=valor en el body
// ===================================================================================================================================== //
static void get_roles_value(const char *body, const char *key, char *out, size_t out_size) {

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
// Iniciliacion de cache
// ===================================================================================================================================== //
void roles_init_cache(ArrayList* alist_roles) {

    if(alist_roles != NULL) {
        // 2. ASIGNACIÓN CRÍTICA: Aquí guardamos la dirección de memoria que viene del main
        pListRolesLocal = alist_roles;
        printf("===================================================================================\n");
        printf("✅ Negociando espacio en memoria para el servicio de Roles.\n");
    } else {
        printf("===================================================================================\n");
        printf("⚠️ Advertencia: Se intentó inicializar la caché de Roles con NULL.\n");
    }
}

// ===================================================================================================================================== //
// cargar datos de medicos en ArrayList
// ===================================================================================================================================== //
void roles_load_storage(ArrayList* alist_roles) {

    if(alist_roles == NULL) return;

    // Ajusta la query a tus necesidades
    DBResult *res = db_query("CALL sp_obtener_todos_roles()");
    if (!res) return;

    MYSQL_ROW row;
    while ((row = mysql_fetch_row(res))) {
        Roles* oneRole = new_role();
        if (oneRole) {
            oneRole->id = atoi(row[0]);
            strncpy(oneRole->descripcion, row[1], 100);


            alist_roles->add(alist_roles, oneRole);
        }
    }
    db_free_result(res);
    printf("===================================================================================\n");
    printf("📊 Memoria: %d roles cargados. Espacio reservado: %d slots.\n", alist_roles->len(alist_roles), alist_roles->reservedSize);
}


// ===================================================================================================================================== //
// FUNCION PARA REGISTRAR NUEVO ROL
// ===================================================================================================================================== //
int roles_service_register(const char *body, char *error_msg, int error_size) {

    char descripcion[128];
    char d_descripcion[128];

    get_roles_value(body, "descripcion", descripcion, sizeof(descripcion));
    url_decode(d_descripcion, descripcion); // Asumiendo tu función de decode

    if (strlen(d_descripcion) == 0) {
        snprintf(error_msg, error_size, "La descripción es obligatoria.");
        return 0;
    }

    // 1. VERIFICACIÓN DE DUPLICADOS EN MEMORIA (ArrayList)
    // Es más rápido que consultar la DB nuevamente
    if (pListRolesLocal != NULL) {
        for (int i = 0; i < pListRolesLocal->len(pListRolesLocal); i++) {
            Roles* r = (Roles*) pListRolesLocal->get(pListRolesLocal, i);
            if (strcasecmp(r->descripcion, d_descripcion) == 0) {
                snprintf(error_msg, error_size, "Error: El rol '%s' ya existe.", d_descripcion);
                return 0;
            }
        }
    }

    // 2. INSERCIÓN EN BASE DE DATOS
    int nuevo_id = sp_insertar_rol(d_descripcion);

    if (nuevo_id <= 0) {
        snprintf(error_msg, error_size, "Error interno al guardar en la base de datos.");
        return 0;
    }


    Roles* nuevoRol = new_role();
    if (nuevoRol) {

        memset(nuevoRol, 0, sizeof(Roles));

        nuevoRol->id = nuevo_id;
        strncpy(nuevoRol->descripcion, d_descripcion, 100);

        // Sincronizamos el ArrayList inmediatamente
        pListRolesLocal->add(pListRolesLocal, nuevoRol);

        printf("✅ Sincronización exitosa: Rol '%s' (ID: %d) añadido a RAM.\n",
                d_descripcion, nuevo_id);
    }

    return 1;
}


// ===================================================================================================================================== //
// FUNCION EDICIÓN DE ROL
// ===================================================================================================================================== //
int roles_service_edit(const char *body, char *error_msg, int error_size) {

    char id_str[32];
    char descripcion[100];
    char d_descripcion[100];

    // 1. Extraer datos del body
    get_roles_value(body, "id", id_str, sizeof(id_str));
    get_roles_value(body, "descripcion", descripcion, sizeof(descripcion));
    url_decode(d_descripcion, descripcion);

    int id_a_editar = atoi(id_str);

    if (id_a_editar <= 0 || strlen(d_descripcion) == 0) {
        snprintf(error_msg, error_size, "ID o descripción no completados");
        return 0;
    }

    // 2. VERIFICACIÓN DE EXISTENCIA Y DUPLICADOS EN MEMORIA
    if (pListRolesLocal != NULL) {
        for (int i = 0; i < pListRolesLocal->len(pListRolesLocal); i++) {
            Roles* r = (Roles*) pListRolesLocal->get(pListRolesLocal, i);

            // Si el nombre ya existe en otro ID, rebotamos la edición
            if (r->id != id_a_editar && strcasecmp(r->descripcion, d_descripcion) == 0) {
                snprintf(error_msg, error_size, "Error: Rol Existente.");
                return 0;
            }
        }
    }

    // 3. ACTUALIZAR EN BASE DE DATOS
    if (sp_editar_rol(id_a_editar, d_descripcion) == 0) {
        snprintf(error_msg, error_size, "Error al actualizar en la base de datos.");
        return 0;
    }

    // 4. ACTUALIZAR EN MEMORIA (ArrayList)
    if (pListRolesLocal != NULL) {
        for (int i = 0; i < pListRolesLocal->len(pListRolesLocal); i++) {
            Roles* r = (Roles*) pListRolesLocal->get(pListRolesLocal, i);

            if (r->id == id_a_editar) {
                // Actualizamos el puntero directamente en la memoria
                strncpy(r->descripcion, d_descripcion, 99);
                printf("✅ Memoria sincronizada: Rol ID %d actualizado a '%s'.\n", id_a_editar, d_descripcion);
                break;
            }
        }
    }

    return 1;
}


// ===================================================================================================================================== //
// LOGICA QUE RETORNA UN REGISTRO AL SER CONSULTADO POR ID
// ===================================================================================================================================== //
int get_roles_service_id(const char *body, char *json_out, int out_size) {

    char id_str[10];
    get_roles_value(body, "id", id_str, sizeof(id_str));

    if (strlen(id_str) == 0) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"ID no provisto\" }");
        return 0;
    }

    char query[512];

    snprintf(query, sizeof(query),
             "SELECT * FROM cc_roles WHERE id = %s LIMIT 1;", id_str);

    DBResult *res = db_query(query);

    // 🔹 CORRECCIÓN AQUÍ: Usamos db_fetch_row o la función correspondiente de tu db.h
    // Si tu db.h usa mysql_fetch_row directamente:
    MYSQL_ROW row;

    if (!res || !(row = mysql_fetch_row(res))) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"Rol no encontrado\" }");
        if (res) db_free_result(res);
        return 0;
    }

    // Construimos el JSON usando los índices del array 'row'
    snprintf(json_out, out_size,
             "{ \"id\": %s, \"descripcion\": \"%s\" }",
             row[0] ? row[0] : "0",
             row[1] ? row[1] : "");

    db_free_result(res);
    return 1;
}

// ===================================================================================================================================== //
// ROUTES HANDLER
// ===================================================================================================================================== //

// ===================================================================================================================================== //
// Handler POST para el registro
// ===================================================================================================================================== //
static void route_post_roles(int client, const char *body) {
    char error_msg[256];
    if (roles_service_register(body, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Rol creado y caché actualizada\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "400 Bad Request", "application/json", response);
    }
}

// ===================================================================================================================================== //
// Handler POST para la edición
// ===================================================================================================================================== //
static void route_post_roles_edit(int client, const char *body) {

    char error_msg[256];
    if (roles_service_edit(body, error_msg, sizeof(error_msg))) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"Rol actualizado correctamente\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response), "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "400 Bad Request", "application/json", response);
    }
}


// ===================================================================================================================================== //
// ROUTES FOR LIST ROLES
// ===================================================================================================================================== //
static void route_get_roles_list(int client, const char *body) {

    // Ahora pListMedicosLocal ya no debería ser NULL
    if (pListRolesLocal == NULL) {
        printf("❌ Error crítico: pListRolesLocal sigue siendo NULL en el handler.\n");
        send_response(client, "500 Internal Error", "application/json", "{\"error\":\"Error de vinculación de memoria\"}");
        return;
    }

    // Estimamos el tamaño del JSON (aprox 150 bytes por médico)
    size_t total_roles = pListRolesLocal->len(pListRolesLocal);
    size_t buffer_size = (total_roles * 150) + 10;
    char *json = (char*) calloc(1, buffer_size); // calloc limpia la memoria

    if (json == NULL) {
        send_response(client, "500 Internal Server Error", "text/plain", "Error de memoria");
        return;
    }

    strcpy(json, "[");
    for (int i = 0; i < total_roles; i++) {
        Roles* oneRole = (Roles*) pListRolesLocal->get(pListRolesLocal, i);
        char item[200];

        // Armamos el objeto JSON
        snprintf(item, sizeof(item),
            "{\"id\": %d, \"descripcion\": \"%s\" }%s",
            oneRole->id, oneRole->descripcion, (i < total_roles - 1) ? "," : "");

        strcat(json, item);
    }
    strcat(json, "]");

    send_response(client, "200 OK", "application/json", json);
    free(json); // Liberamos el buffer del JSON
}


// ===================================================================================================================================== //
// ROUTE OR GET ONE REGESTRY
// ===================================================================================================================================== //
static void route_get_roles_by_id(int client, const char *body) {

    char response_json[1024];

    if (get_roles_service_id(body, response_json, sizeof(response_json))) {
        send_response(client, "200 OK", "application/json", response_json);
    } else {
        send_response(client, "404 Not Found", "application/json", response_json);
    }
}

// ===================================================================================================================================== //
// INIT ALL ROUTES ROLES
// ===================================================================================================================================== //
void init_roles_routes() {

    add_route("GET", "/roles/list", route_get_roles_list); // endpoint para listar roles
    add_route("POST", "/roles/add", route_post_roles); // endpoint para alta de nuevo rol
    add_route("POST", "/roles/edit", route_post_roles_edit); // endpoint para editar un registro de roles
    add_route("POST", "/roles/get", route_get_roles_by_id); // endpoint para consultar un registro por ID
}
