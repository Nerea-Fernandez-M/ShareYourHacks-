/*
 * navegacion.cpp
 *
 * Created on: 7 may 2026
 * Author: nerea.f.m
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <winsock2.h>
#include "../src/estructuras.h"
#include "../src/navegacion.h"

// --- COMUNICACION CON EL SERVIDOR ---

// Envía un comando y espera la respuesta del servidor.
// El protocolo del procesador.c es:
//   send(comando)  →  send(param1) → … → send("CMD-END")  →  recv(respuesta)
static void enviarComando(SOCKET sock, const char *cmd) {
    char buf[64] = {0};
    strncpy(buf, cmd, sizeof(buf) - 1);
    send(sock, buf, sizeof(buf), 0);
}

static void enviarParam(SOCKET sock, const char *param, int bufSize = 64) {
    // Usa un buffer fijo dinámico igual al que espera el servidor en cada recv()
    char *buf = new char[bufSize]();
    strncpy(buf, param, bufSize - 1);
    send(sock, buf, bufSize, 0);
    delete[] buf;
}

static void enviarFin(SOCKET sock, const char *tag) {
    char buf[64] = {0};
    strncpy(buf, tag, sizeof(buf) - 1);
    send(sock, buf, sizeof(buf), 0);
}

static void recibirRespuesta(SOCKET sock, char *out, int outSize) {
    char buf[32768];
    memset(buf, 0, sizeof(buf));
    recv(sock, buf, sizeof(buf), 0);
    strncpy(out, buf, outSize - 1);
}

// --- FUNCIONES DE GESTION DE VENTANAS ---

void navegar(Ventana *v, TipoVentana destino) {
    v->historial[v->historialTop++] = v->actual;
    v->actual = destino;
}

void volver(Ventana *v) {
    if (v->historialTop > 0)
        v->actual = v->historial[--v->historialTop];
    else
        v->actual = VENTANA_MENU_MAIN;
}

void ventanaLimpiar(Ventana *v) {
    if (v->usuario != NULL) {
        free(v->usuario);
        v->usuario = NULL;
    }
}

// --- FUNCIONES DE VENTANAS ---

void funcionalidadMenu(Ventana *v) {

    // Print del texto
    printf("----Bienvenid@ a ShareYourHacks -----\n"); fflush(stdout);
    printf("1) Ver tus retos activos\n"); fflush(stdout);
    printf("2) Ver proximos retos\n"); fflush(stdout);
    printf("3) Ver todos los retos\n"); fflush(stdout);
    printf("4) Ver tu perfil\n"); fflush(stdout);
    printf("5) Ver Ranking\n"); fflush(stdout);
    printf("6) Organizar un reto\n"); fflush(stdout);
    printf("0) Salir de la aplicacion\n"); fflush(stdout);
    printf("Introduce un numero: "); fflush(stdout);

    int opcion;
    scanf("%d", &opcion);
    while (getchar() != '\n');

    switch (opcion) {
        case 1:
            if (v->usuario == NULL) {
                printf("Debes iniciar sesion para ver tus retos activos.\n");
                break;
            }
            v->filtro = FILTRO_ACTIVOS_USUARIO;
            navegar(v, VENTANA_VER_RETOS);
            break;
        case 2:
            v->filtro = FILTRO_ACTIVOS;
            navegar(v, VENTANA_VER_RETOS);
            break;
        case 3:
            v->filtro = FILTRO_TODOS;
            navegar(v, VENTANA_VER_RETOS);
            break;
        case 4:
            navegar(v, VENTANA_PERFIL);
            break;
        case 5:
            v->tipoRanking = RANKING_GLOBAL;
            navegar(v, VENTANA_RANKING);
            break;
        case 6:
            if (v->usuario == NULL) {
                printf("Debes iniciar sesion para organizar un reto.\n");
                break;
            }
            navegar(v, VENTANA_ORGANIZAR_RETO);
            break;
        case 0:
            v->actual = VENTANA_EXIT;
            break;
        default:
            printf("Opcion no valida, intentalo de nuevo.\n"); fflush(stdout);
            break;
    }
}

// ─────────────────────────────────────────────

void funcionalidadPerfil(Ventana *v) {

    if (v->usuario != NULL) {

        // pedir datos de perfil al servidor
        char id_str[16];
        snprintf(id_str, sizeof(id_str), "%d", v->usuario->id);

        enviarComando(v->sock, "GET_PERFIL");
        enviarParam(v->sock, id_str, 16);
        enviarFin(v->sock, "GET_PERFIL-END");

        char resp[32768];
        recibirRespuesta(v->sock, resp, sizeof(resp));

        int ranking = 0, puntos_activos = 0, puntos_organizados = 0;
        sscanf(resp, "%d|%d|%d", &ranking, &puntos_activos, &puntos_organizados);

        // print texto
        printf("Hola %s!\n", v->usuario->nombre); fflush(stdout);
        printf("Tu ranking: #%d\n", ranking); fflush(stdout);
        printf("0) Para volver al menu\n"); fflush(stdout);
        printf("1) Ver tus retos activos  (%d pts)\n", puntos_activos); fflush(stdout);
        printf("2) Ver tus retos organizados (%d pts)\n", puntos_organizados); fflush(stdout);
        printf("Que quieres hacer:  \n"); fflush(stdout);

        int opcion;
        scanf("%d", &opcion);
        while (getchar() != '\n');

        switch (opcion) {
        case 1:
            v->filtro = FILTRO_ACTIVOS_USUARIO;
            navegar(v, VENTANA_VER_RETOS);
            break;
        case 2:
            v->filtro = FILTRO_ORGANIZADOS_USUARIO;
            navegar(v, VENTANA_VER_RETOS);
            break;
        case 0:
            volver(v);
            break;
        default:
            printf("Opcion no valida, intentalo de nuevo.\n"); fflush(stdout);
            break;
        }

    } else { // sesion no iniciada

        printf("Quieres iniciar sesion (1) o registrarte (2) (0 para volver): "); fflush(stdout);

        int opcion;
        scanf("%d", &opcion);
        while (getchar() != '\n');

        switch (opcion) {
        case 1:
            iniciarSesion(v);
            break;
        case 2:
            registrar(v);
            break;
        case 0:
            volver(v);
            break;
        default:
            printf("Opcion no valida.\n"); fflush(stdout);
            break;
        }
    }
}

// ─────────────────────────────────────────────

void funcionalidadVerRetos(Ventana *v) {

    // pedir lista de retos al servidor
    const char *filtro_str = "TODOS";
    switch (v->filtro) {
        case FILTRO_TODOS:           filtro_str = "TODOS";          break;
        case FILTRO_ACTIVOS:         filtro_str = "ACTIVOS";        break;
        case FILTRO_ACTIVOS_USUARIO: filtro_str = "ACTIVOS_USUARIO"; break;
        case FILTRO_ORGANIZADOS_USUARIO: filtro_str = "ORGANIZADOS"; break;
        default:                     filtro_str = "TODOS";          break;
    }

    char id_usuario_str[16] = "0";
    if (v->usuario != NULL)
        snprintf(id_usuario_str, sizeof(id_usuario_str), "%d", v->usuario->id);

    enviarComando(v->sock, "GET_RETOS");
    enviarParam(v->sock, filtro_str, 64);
    enviarParam(v->sock, id_usuario_str, 64);
    enviarFin(v->sock, "GET_RETOS-END");

    char resp[32768];
    recibirRespuesta(v->sock, resp, sizeof(resp));

    Reto retos[MAX_RETOS];
    int cantidad = 0;

    if (strncmp(resp, "VACIO", 5) != 0) {
        char *linea = strtok(resp, "\n");
        while (linea != NULL && cantidad < MAX_RETOS) {
            int estado_int = 0, puntos = 0;
            char estado_str[16] = "";
            sscanf(linea, "%d|%49[^|]|%15[^|]|%d",
                   &retos[cantidad].id,
                   retos[cantidad].titulo,
                   estado_str,
                   &puntos);
            retos[cantidad].puntos = puntos;

            if      (strcmp(estado_str, "SIN_COMENZAR") == 0) retos[cantidad].estadoReto = SIN_COMENZAR;
            else if (strcmp(estado_str, "EN_CURSO")      == 0) retos[cantidad].estadoReto = EN_CURSO;
            else                                               retos[cantidad].estadoReto = FINALIZADO;
            cantidad++;
            linea = strtok(NULL, "\n");
        }
    }

    // print de retos
    if (cantidad == 0) {
        printf("No hay retos para mostrar.\n"); fflush(stdout);
    } else {
        for (int i = 0; i < cantidad; i++) {
            printf("%d) %-40s | %s | %d pts\n",
                   i + 1,
                   retos[i].titulo,
                   estado_reto_a_string(retos[i].estadoReto),
                   retos[i].puntos);
        }
    }

    printf("Pulsa el numero del reto cuyos datos quieras ver\n"); fflush(stdout);
    printf("Pulsa 0 para volver a la ventana anterior\n"); fflush(stdout);
    printf("Que quieres hacer: "); fflush(stdout);

    int opcion;
    scanf("%d", &opcion);
    while (getchar() != '\n');

    if (opcion == 0) { volver(v); return; }

    if (opcion >= 1 && opcion <= cantidad) {
        v->retoSeleccionadoId = retos[opcion - 1].id;
        navegar(v, VENTANA_RETO);
        return;
    }

    printf("Opcion no valida, intentalo de nuevo.\n"); fflush(stdout);
}

// ─────────────────────────────────────────────

void funcionalidadReto(Ventana *v) {

    // obtener datos basicos del reto
    char id_str[16];
    snprintf(id_str, sizeof(id_str), "%d", v->retoSeleccionadoId);

    enviarComando(v->sock, "GET_RETO");
    enviarParam(v->sock, id_str, 64);
    enviarFin(v->sock, "GET_RETO-END");

    char resp[32768];
    recibirRespuesta(v->sock, resp, sizeof(resp));

    Reto reto;
    memset(&reto, 0, sizeof(reto));
    char estado_str[16] = "", tipo_str[16] = "", dif_str[16] = "";
    int id_org = 0;

    sscanf(resp, "%d|%49[^|]|%199[^|]|%15[^|]|%15[^|]|%19[^|]|%19[^|]|%15[^|]|%d|%d",
           &reto.id,
           reto.titulo,
           reto.descripcion,
           estado_str,
           tipo_str,
           reto.fecha_i_inscripcion,
           reto.fecha_f_inscripcion,
           dif_str,
           &reto.puntos,
           &id_org);
    reto.id_organizador = id_org;

    if      (strcmp(estado_str, "SIN_COMENZAR") == 0) reto.estadoReto = SIN_COMENZAR;
    else if (strcmp(estado_str, "EN_CURSO")      == 0) reto.estadoReto = EN_CURSO;
    else                                               reto.estadoReto = FINALIZADO;

    if      (strcmp(tipo_str, "CTF")       == 0) reto.tipoReto = CTF;
    else                                         reto.tipoReto = HACKATHON;

    if      (strcmp(dif_str, "FACIL")   == 0) reto.dificultadReto = FACIL;
    else if (strcmp(dif_str, "MEDIO")   == 0) reto.dificultadReto = MEDIO;
    else                                       reto.dificultadReto = DIFICIL;

    // obtener nombre del organizador
    char id_org_str[16];
    snprintf(id_org_str, sizeof(id_org_str), "%d", id_org);

    enviarComando(v->sock, "GET_USUARIO");
    enviarParam(v->sock, id_org_str, 64);
    enviarFin(v->sock, "GET_USUARIO-END");

    char resp2[32768];
    recibirRespuesta(v->sock, resp2, sizeof(resp2));

    char nombre_org[30] = "";
    if (strncmp(resp2, "OK", 2) == 0)
        sscanf(resp2, "OK|%29[^|]", nombre_org);

    // mostrar segun estado

    if (reto.estadoReto == SIN_COMENZAR) {

        printf("%s\n", reto.titulo); fflush(stdout);
        printf("Creado por:%s \n", nombre_org); fflush(stdout);
        printf("Resumen: %s\n", reto.descripcion); fflush(stdout);
        printf("Tipo de reto: %s\n", tipo_reto_a_string(reto.tipoReto)); fflush(stdout);
        printf("Inscripcion: %s-%s\n", reto.fecha_i_inscripcion, reto.fecha_f_inscripcion); fflush(stdout);
        printf("Dificultad: %s\n", dificultad_reto_a_string(reto.dificultadReto)); fflush(stdout);
        printf("Puntos: %d\n", reto.puntos); fflush(stdout);
        printf("Pulsa 1 para apuntarte o 0 para volver a la pagina anterior:  \n"); fflush(stdout);

        int opcion;
        scanf("%d", &opcion);
        while (getchar() != '\n');

        switch (opcion) {
            case 0: volver(v); break;
            case 1:
                if (v->usuario == NULL) {
                    printf("Debes iniciar sesion para inscribirte.\n");
                    break;
                }
                navegar(v, VENTANA_RETO_COMPLETO);
                break;
            default: printf("Opcion no valida, intentalo de nuevo.\n"); fflush(stdout); break;
        }

    } else if (reto.estadoReto == EN_CURSO) {

        // datos extras (rol, puesto, dias)
        char id_usuario_str[16] = "0";
        if (v->usuario != NULL)
            snprintf(id_usuario_str, sizeof(id_usuario_str), "%d", v->usuario->id);

        enviarComando(v->sock, "GET_RETO_EXTRA");
        enviarParam(v->sock, id_str, 64);
        enviarParam(v->sock, id_usuario_str, 64);
        enviarFin(v->sock, "GET_RETO_EXTRA-END");

        char resp3[32768];
        recibirRespuesta(v->sock, resp3, sizeof(resp3));

        TipoRol rol = HACKER;
        int puesto = 0, dias_restantes = 0;
        if (strncmp(resp3, "OK", 2) == 0) {
            int rol_int = 0;
            sscanf(resp3, "OK|%d|%d|%d", &rol_int, &puesto, &dias_restantes);
            rol = (TipoRol)rol_int;
        }

        printf("%s\n", reto.titulo); fflush(stdout);
        printf("Creado por:%s \n", nombre_org); fflush(stdout);
        printf("Resumen: %s\n", reto.descripcion); fflush(stdout);
        printf("Tu rol: %s\n", tipo_rol_a_string(rol)); fflush(stdout);
        printf("Tipo de reto: %s\n", tipo_reto_a_string(reto.tipoReto)); fflush(stdout);
        printf("Tiempo restante: %d dias\n", dias_restantes); fflush(stdout);
        printf("Tu puesto: #%d\n", puesto); fflush(stdout);
        printf("1) Ver detalles\n"); fflush(stdout);
        printf("2) Ver ranking del reto\n"); fflush(stdout);
        printf("0) Volver a la pantalla anterior\n"); fflush(stdout);
        printf("Que quieres hacer: \n"); fflush(stdout);

        int opcion;
        scanf("%d", &opcion);
        while (getchar() != '\n');

        switch (opcion) {
        case 0: volver(v); break;
        case 1: navegar(v, VENTANA_RETO_COMPLETO); break;
        case 2: v->tipoRanking = RANKING_RETO; navegar(v, VENTANA_RANKING); break;
        default: printf("Opcion no valida, intentalo de nuevo.\n"); fflush(stdout); break;
        }

    } else {

        printf("%s\n", reto.titulo); fflush(stdout);
        printf("Creado por:%s\n", nombre_org); fflush(stdout);
        printf("Resumen: %s\n", reto.descripcion); fflush(stdout);
        printf("Este reto ya ha finalizado\n"); fflush(stdout);
        printf("Tipo de reto: %s\n", tipo_reto_a_string(reto.tipoReto)); fflush(stdout);
        printf("Pulsa 1 para ver el ranking del reto o 0 para volver a la ventana anterior:  \n"); fflush(stdout);

        int opcion;
        scanf("%d", &opcion);
        while (getchar() != '\n');

        switch (opcion) {
        case 0: volver(v); break;
        case 1: v->tipoRanking = RANKING_RETO; navegar(v, VENTANA_RANKING); break;
        default: printf("Opcion no valida, intentalo de nuevo.\n"); fflush(stdout); break;
        }
    }
}

// ─────────────────────────────────────────────

void funcionalidadRetoCompleto(Ventana *v) {

    char id_str[16];
    snprintf(id_str, sizeof(id_str), "%d", v->retoSeleccionadoId);

    enviarComando(v->sock, "GET_RETO");
    enviarParam(v->sock, id_str, 64);
    enviarFin(v->sock, "GET_RETO-END");

    char resp[32768];
    recibirRespuesta(v->sock, resp, sizeof(resp));

    Reto reto;
    memset(&reto, 0, sizeof(reto));
    char estado_str[16] = "", tipo_str[16] = "", dif_str[16] = "";
    int id_org = 0;
    sscanf(resp, "%d|%49[^|]|%199[^|]|%15[^|]|%15[^|]|%19[^|]|%19[^|]|%15[^|]|%d|%d",
           &reto.id, reto.titulo, reto.descripcion,
           estado_str, tipo_str,
           reto.fecha_i_inscripcion, reto.fecha_f_inscripcion,
           dif_str, &reto.puntos, &id_org);

    if      (strcmp(tipo_str, "CTF") == 0) reto.tipoReto = CTF;
    else                                   reto.tipoReto = HACKATHON;

    // nombre del organizador
    char id_org_str[16];
    snprintf(id_org_str, sizeof(id_org_str), "%d", id_org);
    enviarComando(v->sock, "GET_USUARIO");
    enviarParam(v->sock, id_org_str, 64);
    enviarFin(v->sock, "GET_USUARIO-END");

    char resp2[32768];
    recibirRespuesta(v->sock, resp2, sizeof(resp2));

    char nombre_org[30] = "";
    if (strncmp(resp2, "OK", 2) == 0)
        sscanf(resp2, "OK|%29[^|]", nombre_org);

    // rol y puesto del usuario
    char id_usuario_str[16] = "0";
    if (v->usuario != NULL)
        snprintf(id_usuario_str, sizeof(id_usuario_str), "%d", v->usuario->id);

    enviarComando(v->sock, "GET_RETO_EXTRA");
    enviarParam(v->sock, id_str, 64);
    enviarParam(v->sock, id_usuario_str, 64);
    enviarFin(v->sock, "GET_RETO_EXTRA-END");

    char resp3[32768];
    recibirRespuesta(v->sock, resp3, sizeof(resp3));

    int puesto = 0;
    if (strncmp(resp3, "OK", 2) == 0) {
        int rol_int = 0, dias = 0;
        sscanf(resp3, "OK|%d|%d|%d", &rol_int, &puesto, &dias);
    }

    // print texto
    printf("%s\n", reto.titulo); fflush(stdout);
    printf("Creado por:%s \n", nombre_org); fflush(stdout);
    printf("Resumen: %s\n", reto.descripcion); fflush(stdout);
    printf("Tipo de reto: %s\n", tipo_reto_a_string(reto.tipoReto)); fflush(stdout);
    printf("Tu puesto: #%d\n", puesto); fflush(stdout);
    printf("Pulsa 1 para descargar archivos adicionales o 0 para volver al menu: "); fflush(stdout);

    int opcion;
    scanf("%d", &opcion);
    while (getchar() != '\n');

    switch (opcion) {
    case 0: volver(v); break;
    case 1:
        printf("No hay archivos extra para este reto\n"); fflush(stdout);
        navegar(v, VENTANA_RETO);
        break;
    default:
        printf("Opcion no valida, intentalo de nuevo.\n"); fflush(stdout);
        break;
    }
}

// ─────────────────────────────────────────────

void funcionalidadApuntarse(Ventana *v) {

    char id_str[16];
    snprintf(id_str, sizeof(id_str), "%d", v->retoSeleccionadoId);

    enviarComando(v->sock, "GET_RETO");
    enviarParam(v->sock, id_str, 64);
    enviarFin(v->sock, "GET_RETO-END");

    char resp[32768];
    recibirRespuesta(v->sock, resp, sizeof(resp));

    Reto reto;
    memset(&reto, 0, sizeof(reto));
    char aux1[16], aux2[16], aux3[16], aux4[16], aux5[16], aux6[16];
    int  aux_int = 0;
    sscanf(resp, "%d|%49[^|]|%199[^|]|%15[^|]|%15[^|]|%19[^|]|%19[^|]|%15[^|]|%d|%d",
           &reto.id, reto.titulo, reto.descripcion,
           aux1, aux2, aux3, aux4, aux5,
           &reto.puntos, &aux_int);

    printf("%s\n", reto.titulo); fflush(stdout);

    // elegir rol
    printf("Pulsa 1 para ser Hacker o 2 para ser miembro del Staff: "); fflush(stdout);
    int opcion_rol;
    scanf("%d", &opcion_rol);
    while (getchar() != '\n');

    // escribir la motivacion
    char motivacion[256];
    printf("Escribe brevemente tu motivacion para participar en este reto: "); fflush(stdout);
    fgets(motivacion, sizeof(motivacion), stdin);
    motivacion[strcspn(motivacion, "\n")] = '\0';

    // Confirmar
    printf("Pulsa 1 para apuntarte o 0 para volver al menu: "); fflush(stdout);
    int confirmar;
    scanf("%d", &confirmar);
    while (getchar() != '\n');

    if (confirmar == 0) { volver(v); return; }

    // equipo
    enviarComando(v->sock, "RETO_REQUIERE_EQUIPO");
    enviarParam(v->sock, id_str, 64);
    enviarFin(v->sock, "RETO_REQUIERE_EQUIPO-END");

    char resp2[32768];
    recibirRespuesta(v->sock, resp2, sizeof(resp2));

    int requiere_equipo = 0;
    if (strncmp(resp2, "OK", 2) == 0)
        sscanf(resp2, "OK|%d", &requiere_equipo);

    char id_usuario_str[16];
    snprintf(id_usuario_str, sizeof(id_usuario_str), "%d", v->usuario->id);
    char id_reto_str[16];
    snprintf(id_reto_str, sizeof(id_reto_str), "%d", reto.id);

    if (!requiere_equipo) {
        enviarComando(v->sock, "INSCRIBIR");
        enviarParam(v->sock, id_usuario_str, 64);
        enviarParam(v->sock, id_reto_str,    64);
        enviarParam(v->sock, "0",            64);
        enviarParam(v->sock, motivacion,    256); // Este al ser largo lo dejamos en un búfer mayor definido
        enviarFin(v->sock, "INSCRIBIR-END");

        char resp3[32768];
        recibirRespuesta(v->sock, resp3, sizeof(resp3));

        if (strncmp(resp3, "OK", 2) == 0)
            printf("Te has inscrito en %s correctamente.\n", reto.titulo);
        else
            printf("Algo ha ido mal al inscribirse, intentalo de nuevo.\n");
        fflush(stdout);
        volver(v);
        return;
    }

    // gestionar equipo
    printf("Pulsa 1 para unirte a un equipo o 2 para crear uno: "); fflush(stdout);
    int opcion_equipo;
    scanf("%d", &opcion_equipo);
    while (getchar() != '\n');

    int id_equipo = 0;
    printf("(Gestion de equipos pendiente de implementar en el servidor)\n"); fflush(stdout);

    char id_equipo_str[16];
    snprintf(id_equipo_str, sizeof(id_equipo_str), "%d", id_equipo);

    enviarComando(v->sock, "INSCRIBIR");
    enviarParam(v->sock, id_usuario_str,  64);
    enviarParam(v->sock, id_reto_str,     64);
    enviarParam(v->sock, id_equipo_str,   64);
    enviarParam(v->sock, motivacion,     256);
    enviarFin(v->sock, "INSCRIBIR-END");

    char resp4[32768];
    recibirRespuesta(v->sock, resp4, sizeof(resp4));

    if (strncmp(resp4, "OK", 2) == 0)
        printf("Te has inscrito en %s correctamente.\n", reto.titulo);
    else
        printf("Algo ha ido mal al inscribirse, intentalo de nuevo.\n");
    fflush(stdout);
    volver(v);
}

// ─────────────────────────────────────────────

void funcionalidadRanking(Ventana *v) {

    const char *tipo_str = (v->tipoRanking == RANKING_GLOBAL) ? "GLOBAL" : "RETO";
    char id_reto_str[16];
    snprintf(id_reto_str, sizeof(id_reto_str), "%d", v->retoSeleccionadoId);

    enviarComando(v->sock, "GET_RANKING");
    enviarParam(v->sock, tipo_str,    64);
    enviarParam(v->sock, id_reto_str, 64);
    enviarFin(v->sock, "GET_RANKING-END");

    char resp[32768];
    recibirRespuesta(v->sock, resp, sizeof(resp));

    if (v->tipoRanking == RANKING_GLOBAL)
        printf("=== Ranking global ===\n");
    else
        printf("=== Ranking del reto ===\n");
    fflush(stdout);

    if (strncmp(resp, "VACIO", 5) == 0) {
        printf("No hay datos para mostrar.\n"); fflush(stdout);
    } else {
        int pos = 1;
        char *linea = strtok(resp, "\n");
        while (linea != NULL) {
            char nombre[30] = "";
            int  pts = 0;
            sscanf(linea, "%29[^|]|%d", nombre, &pts);
            printf("#%-3d %-30s %d pts\n", pos++, nombre, pts);
            fflush(stdout);
            linea = strtok(NULL, "\n");
        }
    }

    printf("Pulsa 0 para volver: "); fflush(stdout);
    int opcion;
    scanf("%d", &opcion);
    while (getchar() != '\n');
    volver(v);
}

// ─────────────────────────────────────────────

void funcionalidadOrganizarReto(Ventana *v) {

    char titulo[128], resumen[512], tipo[16];
    char fecha_inicio_insc[11], fecha_fin_insc[11];
    char fecha_inicio[11], fecha_fin[11];
    char dificultad[16];
    int  puntos = 0, limite_de_plazas = 0;

    printf("Introduce los siguientes datos de tu reto\n"); fflush(stdout);

    printf("Titulo del reto: \n"); fflush(stdout);
    fgets(titulo, sizeof(titulo), stdin);
    titulo[strcspn(titulo, "\n")] = '\0';

    printf("Resumen: \n"); fflush(stdout);
    fgets(resumen, sizeof(resumen), stdin);
    resumen[strcspn(resumen, "\n")] = '\0';

    while (1) {
        printf("Tipo (CTF / HACKATHON): \n"); fflush(stdout);
        scanf("%15s", tipo);
        while (getchar() != '\n');
        for (int i = 0; tipo[i]; i++) tipo[i] = toupper(tipo[i]);
        if (strcmp(tipo, "CTF") == 0 || strcmp(tipo, "HACKATHON") == 0) break;
        printf("Tipo no valido, escribe CTF o HACKATHON.\n"); fflush(stdout);
    }

    printf("Escribe las siguientes fechas en formato (dd/mm/aaaa)\n"); fflush(stdout);

    printf("Inicio inscripcion: \n"); fflush(stdout);
    scanf("%10s", fecha_inicio_insc); while (getchar() != '\n');

    printf("Fin inscripcion: \n"); fflush(stdout);
    scanf("%10s", fecha_fin_insc); while (getchar() != '\n');

    printf("Inicio reto: \n"); fflush(stdout);
    scanf("%10s", fecha_inicio); while (getchar() != '\n');

    printf("Fin reto: \n"); fflush(stdout);
    scanf("%10s", fecha_fin); while (getchar() != '\n');

    while (1) {
        printf("Dificultad (FACIL / MEDIO / DIFICIL): \n"); fflush(stdout);
        scanf("%15s", dificultad); while (getchar() != '\n');
        for (int i = 0; dificultad[i]; i++) dificultad[i] = toupper(dificultad[i]);
        if (strcmp(dificultad, "FACIL")  == 0 ||
            strcmp(dificultad, "MEDIO")  == 0 ||
            strcmp(dificultad, "DIFICIL") == 0) break;
        printf("Dificultad no valida, escribe FACIL, MEDIO o DIFICIL.\n"); fflush(stdout);
    }

    printf("Puntos: \n"); fflush(stdout);
    scanf("%d", &puntos); while (getchar() != '\n');

    printf("Limite de plazas: \n"); fflush(stdout);
    scanf("%d", &limite_de_plazas); while (getchar() != '\n');

    printf("Para crear el reto pulsa 1, para volver al menu pulsa 0: \n"); fflush(stdout);
    int confirmar;
    scanf("%d", &confirmar); while (getchar() != '\n');

    if (confirmar == 0) { volver(v); return; }

    char puntos_str[16], plazas_str[16], id_org_str[16];
    snprintf(puntos_str,  sizeof(puntos_str),  "%d", puntos);
    snprintf(plazas_str,  sizeof(plazas_str),  "%d", limite_de_plazas);
    snprintf(id_org_str,  sizeof(id_org_str),  "%d", v->usuario->id);

    enviarComando(v->sock, "CREAR_RETO");
    enviarParam(v->sock, titulo, 128);
    enviarParam(v->sock, resumen, 512);
    enviarParam(v->sock, tipo, 64);
    enviarParam(v->sock, fecha_inicio_insc, 64);
    enviarParam(v->sock, fecha_fin_insc, 64);
    enviarParam(v->sock, fecha_inicio, 64);
    enviarParam(v->sock, fecha_fin, 64);
    enviarParam(v->sock, dificultad, 64);
    enviarParam(v->sock, puntos_str, 64);
    enviarParam(v->sock, plazas_str, 64);
    enviarParam(v->sock, id_org_str, 64);
    enviarFin(v->sock, "CREAR_RETO-END");

    char resp[32768];
    recibirRespuesta(v->sock, resp, sizeof(resp));

    if (strncmp(resp, "OK", 2) == 0)
        printf("Reto \"%s\" creado correctamente.\n", titulo);
    else
        printf("Algo ha ido mal al crear el reto, intentalo de nuevo.\n");
    fflush(stdout);
    volver(v);
}

// --- GESTIONES DE USER (login...) ---

void iniciarSesion(Ventana *v) {

    char nombre[64], contrasena[128];

    while (1) {
        printf("Nombre de usuario: "); fflush(stdout);
        scanf("%63s", nombre); while (getchar() != '\n');

        printf("Contraseña: "); fflush(stdout);
        scanf("%127s", contrasena); while (getchar() != '\n');

        enviarComando(v->sock, "LOGIN");
        enviarParam(v->sock, nombre,    64);
        enviarParam(v->sock, contrasena, 128);
        enviarFin(v->sock, "LOGIN-END");

        char resp[32768];
        recibirRespuesta(v->sock, resp, sizeof(resp));

        if (strncmp(resp, "OK", 2) == 0) {
            int id = 0, puntos = 0;
            char nombre_resp[64] = "";
            sscanf(resp, "OK|%d|%d|%63s", &id, &puntos, nombre_resp);

            v->usuario = (Usuario *)malloc(sizeof(Usuario));
            v->usuario->id           = id;
            v->usuario->total_puntos = puntos;
            strncpy(v->usuario->nombre, nombre_resp, sizeof(v->usuario->nombre) - 1);

            navegar(v, VENTANA_PERFIL);
            return;
        }

        printf("Nombre de usuario o contraseña incorrectos\n"); fflush(stdout);
        printf("Pulsa 1 para intentarlo de nuevo o 0 para volver: "); fflush(stdout);

        int opcion;
        scanf("%d", &opcion); while (getchar() != '\n');

        if (opcion == 0) { volver(v); return; }
    }
}

void registrar(Ventana *v) {

    char email[128], nombre[64], contrasena[128], contrasena2[128];

    while (1) {
        printf("Correo electronico: "); fflush(stdout);
        scanf("%127s", email); while (getchar() != '\n');

        printf("Nombre de usuario: "); fflush(stdout);
        scanf("%63s", nombre); while (getchar() != '\n');

        printf("Contraseña: "); fflush(stdout);
        scanf("%127s", contrasena); while (getchar() != '\n');

        printf("Repetir contraseña: "); fflush(stdout);
        scanf("%127s", contrasena2); while (getchar() != '\n');

        if (strcmp(contrasena, contrasena2) != 0) {
            printf("Las contraseñas no coinciden, intentalo de nuevo.\n\n"); fflush(stdout);
            continue;
        }

        enviarComando(v->sock, "REGISTRAR");
        enviarParam(v->sock, email,     128);
        enviarParam(v->sock, nombre,     64);
        enviarParam(v->sock, contrasena, 128);
        enviarFin(v->sock, "REGISTRAR-END");

        char resp[32768];
        recibirRespuesta(v->sock, resp, sizeof(resp));

        if (strncmp(resp, "OK", 2) == 0) {
            int id = 0;
            sscanf(resp, "OK|%d", &id);

            v->usuario = (Usuario *)malloc(sizeof(Usuario));
            v->usuario->id           = id;
            v->usuario->total_puntos = 0;
            strncpy(v->usuario->nombre, nombre, sizeof(v->usuario->nombre) - 1);

            navegar(v, VENTANA_PERFIL);
            return;
        }

        printf("No se ha podido registrar el usuario, intentalo de nuevo.\n\n"); fflush(stdout);
    }
}
