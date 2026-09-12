#ifndef HASH_H
#define HASH_H

#include <stddef.h>

// Función dummy de verificación de password
int verify_password(const char *password, const char *hash);
void hash_password(const char *password, const char *salt, char *out_hash);
void url_decode(char *dst, const char *src);

#endif
