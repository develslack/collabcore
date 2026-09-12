#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

MYSQL* connect_db() {
    MYSQL *conn = mysql_init(NULL);
    if (!conn) {
        fprintf(stderr, "mysql_init() failed\n");
        return NULL;
    }

    if (!mysql_real_connect(conn, HOST, USER, PASS, DBASE, PORT, NULL, 0)) {
        fprintf(stderr, "mysql_real_connect() failed: %s\n", mysql_error(conn));
        return NULL;
    }
    return conn;
}

int db_execute(const char *query) {

    MYSQL *conn = connect_db();
    if (!conn) return 1;

    if (mysql_query(conn, query)) {
        fprintf(stderr, "DB Query Error: %s\nQuery: %s\n", mysql_error(conn), query);
        mysql_close(conn);
        return 1;
    }

    mysql_close(conn);
    return 0;
}


int db_query_int(const char *query) {
    int result = -1;
    MYSQL *conn = connect_db();
    if (!conn) return -1;

    if (mysql_query(conn, query)) {
        fprintf(stderr, "DB Query Error: %s\nQuery: %s\n", mysql_error(conn), query);
        mysql_close(conn);
        return -1;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (!res) {
        mysql_close(conn);
        return -1;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row && row[0]) result = atoi(row[0]);

    mysql_free_result(res);
    mysql_close(conn);
    return result;
}


int db_query_single(const char *query, char *out_value, int out_size) {

    MYSQL* conn = connect_db();

    if (!conn) {
        fprintf(stderr, "db_query_single: No hay conexión activa\n");
        return 1;
    }

    if (mysql_query(conn, query)) {
        fprintf(stderr, "db_query_single failed: %s\n", mysql_error(conn));
        return 1;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (!res) {
        fprintf(stderr, "mysql_store_result failed: %s\n", mysql_error(conn));
        return 1;
    }

    MYSQL_ROW row = mysql_fetch_row(res);
    if (!row || !row[0]) {
        mysql_free_result(res);
        return 1; // no encontrado
    }

    strncpy(out_value, row[0], out_size - 1);
    out_value[out_size - 1] = '\0';

    mysql_free_result(res);
    return 0; // OK
}


DBResult* db_query(const char *query) {
    MYSQL *conn = connect_db();
    if (!conn) return NULL;

    if (mysql_query(conn, query)) {
        fprintf(stderr, "DB Query Error: %s\nQuery: %s\n", mysql_error(conn), query);
        mysql_close(conn);
        return NULL;
    }

    MYSQL_RES *res = mysql_store_result(conn);
    if (!res) {
        fprintf(stderr, "mysql_store_result failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }

    // 👀 Ojo: en esta versión vos cerrás la conexión acá
    mysql_close(conn);

    return res;
}

void db_free_result(DBResult *res) {
    if (res) mysql_free_result(res);
}

/**
 * @brief Obtiene el último ID autoincremental generado en la conexión actual.
 * @return long long con el ID generado, o 0 si no hubo inserción.
 */
int db_get_last_id() {

    MYSQL* conn = connect_db();

    if (conn == NULL) {
        printf("❌ Error: Conexión a DB es NULL al pedir last_id\n");
        return 0;
    }

    unsigned int last_id = (unsigned int)mysql_insert_id(conn);

    if (last_id == 0) {
        // Si devuelve 0, intentamos una consulta directa como plan B
        DBResult *res = db_query("SELECT LAST_INSERT_ID()");
        if (res) {
            MYSQL_ROW row = mysql_fetch_row(res);
            if (row && row[0]) {
                last_id = atoi(row[0]);
            }
            db_free_result(res);
        }
    }

    return (int)last_id;
}
