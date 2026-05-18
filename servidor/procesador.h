#ifndef PROCESADOR_H
#define PROCESADOR_H

#include <winsock2.h>

typedef struct sqlite3 sqlite3;

void procesarComando(SOCKET comm_socket, sqlite3 *db, char *comando);

#endif
