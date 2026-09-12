#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include "routes.h"
#include "auth.h"
#include "users_service.h"

// Handlers de auth (a futuro llamarán lógica de negocio en otro archivo)
static void route_home(int client, const char *body) {
    send_response(client, "200 OK", "application/json", "{ \"message\": \"home\" }");
}

static void route_password(int client, const char *body) {

    send_response(client, "200 OK", "application/json", "{ \"message\": \"password\" }");
}

static void route_change_password(int client, const char *body) {

    char error[256];
    if (users_service_password(body, error, sizeof(error))) {
        send_response(client, "200 OK", "application/json",
                      "{ \"status\": \"ok\", \"message\": \"password change successfully\" }");
    } else {
        char buf[512];
        snprintf(buf, sizeof(buf),
                 "{ \"status\": \"error\", \"message\": \"%s\" }", error);
        send_response(client, "400 Bad Request", "application/json", buf);
    }

}

// Registro de rutas
void init_auth_routes() {
    add_route("GET", "/", route_home);
    add_route("GET", "/password", route_password);
    add_route("POST", "/password", route_change_password);
}
