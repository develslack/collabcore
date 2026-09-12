#ifndef USERS_H
#define USERS_H

// INIT
void init_user_routes();

// RUTAS
static void route_get_users(int client, const char *body);
static void route_get_register(int client, const char *body);
static void route_post_register(int client, const char *body);


#endif

