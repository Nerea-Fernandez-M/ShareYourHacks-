/*
 * estructuras.h
 *
 *  Created on: 1 abr 2026
 *      Author: nerea.f.m
 */

#ifndef ESTRUCTURAS_H_
#define ESTRUCTURAS_H_

//Enums

typedef enum { //Para participa
	EN_ESPERA,
	ACEPTADO,
	RECHAZADO,
    ESTADO_ACEPTACION_COUNT
} EstadoAceptacion;

typedef enum { //Para reto
	SIN_COMENZAR,
	EN_CURSO,
	FINALIZADO
} EstadoReto;

typedef enum { //Para reto
	FACIL,
	MEDIO,
	DIFICIL,
	DIFICULTAD_RETO_COUNT
} DificultadReto;

typedef enum { //Para reto
	CTF,
	HACKATHON,
    TIPO_RETO_COUNT
} TipoReto;

typedef enum { //Para user
	ORGANIZADOR,
	HACKER,
	STAFF,
	TIPO_ROL_COUNT
} TipoRol;

//Structs

typedef struct {
	int id_user;
	int id_equipo;
	int id_reto;
} Desempena;

typedef struct {
	int id;
	char nombre[30];
	int num_miembros;
	int ids[]; //El n de elementos se decide al crear la struct
}Equipo;

typedef struct {
	int id;
	int id_user;
	int id_reto;
	int id_equipo;
	char motivacion [300];
	int puntos;
	EstadoAceptacion estadoAceptacion;
} Participa;

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
	int id_organizador;
	int puntos;
}Reto;

typedef struct {
	int id;
	char nombre[30];
	char email[50];
	char contrasena[50];
	TipoRol tipoRol;
	float media_puntuacion;
	int total_puntos;
} Usuario;

//Conversion de enums
const char *estado_aceptacion_a_string(EstadoAceptacion estado);
const char *estado_reto_a_string      (EstadoReto estado);
const char *tipo_reto_a_string        (TipoReto tipo);
const char *dificultad_reto_a_string  (DificultadReto dificultad);
const char *tipo_rol_a_string         (TipoRol rol);

#endif /* ESTRUCTURAS_H_ */
