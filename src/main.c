#include <stdio.h>
#include "mysqlite.h"

int main() {
    // Conexion con la db
    sqlite3 *db = conectarBD("shareyourhacks.db");
    if (!db) return 1;

    // Inicializar la db 
    if (inicializarBD(db) != SQLITE_OK) {
        desconectarBD(db);
        return 1;
    }
}