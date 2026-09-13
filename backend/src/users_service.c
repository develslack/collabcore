#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <openssl/sha.h>
#include <mysql/mysql.h>
#include "routes.h"
#include "hash.h"
#include "db.h"
#include "users_service.h"
#include "session_manager.h"


// función auxiliar: obtiene valor de key=valor en el body
void get_value(const char *body, const char *key, char *out, size_t out_size) {

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


// BUSCA UN REGISTRO EN LA TABLA DM_USUARIOS POR NOMBRE

int get_user_service_name(const char *body, char *json_out, int out_size) {

    char name_str[100];
    char decoded_name[100];
    get_value(body, "user_name", name_str, sizeof(name_str));

    if (strlen(name_str) == 0) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"Nombre no provisto\" }");
        return 0;
    }

    url_decode(decoded_name,name_str);

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT id, nombre, email, rol_id "
             "FROM cc_usuarios WHERE nombre = \"%s\" LIMIT 1;", decoded_name);

    DBResult *res = db_query(query);

    MYSQL_ROW row;
    if (!res || !(row = mysql_fetch_row(res))) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"Usuario no encontrado\" }");
        if (res) db_free_result(res);
        return 0;
    }

    // Construimos el JSON usando los índices del array 'row'
    snprintf(json_out, out_size,
             "{ \"id\": %s, \"nombre\": \"%s\", \"email\": \"%s\", \"rol\": \"%s\" }",
             row[0] ? row[0] : "0",
             row[1] ? row[1] : "",
             row[2] ? row[2] : "",
             row[3] ? row[3] : "");

    db_free_result(res);
    return 1;
}


// BUSCA UN REGISTRO EN LA TABLA PS_USUARIOS POR ID

int get_user_service_id(const char *body, char *json_out, int out_size) {

    char id_str[100];
    char decoded_id[100];
    get_value(body, "user_id", id_str, sizeof(id_str));

    if (strlen(id_str) == 0) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"ID no provisto\" }");
        return 0;
    }

    url_decode(decoded_id,id_str);

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT id, nombre, email, rol_id "
             "FROM cc_usuarios WHERE id = \"%s\" LIMIT 1;", decoded_id);

    DBResult *res = db_query(query);

    MYSQL_ROW row;
    if (!res || !(row = mysql_fetch_row(res))) {
        snprintf(json_out, out_size, "{ \"status\": \"error\", \"message\": \"Usuario no encontrado\" }");
        if (res) db_free_result(res);
        return 0;
    }

    // Construimos el JSON usando los índices del array 'row'
    snprintf(json_out, out_size,
             "{ \"id\": %s, \"nombre\": \"%s\", \"email\": \"%s\", \"rol\": \"%s\" }",
             row[0] ? row[0] : "0",
             row[1] ? row[1] : "",
             row[2] ? row[2] : "",
             row[3] ? row[3] : "");

    db_free_result(res);
    return 1;
}


// Handler para la ruta de la funcion get_user_service_name

static void route_get_user_by_name(int client, const char *body) {
    char response_json[1024];
    if (get_user_service_name(body, response_json, sizeof(response_json))) {
        send_response(client, "200 OK", "application/json", response_json);
    } else {
        send_response(client, "404 Not Found", "application/json", response_json);
    }
}


// Handler para la ruta de la funcion get_user_service_id

static void route_get_user_by_id(int client, const char *body) {
    char response_json[1024];
    if (get_user_service_id(body, response_json, sizeof(response_json))) {
        send_response(client, "200 OK", "application/json", response_json);
    } else {
        send_response(client, "404 Not Found", "application/json", response_json);
    }
}

// FUNCION QUE CAPTURA LOS DATOS PARA REGISTRO DE UN USUARIO

