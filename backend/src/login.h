#ifndef LOGIN_H
#define LOGIN_H

static void route_get_login(int client, const char *body);
static void route_post_login(int client, const char *body);
void init_login_routes();

#endif
