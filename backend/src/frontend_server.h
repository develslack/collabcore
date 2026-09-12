#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <pthread.h>
#include <fcntl.h>

#define FRONT_PORT 4001

#define FRONTEND_ROJO  "\033[1;31m"
#define FRONTEND_VERDE "\033[1;32m"
#define FRONTEND_AMARILLO "\033[1;33m"
#define FRONTEND_AZUL "\033[1;34m"
#define FRONTEND_NORMAL "\033[0m"

void serve_file(int client_socket, const char *file_path);
void *handle_client(void *arg);
void *frontend_server_thread(void *arg);
void start_frontend_server(int port, const char *root_path);