int users_service_register(const char *body, char *error_msg, int error_size) {

    char nombre[128];
    char email[128];
    char password_1[128];
    char password_2[128];
    char rol[128];
    char decoded_email[256];
    char decoded_nombre[256];

    get_value(body, "nombre", nombre, sizeof(nombre));
    get_value(body, "email", email, sizeof(email));
    get_value(body, "password_1", password_1, sizeof(password_1));
    get_value(body, "password_2", password_2, sizeof(password_2));
    get_value(body, "rol", rol, sizeof(rol));

    if (strlen(nombre) == 0 || strlen(email) == 0 || strlen(password_1) == 0 || strlen(password_2) == 0 || strlen(rol) == 0) {
        snprintf(error_msg, error_size, "missing required fields");
        return 0;
    }else{

        if(strcmp(password_1,password_2) != 0){
            snprintf(error_msg, error_size, "Los Passwords no coinciden!");
            return 0;
        }
    }

    url_decode(decoded_email, email);
    url_decode(decoded_nombre,nombre);

    // Validar email repetido
    char query_check[256];
    snprintf(query_check, sizeof(query_check),
             "SELECT COUNT(*) FROM cc_usuarios WHERE nombre = '%s' and email = '%s'", decoded_nombre, decoded_email);
    int exists = db_query_int(query_check);
    if (exists < 0) {
        snprintf(error_msg, error_size, "database error");
        return 0;
    } else if (exists > 0) {
        snprintf(error_msg, error_size, "email already exists");
        return 0;
    }

    // Generar hash directamente sin salt
    char hash[SHA_DIGEST_LENGTH * 2 + 1];
    hash_password(password_1, "", hash);

    // SQL: insertar usuario
    char query[512];
    snprintf(query, sizeof(query),
             "INSERT INTO cc_usuarios (nombre, email, password, rol_id) "
             "VALUES ('%s','%s','%s', %d)",
             decoded_nombre, decoded_email, hash, atoi(rol));

    if (db_execute(query) != 0) {
        snprintf(error_msg, error_size, "database error");
        return 0;
    }

    return 1;
}


int users_service_password(const char *body, char *error_msg, int error_size){

    char email[128];
    char password_1[128];
    char password_2[128];
    char decoded_email[256];

    get_value(body, "email", email, sizeof(email));
    get_value(body, "password_1", password_1, sizeof(password_1));
    get_value(body, "password_2", password_2, sizeof(password_2));

    if (strlen(email) == 0 || strlen(password_1) == 0 || strlen(password_2) == 0) {
        snprintf(error_msg, error_size, "Hay campos sin completar...");
        return 0;
    }else{
        if(strcmp(password_1,password_2) != 0){
            snprintf(error_msg, error_size, "Los passwords no coinciden");
            return 0;
        }
    }

    url_decode(decoded_email, email);

    // Validar email repetido
    char query_check[256];
    snprintf(query_check, sizeof(query_check),
             "SELECT COUNT(*) FROM cc_usuarios WHERE email = '%s'", decoded_email);
    int exists = db_query_int(query_check);

    if (exists < 0) {

        snprintf(error_msg, error_size, "database error");
        return 0;

    }else if(exists == 0){

        snprintf(error_msg, error_size, "Usuario inexistente...");
        return 0;

    }else if (exists > 0) {

        // Generar hash directamente sin salt
        char hash[SHA_DIGEST_LENGTH * 2 + 1];
        hash_password(password_1, "", hash);

        // SQL: actualizar password
        char query[512];
        snprintf(query, sizeof(query),
                "UPDATE cc_usuarios SET password = '%s' where email = '%s'", hash, decoded_email);

        if (db_execute(query) != 0) {
            snprintf(error_msg, error_size, "database error");
            return 0;
        }

    }

    return 1;

}


int users_service_update_rol(const char* body, char* error_msg, int error_size){

    char id_str[64];
    char rol[32];

    get_value(body, "id", id_str, sizeof(id_str));
    get_value(body, "rol", rol, sizeof(rol));

    if(strlen(id_str) == 0 || strlen(rol) == 0){
        snprintf(error_msg, error_size, "Hay campos sin datos...");
        return 0;
    }

    // Validar usuario
    char query_check[256];
    snprintf(query_check, sizeof(query_check),
             "SELECT COUNT(*) FROM cc_usuarios WHERE id = '%s'", id_str);
    int exists = db_query_int(query_check);

    if (exists < 0) {

        snprintf(error_msg, error_size, "database error");
        return 0;

    }else if(exists == 0){

        snprintf(error_msg, error_size, "Usuario inexistente...");
        return 0;

    }else if (exists > 0) {

        char query[512];
        snprintf(query, sizeof(query),
                "UPDATE cc_usuarios SET rol_id = '%s' where id = '%s'", rol, id_str);

        if (db_execute(query) != 0) {
            snprintf(error_msg, error_size, "database error");
            return 0;
        }

    }

    return 1;
}

