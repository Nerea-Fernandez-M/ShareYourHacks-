/*
 * reto.h
 *
 *  Created on: 26 mar 2026
 *      Author: nerea.garona
 */

#ifndef RETO_H_
#define RETO_H_

// enums de variables que necesitamos para el struct
typedef enum {
	SIN_COMENZAR,
	EN_CURSO,
	FINALIZADO
} EstadoReto;

typedef enum {
	FACIL,
	MEDIO,
	DIFICIL
} DificultadReto;

typedef enum {
	CTF,
	HACKATHON
} TipoReto;

// struct
typedef struct {
	int id;
	char titulo [50];
	char descripcion [200];
	EstadoReto estadoReto;
	DificultadReto dificultadReto;
	TipoReto tipoReto;
	int limite_plazas;
	char fecha_i[20];				// fecha inicio reto
	char fecha_f[20];				// fecha final reto
	char fecha_i_inscripcion[20];	// fecha inicio inscripcion
	char fecha_f_inscripcion[20];	// fecha fin inscripcion
	int puntos;

};



#endif /* RETO_H_ */
