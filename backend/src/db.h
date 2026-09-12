#include <mysql/mysql.h>

#ifndef DB_H
#define DB_H

//#define HOST "slackzone.ddns.net"
#define HOST "localhost"
#define USER "root"
#define PASS "slack142"
#define DBASE "collab_core"
#define PORT 3306

typedef MYSQL_RES DBResult;

DBResult* db_query(const char *query);
void db_free_result(DBResult *res);

MYSQL* connect_db();
// Ejecuta un query que no devuelve resultados (INSERT, UPDATE, DELETE)
// Devuelve 0 si OK, !=0 si error
int db_execute(const char *query);

// Ejecuta un SELECT que devuelve un entero (por ejemplo, COUNT(*))
// Devuelve -1 en caso de error
int db_query_int(const char *query);

int db_query_single(const char *query, char *out_value, int out_size);

int db_get_last_id();

#endif
