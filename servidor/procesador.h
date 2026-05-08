#ifndef PROCESADOR_H
#define PROCESADOR_H

#include <winsock2.h>
#include "sqlite3.h"

void procesarComando(SOCKET comm_socket, sqlite3 *db, char *comando);

#endif
