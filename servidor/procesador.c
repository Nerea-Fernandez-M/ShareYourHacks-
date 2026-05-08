#include "procesador.h"
#include "mysqlite.h"
#inlcude "estructuras.h"
#include <string.h>
#include <stdio.h>

// Función principal que recibe el comando y el socket
void procesarComando(SOCKET comm_socket, sqlite3 *db, char *comando) {

    char recvBuff[1024];
    char sendBuff[1024];

    // ---- LOGIN ----
    if (strcmp(comando, "LOGIN") == 0) {

        // Recibir parámetros
        char nombre[64], contrasena[128];
        recv(comm_socket, nombre, sizeof(nombre),     0);
        recv(comm_socket, contrasena, sizeof(contrasena), 0);
        recv(comm_socket, recvBuff, sizeof(recvBuff),   0); // LOGIN-END

        // Llamar a la BD
        int id = 0, puntos = 0;
        int res = obtenerUsuarioPorCredenciales(db, nombre, contrasena, &id, &puntos);

        // Responder al cliente
        if (res == SQLITE_OK)
            sprintf(sendBuff, "OK|%d|%d|%s", id, puntos, nombre);
        else
            strcpy(sendBuff, "ERROR");

        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
    }

    // ---- REGISTRAR ----
    else if (strcmp(comando, "REGISTRAR") == 0) {

        char email[128], nombre[64], contrasena[128];
        recv(comm_socket, email, sizeof(email),0);
        recv(comm_socket, nombre, sizeof(nombre), 0);
        recv(comm_socket, contrasena,sizeof(contrasena),0);
        recv(comm_socket, recvBuff, sizeof(recvBuff), 0); // REGISTRAR-END

        int res = insertarUsuario(db, nombre, email, contrasena, 2);

        if (res == SQLITE_OK) {
            int id = 0, puntos = 0;
            obtenerUsuarioPorCredenciales(db, nombre, contrasena, &id, &puntos);
            sprintf(sendBuff, "OK|%d", id);
        } else {
            strcpy(sendBuff, "ERROR");
        }

        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
    }

    // ---- GET_RETOS ----
    else if (strcmp(comando, "GET_RETOS") == 0) {

        char filtro[32];
        char id_usuario_str[16];
        recv(comm_socket, filtro, sizeof(filtro), 0);
        recv(comm_socket, id_usuario_str, sizeof(id_usuario_str), 0);
        recv(comm_socket, recvBuff, sizeof(recvBuff), 0); // GET_RETOS-END

        int id_usuario = atoi(id_usuario_str);
        Reto retos[MAX_RETOS];
        int cantidad = 0;

        if      (strcmp(filtro, "TODOS") == 0) listarTodosRetos(db, retos, &cantidad);
        else if (strcmp(filtro, "ACTIVOS") == 0) listarRetosActivos(db, retos, &cantidad);
        else if (strcmp(filtro, "ACTIVOS_USUARIO") == 0) listarRetosActivosUsuario(db, id_usuario, retos, &cantidad);
        else if (strcmp(filtro, "ORGANIZADOS") == 0) listarRetosOrganizadosUsuario(db, id_usuario, retos, &cantidad);

        // Construir respuesta: una fila por reto separada por \n
        // Formato: id|titulo|estado|puntos\n
        memset(sendBuff, 0, sizeof(sendBuff));
        for (int i = 0; i < cantidad; i++) {
            char linea[256];
            sprintf(linea, "%d|%s|%s|%d\n",
                    retos[i].id,
                    retos[i].titulo,
                    estado_reto_a_string(retos[i].estadoReto),
                    retos[i].puntos);
            strcat(sendBuff, linea);
        }
        if (cantidad == 0) strcpy(sendBuff, "VACIO");

        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
    }

    // ---- GET_RETO ----
    else if (strcmp(comando, "GET_RETO") == 0) {

        char id_str[16];
        recv(comm_socket, id_str, sizeof(id_str), 0);
        recv(comm_socket, recvBuff, sizeof(recvBuff), 0); // GET_RETO-END

        Reto reto;
        obtenerRetoPorId(db, atoi(id_str), &reto);

        sprintf(sendBuff, "%d|%s|%s|%s|%s|%s|%s|%s|%d|%d",
                reto.id,
                reto.titulo,
                reto.descripcion,
                estado_reto_a_string(reto.estadoReto),
                tipo_reto_a_string(reto.tipoReto),
                reto.fecha_i_inscripcion,
                reto.fecha_f_inscripcion,
                dificultad_reto_a_string(reto.dificultadReto),
                reto.puntos,
                reto.id_organizador);

        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
    }

    // ---- GET_PERFIL ----
    else if (strcmp(comando, "GET_PERFIL") == 0) {

        char id_str[16];
        recv(comm_socket, id_str, sizeof(id_str), 0);
        recv(comm_socket, recvBuff, sizeof(recvBuff), 0); // GET_PERFIL-END

        int id_usuario = atoi(id_str);
        int ranking = 0, puntos_activos = 0, puntos_organizados = 0;

        obtenerRankingUsuario(db, id_usuario, &ranking);
        obtenerPuntosParticipacion(db, id_usuario, &puntos_activos);
        obtenerPuntosOrganizacion(db, id_usuario, &puntos_organizados);

        sprintf(sendBuff, "%d|%d|%d", ranking, puntos_activos, puntos_organizados);
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
    }

    // ---- GET_RANKING ----
    else if (strcmp(comando, "GET_RANKING") == 0) {

        char tipo[16], id_str[16];
        recv(comm_socket, tipo, sizeof(tipo), 0);
        recv(comm_socket, id_str, sizeof(id_str), 0);
        recv(comm_socket, recvBuff, sizeof(recvBuff), 0); // GET_RANKING-END

        Usuario usuarios[MAX_USUARIOS];
        int cantidad = 0;

        if (strcmp(tipo, "GLOBAL") == 0)
            listarRankingGlobal(db, usuarios, &cantidad);
        else
            listarRankingReto(db, atoi(id_str), usuarios, &cantidad);

        memset(sendBuff, 0, sizeof(sendBuff));
        for (int i = 0; i < cantidad; i++) {
            char linea[128];
            sprintf(linea, "%s|%d\n", usuarios[i].nombre, usuarios[i].total_puntos);
            strcat(sendBuff, linea);
        }
        if (cantidad == 0) strcpy(sendBuff, "VACIO");

        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
    }

    // ---- INSCRIBIR ----
    else if (strcmp(comando, "INSCRIBIR") == 0) {

        char id_usuario_str[16], id_reto_str[16], id_equipo_str[16], motivacion[256];
        recv(comm_socket, id_usuario_str, sizeof(id_usuario_str), 0);
        recv(comm_socket, id_reto_str, sizeof(id_reto_str), 0);
        recv(comm_socket, id_equipo_str, sizeof(id_equipo_str), 0);
        recv(comm_socket, motivacion, sizeof(motivacion), 0);
        recv(comm_socket, recvBuff, sizeof(recvBuff), 0); // INSCRIBIR-END

        int res = insertarParticipacion(db,
                                        atoi(id_usuario_str),
                                        atoi(id_reto_str),
                                        atoi(id_equipo_str),
                                        motivacion);

        strcpy(sendBuff, res == SQLITE_OK ? "OK" : "ERROR");
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
    }

    // ---- CREAR_RETO ----
    else if (strcmp(comando, "CREAR_RETO") == 0) {

        char titulo[128], resumen[512], tipo[16];
        char f_ini_insc[11], f_fin_insc[11], f_ini[11], f_fin[11];
        char dificultad[16], puntos_str[16], plazas_str[16], id_org_str[16];

        recv(comm_socket, titulo, sizeof(titulo), 0);
        recv(comm_socket, resumen, sizeof(resumen), 0);
        recv(comm_socket, tipo, sizeof(tipo), 0);
        recv(comm_socket, f_ini_insc, sizeof(f_ini_insc), 0);
        recv(comm_socket, f_fin_insc, sizeof(f_fin_insc), 0);
        recv(comm_socket, f_ini,sizeof(f_ini), 0);
        recv(comm_socket, f_fin, sizeof(f_fin),0);
        recv(comm_socket, dificultad, sizeof(dificultad), 0);
        recv(comm_socket, puntos_str, sizeof(puntos_str), 0);
        recv(comm_socket, plazas_str, sizeof(plazas_str), 0);
        recv(comm_socket, id_org_str, sizeof(id_org_str), 0);
        recv(comm_socket, recvBuff, sizeof(recvBuff),0); // CREAR_RETO-END

        int res = insertarReto(db, titulo, resumen, dificultad,
                               atoi(plazas_str), f_ini, f_fin,
                               f_ini_insc, f_fin_insc,
                               atoi(puntos_str), atoi(id_org_str));

        strcpy(sendBuff, res == SQLITE_OK ? "OK" : "ERROR");
        send(comm_socket, sendBuff, sizeof(sendBuff), 0);
    }

    // ---- EXIT ----
    else if (strcmp(comando, "EXIT") == 0) {
        // servidor.c gestiona el cierre
    }
}
