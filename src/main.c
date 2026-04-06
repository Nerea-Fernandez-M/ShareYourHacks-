#include <stdio.h>
#include "mysqlite.h"
#include "navegacion.h"
#include "config.h"

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

    char ruta[256];
    getcwd(ruta, sizeof(ruta));
    printf("Ejecutando desde: %s\n", ruta);
    fflush(stdout);

    // Config
	Config miConfig;
	if (cargarConfig("config.ini", &miConfig) != 0) {
		printf("Error! no se pudo leer el config.ini\n");
		return 1;
	}

	printf("Iniciando sesion para el admin: %s\n", miConfig.admin_user);





    sqlite3 *db = conectarBD(miConfig.nombre_db);
    printf("BD conectada\n");
    fflush(stdout);
    if (!db) return 1;

    printf("Inicializando BD...\n");
    fflush(stdout);
    int res = inicializarBD(db, miConfig.ruta_init);
    printf("Resultado inicializarBD: %d\n", res);
    fflush(stdout);

    if (res != SQLITE_OK) {
        desconectarBD(db);
        return 1;
    }
    printf("Entrando al bucle\n");
    fflush(stdout);

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
