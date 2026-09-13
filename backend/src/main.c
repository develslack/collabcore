#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "system_struct.h"
#include "frontend_server.h"
#include "backend_server.h"
#include "roles_service.h"
#include "session_manager.h"
#include "grupos_service.h"
#include "ArrayList.h"



int main() {

    // 1. Limpieza visual de la terminal de Slackware/Debian
    system("clear");
    printf("=== AUDITORÍA DE INFRAESTRUCTURA DE SISTEMA (FAIL-FAST) ===\n");

    // 2. Activamos el blindaje de sockets primero
    system_blindar_senales();

    // 3. Freno de mano: Si hay menos de 4 núcleos, esta función imprime el error,
    // escribe en el log y ejecuta un exit(EXIT_FAILURE), frenando el main ACÁ mismo.
    system_verificar_y_mapear_hardware();

    // 4. Iniciamos session_manager como controlador del sistema
    session_manager_init();


    // ================================================================================= //
    // 🚀 SI LLEGA A ESTE PUNTO, EL HARDWARE ESTÁ BLINDADO Y ES APTO PARA 24x365
    // ================================================================================= //
    printf("\n=== INICIANDO CONFIGURACIÓN DE MEMORIA DEL MAESTRO ===\n");

    // 00. INCIALAR CACHE DE SESSION MANAGER (MODULAR)
    //session_manager_init();

    // 1. SE CREA LOS ARRAYLIST
    ArrayList* alist_roles = al_newArrayList();
    ArrayList* alist_grupos = al_newArrayList();

    // 2. Inicializar cache de roles (MODULAR)
    roles_init_cache(alist_roles);
    roles_load_storage(alist_roles);

    // 3. Inicializar cache de grupos (MODULAR)
    grupos_init_cache(alist_grupos);
    grupos_load_storage(alist_grupos);

    printf("Iniciando aplicación...\n");

    // 🔹 Levantar frontend
    start_frontend_server(4001, "../../frontend/templates/views");
    //  └── dentro de ./frontend deben estar index.html y dashboard.html

    // 🔹 Luego iniciar backend normalmente
    start_backend(4000);

    return 0;
}


