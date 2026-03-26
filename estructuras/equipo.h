/*
 * equipo.h
 *
 *  Created on: 26 mar 2026
 *      Author: nerea.garona
 */

#ifndef EQUIPO_H_
#define EQUIPO_H_

// TODO Aqui he intentado guardar los ids de todos los miembros del equipo pero no me deja guardar lista, por si lo podeis hacer
// mientras he puesto solo el id del que ha creado el grupo
typedef struct {
	int id;
	char nombre[30];
	int num_miembros;
	int id_lider;
};


#endif /* EQUIPO_H_ */
