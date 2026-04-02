#ifndef MYSQLITE_H_
#define MYSQLITE_H_

#include "sqlite3.h"
#include <stdio.h>
#include <string.h>
#include "estructuras.h"
#include "navegacion.h"

#define MAX_RETOS 128 //para visualizar retos
#define MAX_USUARIOS 128 //para visualizar el ranking

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
	//Funciones reto activo
int obtenerRolEnReto        (sqlite3 *db, int id_usuario, int id_reto, TipoRol *rol_out);
int obtenerPuestoEnReto     (sqlite3 *db, int id_usuario, int id_reto, int *puesto_out);
int obtenerDiasRestantes    (sqlite3 *db, int id_reto, int *dias_out);
	//funcion gestionar puntero de ventana
int obtenerRetoPorId(sqlite3 *db, int id, Reto *reto_out);

// Equipos
int insertarEquipo(sqlite3 *db, const char *nombre, int id_reto, const char *ids_usuarios);
int obtenerEquipo(sqlite3 *db, int id, char *nombre);
int listarEquiposPorReto(sqlite3 *db, int id_reto);

//Funciones apuntarse a equipo
int retoRequiereEquipo   (sqlite3 *db, int id_reto, int *requiere_out);
int obtenerEquipoPorNombre(sqlite3 *db, const char *nombre, int id_reto,
                            int *id_equipo_out);
int unirseAEquipo        (sqlite3 *db, int id_usuario, int id_equipo);

//Funciones para el ranking
int listarRankingGlobal(sqlite3 *db, Usuario *usuarios_out, int *cantidad_out);
int listarRankingReto  (sqlite3 *db, int id_reto,
                         Usuario *usuarios_out, int *cantidad_out);

// Participaciones
int insertarParticipacion(sqlite3 *db, int id_usuario, int id_reto, 
                          int id_equipo, const char *motivacion);
int obtenerParticipacion(sqlite3 *db, int id);
int listarParticipacionesPorUsuario(sqlite3 *db, int id_usuario);
int listarParticipacionesPorReto(sqlite3 *db, int id_reto);
int actualizarEstadoParticipacion(sqlite3 *db, int id, const char *nuevo_estado);
int actualizarPuntosParticipacion(sqlite3 *db, int id, int puntos);

//Funciones auxiliares conversion de enums
static DificultadReto string_a_dificultad(const char *str);
static TipoReto string_a_tipo_reto(const char *str);
static EstadoReto string_a_estado_reto(const char *str);

#endif
