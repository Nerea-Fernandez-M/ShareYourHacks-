#include <stdio.h>
#include "estructuras.h"

const char *estado_aceptacion_a_string(EstadoAceptacion estado) {
    static const char *nombres[] = {
        [EN_ESPERA]  = "En espera",
        [ACEPTADO]   = "Aceptado",
        [RECHAZADO]  = "Rechazado",
    };
    if (estado < 0 || estado >= ESTADO_ACEPTACION_COUNT)
        return "Desconocido";
    return nombres[estado];
}

const char *estado_reto_a_string(EstadoReto estado) {
    switch (estado) {
        case EN_CURSO:    return "EN_CURSO";
        case FINALIZADO:  return "FINALIZADO";
        case SIN_COMENZAR:
        default:          return "SIN_COMENZAR";
    }
}

const char *tipo_reto_a_string(TipoReto tipo) {
    static const char *nombres[] = {
        [CTF]       = "CTF",
        [HACKATHON] = "Hackathon",
    };
    if (tipo < 0 || tipo >= TIPO_RETO_COUNT)
        return "Desconocido";
    return nombres[tipo];
}

const char *dificultad_reto_a_string(DificultadReto dificultad) {
    static const char *nombres[] = {
        [FACIL]   = "Facil",
        [MEDIO]   = "Medio",
        [DIFICIL] = "Dificil",
    };
    if (dificultad < 0 || dificultad >= DIFICULTAD_RETO_COUNT)
        return "Desconocido";
    return nombres[dificultad];
}

const char *tipo_rol_a_string(TipoRol rol) {
    static const char *nombres[] = {
        [ORGANIZADOR] = "Organizador",
        [HACKER]      = "Hacker",
        [STAFF]       = "Staff",
    };
    if (rol < 0 || rol >= TIPO_ROL_COUNT)
        return "Desconocido";
    return nombres[rol];
}



