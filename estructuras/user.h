/*
 * user.h
 *
 *  Created on: 26 mar 2026
 *      Author: nerea.garona
 */

#ifndef USER_H_
#define USER_H_

typedef struct {
	int id;
	char nombre[30];
	char email[50];
	char contrasena[50];
	TipoRol tipoRol;
	float media_puntuacion;
	int total_puntos;
} User;


#endif /* USER_H_ */
