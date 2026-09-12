#ifndef AUTH_H
#define AUTH_H

// RUTAS
static void route_home(int client, const char *body);
static void route_password(int client, const char *body);
static void route_change_password(int client, const char *body);

// INIT
void init_auth_routes();

#endif
