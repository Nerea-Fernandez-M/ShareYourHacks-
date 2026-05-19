/*
 * main.cpp
 *
 *  Created on: 7 may 2026
 *      Author: nerea.f.m
 */

#include <cstdio>
#include <winsock2.h>
#include "../src/mysqlite.h"
#include "../src/navegacion.h"
#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP   "127.0.0.1"
#define SERVER_PORT 6000

typedef void (*GestorVentana)(Ventana *ventana);

GestorVentana gestores[VENTANA_COUNT] = {
    funcionalidadMenu,          // VENTANA_MENU_MAIN
    funcionalidadVerRetos,      // VENTANA_VER_RETOS
    funcionalidadReto,          // VENTANA_RETO
    funcionalidadRetoCompleto,  // VENTANA_RETO_COMPLETO
    funcionalidadApuntarse,     // VENTANA_APUNTARSE
    funcionalidadRanking,       // VENTANA_RANKING
    funcionalidadPerfil,        // VENTANA_PERFIL
    funcionalidadOrganizarReto, // VENTANA_ORGANIZAR_RETO
    funcionalidadExit,          // VENTANA_EXIT
};

int main() {

    // Inicializar Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Error inicializando Winsock\n");
        return -1;
    }

    // Crear socket y conectar al servidor
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Error creando socket\n");
        WSACleanup();
        return -1;
    }

    struct sockaddr_in server;
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_port        = htons(SERVER_PORT);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Error conectando al servidor en %s:%d\n", SERVER_IP, SERVER_PORT);
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    printf("Conectado al servidor\n");
    fflush(stdout);

    // Inicializar la ventana
    Ventana ventana;
    ventana.actual = VENTANA_MENU_MAIN;
    ventana.historialTop = 0;
    ventana.db = NULL;  // el cliente no accede a la BD directamente
    ventana.usuario = NULL;
    ventana.sock = sock;

    // Bucle principal
    while (ventana.actual != VENTANA_EXIT) {
        gestores[ventana.actual](&ventana);
    }

    funcionalidadExit(&ventana);  // exit
    ventanaLimpiar(&ventana);  // limpieza

    // cerrar conexion
    closesocket(sock);
    WSACleanup();

    return 0;
}
