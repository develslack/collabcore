#!/bin/bash
clear
echo "# ======================================================= #"
echo "COMPILANDO APP (CollabCore)"
echo "# ======================================================= #"

gcc -Wall -g -std=c11  -o backend/bin/collabcore \
    backend/src/ArrayList.h backend/src/ArrayList.c \
    backend/src/commonlib.h backend/src/commonlib.c \
    backend/src/db.h backend/src/db.c \
    backend/src/auth.h backend/src/auth.c \
    backend/src/users.h backend/src/users.c \
    backend/src/users_service.h backend/src/users_service.c \
    backend/src/routes.h backend/src/routes.c \
    backend/src/frontend_server.h backend/src/frontend_server.c \
    backend/src/backend_server.h backend/src/backend_server.c \
    backend/src/roles_service.h backend/src/roles_service.c \
    backend/src/session_manager.h backend/src/session_manager.c \
    backend/src/login.h backend/src/login.c \
    backend/src/hash.h backend/src/hash.c \
    backend/src/system_struct.h backend/src/system_struct.c \
    backend/src/grupos_service.h backend/src/grupos_service.c \
    backend/src/main.c \
    -lpthread -lmysqlclient -lssl -lcrypto 2>&1 | tee comp_err.txt

if [ -f backend/bin/collabcore ]; then
    clear
    echo "# ======================================================= #"
    echo "✅ COMPILACIÓN EXITOSA"
    echo "# ======================================================= #"
    echo "¿Qué acción desea realizar?"
    echo "  [1] Ejecutar en entorno de desarrollo"
    echo "  [2] Instalar en /opt/CollabCore/ (Requiere sudo)"
    echo "  [3] Salir"
    echo "# ======================================================= #"
    read -p "Seleccione una opción [1-3]: " resp

    case $resp in
        1)
            clear
            echo "Iniciando CollabCore en modo desarrollo..."
            cd backend/bin/
            ./collabcore
            ;;
        2)
            clear
            echo "Iniciando instalación en /opt/CollabCore/..."
            DEST_DIR="/opt/CollabCore"

            echo "=> Creando estructura de directorios..."
            sudo mkdir -p $DEST_DIR/backend/bin/
            sudo mkdir -p $DEST_DIR/frontend

            echo "=> Copiando binario del backend..."
            sudo cp backend/bin/collabcore $DEST_DIR/backend/bin/
            sudo cp backend/bin/server.log $DEST_DIR/backend/bin/
            sudo cp backend/bin/system_verification.log $DEST_DIR/backend/bin/

            echo "=> Copiando archivos del frontend..."
            sudo cp -r frontend/* $DEST_DIR/frontend/

            echo "=> Estableciendo permisos de escritura para directorios dinámicos..."
            sudo chmod 777 $DEST_DIR/backend/bin

            echo "# ======================================================= #"
            echo "✅ Instalación completada con éxito."
            echo "   Ruta: $DEST_DIR"
            echo "# ======================================================= #"
            ;;
        3)
            clear
            echo "Saliendo. Solo se dejó el binario compilado en backend/bin/"
            ;;
        *)
            clear
            echo "Opción inválida. Saliendo..."
            ;;
    esac

else
    clear
    echo "# ======================================================= #"
    echo "❌ Error en la compilación. Revisa el archivo comp_err.txt"
    echo "# ======================================================= #"
fi
