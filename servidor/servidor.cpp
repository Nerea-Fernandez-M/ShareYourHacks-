/*
 * servidor.c
 *
 *  Created on: 7 may 2026
 *      Author: nerea.f.m
 */

// Configuración del socket, bind, listen y el bucle de accept.
// Llama a la funcion de procesador.c: procesarComando(comm_socket, db, comando);

#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "procesador.h"
#include "../src/mysqlite.h"

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP   "127.0.0.1"
#define SERVER_PORT 6000

int main(void) {

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup fallido\n");
        return -1;
    }

    // Abrir base de datos
    sqlite3 *db = conectarBD("shareyourhacks.db");
    if (!db) {
        printf("Error: no se pudo conectar a la BD\n");
        WSACleanup();
        return 1;
    }
    if (inicializarBD(db) != SQLITE_OK) {
        printf("Error: no se pudo inicializar la BD\n");
        desconectarBD(db);
        WSACleanup();
        return 1;
    }

    // Crear socket de escucha
    SOCKET conn_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (conn_socket == INVALID_SOCKET) {
        printf("Error creando socket: %d\n", WSAGetLastError());
        desconectarBD(db);
        WSACleanup();
        return -1;
    }

    // Permitir reusar el puerto inmediatamente tras reiniciar
    int opt = 1;
    setsockopt(conn_socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));

    struct sockaddr_in server;
    server.sin_family      = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_IP);
    server.sin_port        = htons(SERVER_PORT);

    if (bind(conn_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("Bind fallido: %d\n", WSAGetLastError());
        closesocket(conn_socket);
        desconectarBD(db);
        WSACleanup();
        return -1;
    }

    if (listen(conn_socket, 5) == SOCKET_ERROR) {
        printf("Listen fallido: %d\n", WSAGetLastError());
        closesocket(conn_socket);
        desconectarBD(db);
        WSACleanup();
        return -1;
    }

    printf("Servidor escuchando en %s:%d\n", SERVER_IP, SERVER_PORT);

    // Bucle principal: aceptar clientes
    while (1) {
        struct sockaddr_in client;
        int client_len = sizeof(client);
        SOCKET comm_socket = accept(conn_socket, (struct sockaddr *)&client, &client_len);

        if (comm_socket == INVALID_SOCKET) {
            printf("Accept fallido: %d, continuando...\n", WSAGetLastError());
            continue;
        }

        printf("Cliente conectado: %s:%d\n",
               inet_ntoa(client.sin_addr), ntohs(client.sin_port));

        // Atender comandos del cliente hasta que mande EXIT o se desconecte
        char comando[64];
        int bytes;
        while ((bytes = recv(comm_socket, comando, sizeof(comando) - 1, 0)) > 0) {
            comando[bytes] = '\0';
            printf("Comando recibido: %s\n", comando);

            procesarComando(comm_socket, db, comando);

            if (strcmp(comando, "EXIT") == 0)
                break;
        }

        printf("Cliente desconectado\n");
        closesocket(comm_socket);
    }

    closesocket(conn_socket);
    desconectarBD(db);
    WSACleanup();

    return 0;
}
