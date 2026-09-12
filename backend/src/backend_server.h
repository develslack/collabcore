#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 4000
#define BUFFER_SIZE 4096

#define BACKEND_ROJO  "\033[1;31m"
#define BACKEND_VERDE "\033[1;32m"
#define BACKEND_AMARILLO "\033[1;33m"
#define BACKEND_AZUL "\033[1;34m"
#define BACKEND_NORMAL "\033[0m"

void start_backend(int port);
