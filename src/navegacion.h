#ifndef NAVEGACION_H_
#define NAVEGACION_H_

#include "estructuras.h"
#include "sqlite3.h"
#include <winsock2.h>
#define MAX_RETOS 128 //para visualizar retos
#define MAX_USUARIOS 128 //para visaulizar el ranking

//Gestion ventanas
typedef enum {
    VENTANA_MENU_MAIN,
    VENTANA_VER_RETOS,
    VENTANA_RETO,
	VENTANA_RETO_COMPLETO,
	VENTANA_APUNTARSE,
	VENTANA_RANKING,
	VENTANA_PERFIL,
	VENTANA_ORGANIZAR_RETO,
    VENTANA_EXIT,
    VENTANA_COUNT
} TipoVentana;

typedef enum {
    FILTRO_TODOS,
    FILTRO_ACTIVOS,
    FILTRO_USUARIO,
    FILTRO_ACTIVOS_USUARIO,
    FILTRO_PARTICIPACION_USUARIO,
    FILTRO_ORGANIZADOS_USUARIO,
} FiltroRetos;

typedef enum {
    RANKING_GLOBAL,
    RANKING_RETO,
    RANKING_COUNT
} TipoRanking;

typedef struct{
	SOCKET sock;
    TipoVentana actual;
    TipoVentana historial[16];
    int historialTop;
    Usuario *usuario;
    sqlite3 *db;
    int retoSeleccionadoId;
    FiltroRetos  filtro;
    TipoRanking  tipoRanking;
} Ventana;

//Funciones de gestion
void navegar(Ventana *v, TipoVentana destino);
void volver(Ventana *v);
void ventanaLimpiar(Ventana *v);


//Funciones de las ventanas
void funcionalidadMenu(Ventana *v);
void funcionalidadPerfil(Ventana *v);
void funcionalidadVerRetos(Ventana *v);
void funcionalidadReto(Ventana *v);
void funcionalidadRetoCompleto(Ventana *v);
void funcionalidadApuntarse(Ventana *v);
void funcionalidadRanking(Ventana *v);
void funcionalidadOrganizarReto(Ventana *v);

//Gestion de Usuario
void iniciarSesion(Ventana *v);
void registrar(Ventana *v);


#endif /* NAVEGACION_H_ */