// FUNCIONES PARA LOGIN //
int users_service_login(const char *email, const char *password, char *error_msg, UserData *user_out) {

    char decoded_email[256];
    url_decode(decoded_email, email);

    MYSQL *conn = connect_db();
    if (!conn) {
        strcpy(error_msg, "Error de conexión a base de datos");
        return 0;
    }

    char query[512];
    snprintf(query, sizeof(query),
             "SELECT id, nombre, email, password, rol_id FROM cc_usuarios WHERE email='%s' LIMIT 1",
             decoded_email);

    if (mysql_query(conn, query)) {
        snprintf(error_msg, 256, "Error en la consulta SQL: %s", mysql_error(conn));
        mysql_close(conn);
        return 0;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(res);

    if (!row) {
        strcpy(error_msg, "Usuario no encontrado");
        mysql_free_result(res);
        mysql_close(conn);
        return 0;
    }

    const char *db_password = row[3];
    if (!db_password) {
        strcpy(error_msg, "Contraseña no disponible");
        mysql_free_result(res);
        mysql_close(conn);
        return 0;
    }

    char hashed_password[SHA_DIGEST_LENGTH * 2 + 1];
    hash_password(password, "", hashed_password);

    if (strcmp(db_password, hashed_password) != 0) {
        strcpy(error_msg, "Contraseña incorrecta");
        mysql_free_result(res);
        mysql_close(conn);
        return 0;
    }

    user_out->id = atoi(row[0]);
    strncpy(user_out->nombre, row[1], sizeof(user_out->nombre) - 1);
    user_out->nombre[sizeof(user_out->nombre) - 1] = '\0';

    strncpy(user_out->email, row[2], sizeof(user_out->email) - 1);
    user_out->email[sizeof(user_out->email) - 1] = '\0';

    user_out->rol_id = atoi(row[4]);

    mysql_free_result(res);
    mysql_close(conn);
    return 1;
}


// Handler GET /login
static void route_get_login(int client, const char *body) {
    (void)body;
    send_response(client, "200 OK", "application/json", "{ \"message\": \"login\" }");
}

// Handler POST /login
static void route_post_login(int client, const char *body) {

    char email[256] = {0}, password[256] = {0};

    get_value(body, "email", email, sizeof(email));
    get_value(body, "password", password, sizeof(password));

    char error_msg[256];
    UserData user;
    user.session_token = tokenGenerator(); // Generamos el token de 128 bytes seguro

    if (users_service_login(email, password, error_msg, &user)) {

        // Registramos al usuario en la RAM del Kernel
        UserSession* sesion_creada = session_manager_crear(user.nombre, user.rol_id, user.session_token);

        if (sesion_creada != NULL) {
            char response[1024];
            snprintf(response, sizeof(response),
                     "{ \"status\": \"success\", \"message\": \"Login correcto\", "
                     "\"user\": { \"id\": %d, \"nombre\": \"%s\", \"email\": \"%s\", \"rol_id\": %d, \"token\": \"%s\" } }",
                     user.id, user.nombre, user.email, user.rol_id, user.session_token);

            send_response(client, "200 OK", "application/json", response);
        } else {
            send_response(client, "500 Internal Error", "application/json", "{ \"status\": \"error\", \"message\": \"No se pudo iniciar sesion en RAM\" }");
        }
    } else {
        char response[512];
        snprintf(response, sizeof(response),
                 "{ \"status\": \"error\", \"message\": \"%s\" }", error_msg);
        send_response(client, "401 Unauthorized", "application/json", response);
    }

    if (user.session_token) {
        free(user.session_token);
    }
}


// FUNCIONES AUXILIARES //
static void append_str(char **dest, size_t *size, const char *src) {

    size_t needed = strlen(*dest) + strlen(src) + 1;
    if (needed > *size) {
        *size = needed * 2;
        *dest = realloc(*dest, *size);
        if (!*dest) {
            perror("realloc");
            exit(1);
        }
    }
    strcat(*dest, src);
}


static char *json_escape(const char *src) {

    size_t len = strlen(src);
    char *out = malloc(len * 2 + 1);
    char *p = out;
    for (const char *s = src; *s; s++) {
        switch (*s) {
            case '\"': *p++ = '\\'; *p++ = '\"'; break;
            case '\\': *p++ = '\\'; *p++ = '\\'; break;
            case '\n': *p++ = '\\'; *p++ = 'n'; break;
            case '\r': *p++ = '\\'; *p++ = 'r'; break;
            case '\t': *p++ = '\\'; *p++ = 't'; break;
            default: *p++ = *s; break;
        }
    }
    *p = '\0';
    return out;
}

// LISTAR USUARIOS //

static void users_service_list(int client, const char *body) {

    (void)body;

    DBResult *res = db_query("SELECT u.id, u.nombre, u.email, u.rol_id, r.descripcion AS rol FROM cc_usuarios u JOIN cc_roles r ON u.rol_id = r.id");
    if (!res) {
        send_response(client, "500 Internal Server Error", "application/json",
                      "{ \"status\": \"error\", \"message\": \"Error al consultar usuarios\" }");
        return;
    }

    MYSQL_ROW row;
    char buffer[8192];
    size_t offset = 0;

    offset += snprintf(buffer + offset, sizeof(buffer) - offset, "[");
    int first = 1;

    while ((row = mysql_fetch_row(res))) {
        if (!first) {
            offset += snprintf(buffer + offset, sizeof(buffer) - offset, ",");
        }
        first = 0;

        offset += snprintf(buffer + offset, sizeof(buffer) - offset,
                           "{ \"id\": %s, \"nombre\": \"%s\", \"email\": \"%s\", \"rol_id\": %s, \"rol\": \"%s\" }",
                           row[0] ? row[0] : "null",
                           row[1] ? row[1] : "",
                           row[2] ? row[2] : "",
                           row[3] ? row[3] : "null",
                           row[4] ? row[4] : "");
    }

    offset += snprintf(buffer + offset, sizeof(buffer) - offset, "]");

    db_free_result(res);

    send_response(client, "200 OK", "application/json", buffer);
}


int users_service_get_rol(int client, const char *body) {

    char id_str[32];
    get_value(body, "id", id_str, sizeof(id_str));

    if (strlen(id_str) == 0) {
        send_response(client, "400 Bad Request", "application/json",
                      "{ \"status\": \"error\", \"message\": \"USER ID no recibido\" }");
        return 0;
    }

    char query_check[256];
    snprintf(query_check, sizeof(query_check),
             "SELECT COUNT(*) FROM cc_usuarios WHERE id = '%s'", id_str);
    int exists = db_query_int(query_check);

    if (exists <= 0) {
        send_response(client, "404 Not Found", "application/json",
                      "{ \"status\": \"error\", \"message\": \"Usuario no encontrado\" }");
        return 0;
    }

    char query[256];
    snprintf(query, sizeof(query),
             "SELECT rol_id FROM cc_usuarios WHERE id = '%s' LIMIT 1", id_str);

    char rol[64] = {0};

    if (db_query_single(query, rol, sizeof(rol)) != 0) {
        send_response(client, "500 Internal Server Error", "application/json",
                      "{ \"status\": \"error\", \"message\": \"Error al obtener rol del usuario\" }");
        return 0;
    }

    char response[256];
    snprintf(response, sizeof(response),
             "{ \"status\": \"ok\", \"rol\": \"%s\" }", rol);

    send_response(client, "200 OK", "application/json", response);

    return 1;
}


// handler GET /users/list
static void route_get_users_list(int client, const char *body) {

    (void)body;
    users_service_list(client,body);

}


static void route_get_users_rol(int client, const char *body){

    (void)body;
    users_service_get_rol(client,body);

}


static void route_users_update_rol(int client, const char* body){

    char error[256];
    if (users_service_update_rol(body, error, sizeof(error))) {
        send_response(client, "200 OK", "application/json",
                      "{ \"status\": \"ok\", \"message\": \"Rol Actualizado Satisfactoriamente\" }");
    } else {
        char buf[512];
        snprintf(buf, sizeof(buf),
                 "{ \"status\": \"error\", \"message\": \"%s\" }", error);
        send_response(client, "400 Bad Request", "application/json", buf);
    }
}

// ===================================================================================================================== //
// GENERADOR DE TOKENS SEGURO Y LIMPIO (Solo caracteres A-Z, a-z, 0-9)
// ===================================================================================================================== //
char* tokenGenerator(){
    static int rand_inicializado = 0;
    if (!rand_inicializado) {
        srand((unsigned int)time(NULL));
        rand_inicializado = 1;
    }

    char* token = malloc(TOKEN_LENGTH + 1);
    if(token == NULL){
        return NULL;
    }

    // 🛡️ CONJUNTO ALFANUMÉRICO LIMPIO: Evita bytes basura Unicode () y caracteres especiales de URL (#, &, ?)
    const char caracteres[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    int longitud_caracteres = sizeof(caracteres) - 1;

    for (int i = 0; i < TOKEN_LENGTH; i++) {
        int indice_aleatorio = rand() % longitud_caracteres;
        token[i] = caracteres[indice_aleatorio];
    }

    token[TOKEN_LENGTH] = '\0';
    return token;
}

// Registrar rutas
void init_users_routes(){

    add_route("GET", "/login", route_get_login);
    add_route("POST", "/users/get-user", route_get_user_by_name);
    add_route("POST", "/login", route_post_login);
    add_route("GET", "/users/list", route_get_users_list);
    add_route("POST", "/users/get-role", route_get_users_rol);
    add_route("POST", "/users/update-role", route_users_update_rol);
    add_route("POST", "/users/get-id", route_get_user_by_id);

}
