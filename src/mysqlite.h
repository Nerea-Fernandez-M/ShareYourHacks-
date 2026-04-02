#ifndef MYSQLITE_H_
#define MYSQLITE_H_

#include "sqlite3.h"
#include <stdio.h>
#include <string.h>

#define MAX_RETOS 128 //para visualizar retos

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

int obtenerUsuarioPorCredenciales(sqlite3 *db, const char *nombre,
                                   const char *contrasena, int *id_out, int *puntos_out); //Iniciar Sesion

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
	//Para visualizar los retos
int listarTodosRetos              (sqlite3 *db, Reto *retos_out, int *cantidad_out);
int listarRetosActivos            (sqlite3 *db, Reto *retos_out, int *cantidad_out);
int listarRetosUsuario            (sqlite3 *db, int id_usuario, Reto *retos_out, int *cantidad_out);
int listarRetosActivosUsuario     (sqlite3 *db, int id_usuario, Reto *retos_out, int *cantidad_out);
int listarRetosOrganizadosUsuario (sqlite3 *db, int id_usuario, Reto *retos_out, int *cantidad_out);
	//Obtener puntuaciones
int obtenerRankingUsuario      (sqlite3 *db, int id_usuario, int *ranking_out);
int obtenerPuntosParticipacion (sqlite3 *db, int id_usuario, int *puntos_out);
int obtenerPuntosOrganizacion  (sqlite3 *db, int id_usuario, int *puntos_out);

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
