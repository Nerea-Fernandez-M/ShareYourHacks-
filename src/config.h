/*
 * config.h
 *
 *  Created on: 6 abr 2026
 *      Author: nerea.garona
 */

#ifndef SRC_CONFIG_H_
#define SRC_CONFIG_H_

typedef struct {
	char admin_user[50];
	char admin_contrasena[50];
	char nombre_db[50];
	char ruta_init[50];

} Config;


int cargarConfig (const char *nombre_fichero, Config *config);

#endif /* SRC_CONFIG_H_ */
