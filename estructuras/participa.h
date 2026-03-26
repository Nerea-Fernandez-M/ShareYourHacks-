/*
 * participa.h
 *
 *  Created on: 26 mar 2026
 *      Author: nerea.garona
 */

#ifndef PARTICIPA_H_
#define PARTICIPA_H_

typedef enum {
	EN_ESPERA,
	ACEPTADO,
	RECHAZADO
} EstadoAceptacion;

typedef struct {
	int id;
	int id_user;
	int id_reto;
	int id_equipo;
	char motivacion [300];
	int puntos;
	EstadoAceptacion estadoAceptacion;
} Participa;


#endif /* PARTICIPA_H_ */
