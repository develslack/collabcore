#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "routes.h"
#include "login.h"
#include "users_service.h"

// Handler GET /login
static void route_get_login(int client, const char *body) {
    (void)body;
    send_response(client, "200 OK", "application/json", "{ \"message\": \"login\" }");
}

// Handler POST /login
static void route_post_login(int client, const char *body) {

    char email[256] = {0}, password[256] = {0};

    // Extraer campos del body (form-urlencoded)
    get_value(body, "email", email, sizeof(email));
    get_value(body, "password", password, sizeof(password));

    char error_msg[256];
    UserData user; // Se declara la estructura requerida

    if (users_service_login(email, password, error_msg, &user)) {
        send_response(client, "200 OK", "application/json", "{ \"status\": \"success\", \"message\": \"Login correcto\" }");
    } else {
        char response[512];
        snprintf(response, sizeof(response),
                 "{ \"status\": \"error\", \"message\": \"%s\" }",
                 error_msg);
        send_response(client, "401 Unauthorized", "application/json", response);
    }
}

// Registrar rutas
void init_login_routes(){

    add_route("GET", "/login", route_get_login);
    add_route("POST", "/login", route_post_login);

}

