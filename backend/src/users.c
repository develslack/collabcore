#include "routes.h"
#include "users.h"
#include "users_service.h"

// Handlers de usuarios
static void route_get_users(int client, const char *body) {
    (void)body;
    send_response(client, "200 OK", "application/json", "[ { \"message\": \"users\" } ]");
}

// Handler GET /users/register → solo valida para mostrar el formulario
static void route_get_register(int client, const char *body) {
    (void)body;
    send_response(client, "200 OK", "application/json", "{ \"status\": \"ok\", \"message\": \"show register form\" }");
}

// Handler POST /users/register → aquí va la lógica de alta de usuario
static void route_post_register(int client, const char *body) {
    char error[256];
    if (users_service_register(body, error, sizeof(error))) {
        send_response(client, "200 OK", "application/json",
                      "{ \"status\": \"ok\", \"message\": \"Usuario Registrado Exitosamente!\" }");
    } else {
        char buf[512];
        snprintf(buf, sizeof(buf),
                 "{ \"status\": \"error\", \"message\": \"%s\" }", error);
        send_response(client, "400 Bad Request", "application/json", buf);
    }
}




// Registro de rutas
void init_user_routes() {
    add_route("GET", "/users", route_get_users);
    add_route("GET", "/users/register", route_get_register);
    add_route("POST", "/users/register", route_post_register);

}

