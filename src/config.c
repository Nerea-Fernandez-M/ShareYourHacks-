/*
 * config.c
 *
 *  Created on: 6 abr 2026
 *      Author: nerea.garona
 */

#include <stdio.h>
#include <string.h>
#include "config.h"


int cargarConfig(const char *nombre_fichero, Config *config) {
	FILE *f = fopen(nombre_fichero, "r");
	if (!f) {
		return -1;
	}

	char linea [150];
	while (fgets(linea, sizeof(linea), f)) {
		linea[strcspn(linea, "\r\n")] = 0;

		if (strstr(linea, "user = ")) {
			sscanf (linea, "user = %s", config -> admin_user);

		} else if (strstr(linea, "contrasena = ")) {
			sscanf (linea, "contrasena = %s", config -> admin_contrasena);

		} else if (strstr(linea, "nombre_db = ")) {
			sscanf (linea, "nombre_db = %s", config -> nombre_db);

		} else if (strstr(linea, "ruta_init = ")) {
			sscanf (linea, "ruta_init = %s", config -> ruta_init);
		}
	}

	fclose(f);
	return 0;
}


