#ifndef SYSTEM_STRUCT_H
#define SYSTEM_STRUCT_H

// Variables globales de lectura pública para todo el backend
extern int SYSTEM_TOTAL_NUCLEOS;      // Cantidad exacta de núcleos físicos online
extern int SYSTEM_CANTIDAD_WORKERS;   // Hilos de procesamiento paralelos sugeridos (Total - 1)

/**
 * @brief Ignora señales destructivas del Kernel (como SIGPIPE) para evitar
 * que caídas de sockets colapsen el binario principal.
 */
void system_blindar_senales(void);

/**
 * @brief Interroga al sistema operativo de forma dinámica, mapea la topología
 * de la CPU y valida los mínimos de infraestructura. Aborta si no cumple.
 */
void system_verificar_y_mapear_hardware(void);

/**
 * @brief Ata de manera fija e inamovible el hilo actual a un núcleo físico específico.
 * @param numero_nucleo Índice del núcleo (0 a N-1) al que se anclará el hilo.
 */
void system_anclar_hilo_a_nucleo(int numero_nucleo);

#endif // SYSTEM_STRUCT_H
