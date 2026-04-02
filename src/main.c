#include <stdio.h>
#include "mysqlite.h"
#include "navegacion.h"

typedef void (*GestorVentana)(Ventana *ventana);

GestorVentana gestores[VENTANA_COUNT] = {
    [VENTANA_MENU_MAIN]      = funcionalidadMenu,
    [VENTANA_VER_RETOS]      = funcionalidadVerRetos,
    [VENTANA_RETO]           = funcionalidadReto,
    [VENTANA_RETO_COMPLETO]  = funcionalidadRetoCompleto,
    [VENTANA_APUNTARSE]      = funcionalidadApuntarse,
    [VENTANA_RANKING]        = funcionalidadRanking,
    [VENTANA_PERFIL]         = funcionalidadPerfil,
    [VENTANA_ORGANIZAR_RETO] = funcionalidadOrganizarReto,
    [VENTANA_EXIT]           = NULL,   // TODO hacer funcion de salida
};

int main() {
    // Conexion con la db
    sqlite3 *db = conectarBD("shareyourhacks.db");
    if (!db) return 1;

    // Inicializar la db 
    if (inicializarBD(db) != SQLITE_OK) {
        desconectarBD(db);
        return 1;
    }

    Ventana ventana = {
        .actual        = VENTANA_MENU_MAIN,
        .historialTop = 0,
        .db            = db,
        .usuario       = NULL,
    };
    while (ventana.actual != VENTANA_EXIT) {
        gestores[ventana.actual](&ventana);
    }
    ventanaLimpiar(&ventana); //TODO limpieza antes de salir
    return 0;

}
