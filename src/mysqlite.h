#ifndef MYSQLITE_H_
#define MYSQLITE_H_

#include "sqlite3.h"
#include <stdio.h>
#include <string.h>

// Conexion con la db
sqlite3* conectarBD(const char *nombreBD);
void desconectarBD(sqlite3 *db);
int inicializarBD(sqlite3 *db);

// Roles
int insertarRol(sqlite3 *db, const char *nombre, const char *descripcion);
int obtenerRol(sqlite3 *db, int id, char *nombre);

// Usuarios
int insertarUsuario(sqlite3 *db, const char *nombre, const char *email, 
                    const char *contrasena, int id_rol);
int obtenerUsuario(sqlite3 *db, int id, char *nombre, char *email);
int listarUsuarios(sqlite3 *db);
int actualizarPuntosUsuario(sqlite3 *db, int id, int puntos);

// Retos
int insertarReto(sqlite3 *db, const char *titulo, const char *descripcion,
                 const char *dificultad, int limite_plazas, 
                 const char *fecha_inicio, const char *fecha_fin,
                 const char *fecha_inicio_insc, const char *fecha_fin_insc,
                 int puntos, int id_organizador);
int obtenerReto(sqlite3 *db, int id, char *titulo);
int listarRetos(sqlite3 *db);
int listarRetosPorDificultad(sqlite3 *db, const char *dificultad);
int actualizarEstadoReto(sqlite3 *db, int id, const char *nuevo_estado);

// Equipos
int insertarEquipo(sqlite3 *db, const char *nombre, int id_reto, const char *ids_usuarios);
int obtenerEquipo(sqlite3 *db, int id, char *nombre);
int listarEquiposPorReto(sqlite3 *db, int id_reto);

// Participaciones
int insertarParticipacion(sqlite3 *db, int id_usuario, int id_reto, 
                          int id_equipo, const char *motivacion);
int obtenerParticipacion(sqlite3 *db, int id);
int listarParticipacionesPorUsuario(sqlite3 *db, int id_usuario);
int listarParticipacionesPorReto(sqlite3 *db, int id_reto);
int actualizarEstadoParticipacion(sqlite3 *db, int id, const char *nuevo_estado);
int actualizarPuntosParticipacion(sqlite3 *db, int id, int puntos);

#endif