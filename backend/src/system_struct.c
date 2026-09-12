#define _GNU_SOURCE // Requerido en Linux para habilitar macros de afinidad de CPU
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sched.h>
#include <time.h>
#include <string.h>
#include "system_struct.h"

// Definición de las variables globales de infraestructura
int SYSTEM_TOTAL_NUCLEOS = 0;
int SYSTEM_CANTIDAD_WORKERS = 0;

// ===================================================================================================================== //
// FUNCIÓN DE ESCRITURA EN BITÁCORA DE INFRAESTRUCTURA
// ===================================================================================================================== //
static void write_system_log(const char *level, const char *message) {
    FILE *log_file = fopen("system_verification.log", "a");
    if (log_file == NULL) {
        printf("🚨 [SISTEMA - ERROR]: No se pudo escribir en system_verification.log\n");
        return;
    }

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", t);

    fprintf(log_file, "[%s] [%s]: %s\n", timestamp, level, message);
    fclose(log_file);

    if (strcmp(level, "CRITICAL") == 0) {
        printf("🚨 [%s]: %s\n", level, message);
    } else {
        printf("ℹ️  [%s]: %s\n", level, message);
    }
}

// ===================================================================================================================== //
// BLINDAJE DE SEÑALES DEL KERNEL
// ===================================================================================================================== //
void system_blindar_senales(void) {
    signal(SIGPIPE, SIG_IGN);
    write_system_log("INFO", "Blindaje perimetral contra señal SIGPIPE activado (Anti-Crash).");
}

// ===================================================================================================================== //
// DETECCION, MAPEO Y OPTIMIZACIÓN DINÁMICA DE HARDWARE
// ===================================================================================================================== //
void system_verificar_y_mapear_hardware(void) {
    // Interrogamos al kernel de Linux en tiempo real
    long nucleos_online = sysconf(_SC_NPROCESSORS_ONLN);
    char log_msg[256];

    write_system_log("INFO", "Iniciando secuencia dinámica de reconocimiento de hardware...");

    // Aplicamos tu cerrojo mínimo de seguridad para evitar despliegues en hostings deficientes
    if (nucleos_online < 4) {
        snprintf(log_msg, sizeof(log_msg),
                 "CRASH PREVENTIVO: El hardware cuenta solo con %ld núcleos activos. Se requiere un mínimo de 4 para entornos 24x365.",
                 nucleos_online);
        write_system_log("CRITICAL", log_msg);

        printf("🛑 [SISTEMA]: Recursos de CPU insuficientes para garantizar la estabilidad. Abortando.\n");
        exit(EXIT_FAILURE); // Fail-Fast radical
    }

    // Guardamos la radiografía exacta del hardware en las variables globales públicas
    SYSTEM_TOTAL_NUCLEOS = (int)nucleos_online;

    // Dejamos siempre 1 núcleo libre para atender la red (el bucle de sockets)
    // y el resto se los asignamos a los hilos trabajadores de procesamiento asíncrono
    SYSTEM_CANTIDAD_WORKERS = SYSTEM_TOTAL_NUCLEOS - 1;

    snprintf(log_msg, sizeof(log_msg),
             "HARDWARE CONFIGURADO -> Núcleos detectados: [%d] | Canales de procesamiento paralelo asignados: [%d]",
             SYSTEM_TOTAL_NUCLEOS, SYSTEM_CANTIDAD_WORKERS);
    write_system_log("SUCCESS", log_msg);
}

// ===================================================================================================================== //
// ANCLAJE FÍSICO DE HILOS (AFINIDAD DE CPU NATIVA DE LINUX)
// ===================================================================================================================== //
void system_anclar_hilo_a_nucleo(int numero_nucleo) {
    // Protección por si se intenta apuntar a un núcleo que no existe en la topología actual
    if (numero_nucleo < 0 || numero_nucleo >= SYSTEM_TOTAL_NUCLEOS) {
        char log_msg[128];
        snprintf(log_msg, sizeof(log_msg), "Intento de anclaje inválido al Núcleo #%d (Inexistente).", numero_nucleo);
        write_system_log("WARNING", log_msg);
        return;
    }

    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(numero_nucleo, &cpuset);

    char log_msg[128];
    if (sched_setaffinity(0, sizeof(cpu_set_t), &cpuset) != 0) {
        snprintf(log_msg, sizeof(log_msg), "Fallo al forzar afinidad del hilo al Núcleo Físico #%d", numero_nucleo);
        write_system_log("WARNING", log_msg);
    } else {
        snprintf(log_msg, sizeof(log_msg), "Hilo de ejecución anclado con éxito al Núcleo Físico #%d.", numero_nucleo);
        write_system_log("INFO", log_msg);
    }
}
