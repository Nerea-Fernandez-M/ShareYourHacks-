#include "mysqlite.h"
#include <direct.h>

// Conexion con la db
sqlite3* conectarBD(const char *nombreBD) {
    sqlite3 *db;
    int resultado = sqlite3_open(nombreBD, &db);
    
    if (resultado != SQLITE_OK) {
        printf("Error abriendo BD: %s\n", sqlite3_errmsg(db));
        return NULL;
    }
    
    printf("Conectado a %s\n", nombreBD);
    return db;
}

void desconectarBD(sqlite3 *db) {
    if (db != NULL) {
        int resultado = sqlite3_close(db);
        if (resultado == SQLITE_OK) {
            printf("Desconectado de BD\n");
        } else {
            printf("Error cerrando BD\n");
        }
    }
}

int inicializarBD(sqlite3 *db) {
    char *errMsg = NULL;
    
    char ruta[256];
    if (getcwd(ruta, sizeof(ruta)) != NULL)
        printf("Directorio actual: %s\n", ruta);

    // Leer el contenido de init.sql
    FILE *archivo = fopen("admin/init.sql", "r");
    if (!archivo) {
        printf("Error: No se encontró init.sql\n");
        return 1;
    }
    
    // Leer todo el archivo
    char sql[10000] = {0};
    size_t bytes_leidos = fread(sql, 1, sizeof(sql) - 1, archivo);
    fclose(archivo);
    
    if (bytes_leidos == 0) {
        printf("Error: init.sql vacío\n");
        return 1;
    }
    
    sql[bytes_leidos] = '\0';
    
    // Ejecutar SQL
    int resultado = sqlite3_exec(db, sql, NULL, NULL, &errMsg);
    
    if (resultado != SQLITE_OK) {
        printf("Error inicializando BD: %s\n", errMsg);
        sqlite3_free(errMsg);
        return resultado;
    }
    
    printf("✓ Base de datos inicializada\n");
    return SQLITE_OK;
}

// ===== ROLES =====
int insertarRol(sqlite3 *db, const char *nombre, const char *descripcion) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO Roles (nombre, descripcion) VALUES (?, ?)";
    
    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error preparando INSERT: %s\n", sqlite3_errmsg(db));
        return resultado;
    }
    
    sqlite3_bind_text(stmt, 1, nombre, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, descripcion, -1, SQLITE_STATIC);
    
    resultado = sqlite3_step(stmt);
    if (resultado != SQLITE_DONE) {
        printf("Error insertando rol: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return resultado;
    }
    
    printf("Rol insertado (ID: %lld)\n", sqlite3_last_insert_rowid(db));
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int obtenerRol(sqlite3 *db, int id, char *nombre) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT nombre FROM Roles WHERE id = ?";
    
    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf(" Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }
    
    sqlite3_bind_int(stmt, 1, id);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        strcpy(nombre, (char *)sqlite3_column_text(stmt, 0));
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }
    
    sqlite3_finalize(stmt);
    return 1;
}

// Usuarios
int insertarUsuario(sqlite3 *db, const char *nombre, const char *email, 
                    const char *contrasena, int id_rol) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO Usuarios (nombre, email, contrasena, id_rol) VALUES (?, ?, ?, ?)";
    
    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }
    
    sqlite3_bind_text(stmt, 1, nombre, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, contrasena, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, id_rol);
    
    resultado = sqlite3_step(stmt);
    if (resultado != SQLITE_DONE) {
        printf("Error insertando usuario: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return resultado;
    }
    
    printf("Usuario insertado (ID: %lld) - %s\n", sqlite3_last_insert_rowid(db), nombre);
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int obtenerUsuario(sqlite3 *db, int id, char *nombre, char *email) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT nombre, email FROM Usuarios WHERE id = ?";
    
    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }
    
    sqlite3_bind_int(stmt, 1, id);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        strcpy(nombre, (char *)sqlite3_column_text(stmt, 0));
        strcpy(email, (char *)sqlite3_column_text(stmt, 1));
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }
    
    sqlite3_finalize(stmt);
    return 1;
}

int listarUsuarios(sqlite3 *db) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, nombre, email, total_puntos FROM Usuarios";
    
    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }
    
    printf("\n LISTA DE USUARIOS: \n");
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *nombre = (char *)sqlite3_column_text(stmt, 1);
        const char *email = (char *)sqlite3_column_text(stmt, 2);
        int puntos = sqlite3_column_int(stmt, 3);
        
        printf("║ ID: %d | %s | %s | Puntos: %d\n", id, nombre, email, puntos);
        count++;
    }
    
    printf("Total: %d usuarios\n\n", count);
    
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int actualizarPuntosUsuario(sqlite3 *db, int id, int puntos) {
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE Usuarios SET total_puntos = total_puntos + ? WHERE id = ?";
    
    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }
    
    sqlite3_bind_int(stmt, 1, puntos);
    sqlite3_bind_int(stmt, 2, id);
    
    resultado = sqlite3_step(stmt);
    if (resultado != SQLITE_DONE) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return resultado;
    }
    
    printf("Puntos actualizados: +%d\n", puntos);
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int obtenerUsuarioPorCredenciales(sqlite3 *db, const char *nombre,  //Para iniciar sesion
                                   const char *contrasena, int *id_out, int *puntos_out) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, total_puntos FROM Usuarios "
                      "WHERE nombre = ? AND contrasena = ?";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    sqlite3_bind_text(stmt, 1, nombre,     -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, contrasena, -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        *id_out     = sqlite3_column_int(stmt, 0);
        *puntos_out = sqlite3_column_int(stmt, 1);
        sqlite3_finalize(stmt);
        return SQLITE_OK;   // credenciales correctas
    }

    sqlite3_finalize(stmt);
    return 1;               // no encontrado
}

// Retos
int insertarReto(sqlite3 *db, const char *titulo, const char *descripcion,
                 const char *dificultad, int limite_plazas, 
                 const char *fecha_inicio, const char *fecha_fin,
                 const char *fecha_inicio_insc, const char *fecha_fin_insc,
                 int puntos, int id_organizador) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO Retos (titulo, descripcion, dificultad, limite_plazas, "
                      "fecha_inicio, fecha_fin, fecha_inicio_inscripcion, fecha_fin_inscripcion, "
                      "puntos, id_organizador) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
    
    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }
    
    sqlite3_bind_text(stmt, 1, titulo, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, descripcion, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, dificultad, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, limite_plazas);
    sqlite3_bind_text(stmt, 5, fecha_inicio, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, fecha_fin, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 7, fecha_inicio_insc, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 8, fecha_fin_insc, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 9, puntos);
    sqlite3_bind_int(stmt, 10, id_organizador);
    
    resultado = sqlite3_step(stmt);
    if (resultado != SQLITE_DONE) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return resultado;
    }
    
    printf("Reto insertado (ID: %lld) - %s\n", sqlite3_last_insert_rowid(db), titulo);
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int obtenerReto(sqlite3 *db, int id, char *titulo) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT titulo FROM Retos WHERE id = ?";
    
    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }
    
    sqlite3_bind_int(stmt, 1, id);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        strcpy(titulo, (char *)sqlite3_column_text(stmt, 0));
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }
    
    sqlite3_finalize(stmt);
    return 1;
}

int listarRetos(sqlite3 *db) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, titulo, dificultad, estado, puntos FROM Retos";
    
    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }
    
    printf("\n LISTA DE RETOS: \n");
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *titulo = (char *)sqlite3_column_text(stmt, 1);
        const char *dificultad = (char *)sqlite3_column_text(stmt, 2);
        const char *estado = (char *)sqlite3_column_text(stmt, 3);
        int puntos = sqlite3_column_int(stmt, 4);
        
        printf("║ ID: %d | %s | Dificultad: %s | Estado: %s | Puntos: %d\n", 
               id, titulo, dificultad, estado, puntos);
        count++;
    }
    
    printf("Total: %d retos\n\n", count);
    
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int listarRetosPorDificultad(sqlite3 *db, const char *dificultad) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, titulo, puntos FROM Retos WHERE dificultad = ?";
    
    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf(" Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }
    
    sqlite3_bind_text(stmt, 1, dificultad, -1, SQLITE_STATIC);
    
    printf("\n Retos de dificultad: %s\n", dificultad);
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *titulo = (char *)sqlite3_column_text(stmt, 1);
        int puntos = sqlite3_column_int(stmt, 2);
        
        printf("• [%d] %s (Puntos: %d)\n", id, titulo, puntos);
        count++;
    }
    
    printf("Total: %d retos\n\n", count);
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int actualizarEstadoReto(sqlite3 *db, int id, const char *nuevo_estado) {
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE Retos SET estado = ? WHERE id = ?";
    
    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }
    
    sqlite3_bind_text(stmt, 1, nuevo_estado, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, id);
    
    resultado = sqlite3_step(stmt);
    if (resultado != SQLITE_DONE) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return resultado;
    }
    
    printf("✓ Estado del reto actualizado a: %s\n", nuevo_estado);
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

// Todos los retos
int listarTodosRetos(sqlite3 *db, Reto *retos_out, int *cantidad_out) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, titulo, descripcion, dificultad, estado, puntos "
                      "FROM Retos";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < MAX_RETOS) {
    	//Posibles problemas
    	const unsigned char *txt;

    	// TÍTULO
    	txt = sqlite3_column_text(stmt, 1);
    	if (txt != NULL) {
    	    snprintf(retos_out[count].titulo,
    	             sizeof(retos_out[count].titulo),
    	             "%s", txt);
    	} else {
    	    retos_out[count].titulo[0] = '\0';
    	}

    	// DESCRIPCIÓN
    	txt = sqlite3_column_text(stmt, 2);
    	if (txt != NULL) {
    	    snprintf(retos_out[count].descripcion,
    	             sizeof(retos_out[count].descripcion),
    	             "%s", txt);
    	} else {
    	    retos_out[count].descripcion[0] = '\0';
    	}

    	//General
        retos_out[count].id = sqlite3_column_int(stmt, 0);
        retos_out[count].dificultadReto = string_a_dificultad((char *)sqlite3_column_text(stmt, 3));
        retos_out[count].estadoReto= string_a_estado_reto((char *)sqlite3_column_text(stmt, 4));
        retos_out[count].puntos = sqlite3_column_int(stmt, 5);
        count++;
    }

    *cantidad_out = count;
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

// Solo retos activos
int listarRetosActivos(sqlite3 *db, Reto *retos_out, int *cantidad_out) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, titulo, descripcion, dificultad, estado, puntos "
                      "FROM Retos WHERE estado = 'activo'";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < MAX_RETOS) {

    	const unsigned char *txt;

    	// TÍTULO
    	txt = sqlite3_column_text(stmt, 1);
    	if (txt != NULL) {
    	    snprintf(retos_out[count].titulo,
    	             sizeof(retos_out[count].titulo),
    	             "%s", txt);
    	} else {
    	    retos_out[count].titulo[0] = '\0';
    	}

    	// DESCRIPCIÓN
    	txt = sqlite3_column_text(stmt, 2);
    	if (txt != NULL) {
    	    snprintf(retos_out[count].descripcion,
    	             sizeof(retos_out[count].descripcion),
    	             "%s", txt);
    	} else {
    	    retos_out[count].descripcion[0] = '\0';
    	}

        retos_out[count].id     = sqlite3_column_int(stmt, 0);
        retos_out[count].dificultadReto = string_a_dificultad((char *)sqlite3_column_text(stmt, 3));
        retos_out[count].estadoReto= string_a_estado_reto((char *)sqlite3_column_text(stmt, 4));
        retos_out[count].puntos = sqlite3_column_int(stmt, 5);
        count++;
    }

    *cantidad_out = count;
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

// Todos los retos del usuario (en los que participa)
int listarRetosUsuario(sqlite3 *db, int id_usuario, Reto *retos_out, int *cantidad_out) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT r.id, r.titulo, r.descripcion, r.dificultad, r.estado, r.puntos "
                      "FROM Retos r "
                      "INNER JOIN Participaciones p ON r.id = p.id_reto "
                      "WHERE p.id_usuario = ?";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    sqlite3_bind_int(stmt, 1, id_usuario);

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < MAX_RETOS) {


    	const unsigned char *txt;

    	// TÍTULO
    	txt = sqlite3_column_text(stmt, 1);
    	if (txt != NULL) {
    	    snprintf(retos_out[count].titulo,
    	             sizeof(retos_out[count].titulo),
    	             "%s", txt);
    	} else {
    	    retos_out[count].titulo[0] = '\0';
    	}

    	// DESCRIPCIÓN
    	txt = sqlite3_column_text(stmt, 2);
    	if (txt != NULL) {
    	    snprintf(retos_out[count].descripcion,
    	             sizeof(retos_out[count].descripcion),
    	             "%s", txt);
    	} else {
    	    retos_out[count].descripcion[0] = '\0';
    	}

        retos_out[count].id  = sqlite3_column_int(stmt, 0);
        retos_out[count].dificultadReto = string_a_dificultad((char *)sqlite3_column_text(stmt, 3));
        retos_out[count].estadoReto = string_a_estado_reto((char *)sqlite3_column_text(stmt, 4));
        retos_out[count].puntos = sqlite3_column_int(stmt, 5);
        count++;
    }

    *cantidad_out = count;
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

// Retos activos del usuario (en los que participa y están activos)
int listarRetosActivosUsuario(sqlite3 *db, int id_usuario, Reto *retos_out, int *cantidad_out) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT r.id, r.titulo, r.descripcion, r.dificultad, r.estado, r.puntos "
                      "FROM Retos r "
                      "INNER JOIN Participaciones p ON r.id = p.id_reto "
                      "WHERE p.id_usuario = ? AND r.estado = 'activo'";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    sqlite3_bind_int(stmt, 1, id_usuario);

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < MAX_RETOS) {


    	const unsigned char *txt;

    	// TÍTULO
    	txt = sqlite3_column_text(stmt, 1);
    	if (txt != NULL) {
    	    snprintf(retos_out[count].titulo,
    	             sizeof(retos_out[count].titulo),
    	             "%s", txt);
    	} else {
    	    retos_out[count].titulo[0] = '\0';
    	}

    	// DESCRIPCIÓN
    	txt = sqlite3_column_text(stmt, 2);
    	if (txt != NULL) {
    	    snprintf(retos_out[count].descripcion,
    	             sizeof(retos_out[count].descripcion),
    	             "%s", txt);
    	} else {
    	    retos_out[count].descripcion[0] = '\0';
    	}

        retos_out[count].id  = sqlite3_column_int(stmt, 0);
        retos_out[count].dificultadReto = string_a_dificultad((char *)sqlite3_column_text(stmt, 3));
        retos_out[count].estadoReto = string_a_estado_reto((char *)sqlite3_column_text(stmt, 4));
        retos_out[count].puntos = sqlite3_column_int(stmt, 5);
        count++;
    }

    *cantidad_out = count;
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

// Retos organizados por el usuario
int listarRetosOrganizadosUsuario(sqlite3 *db, int id_usuario, Reto *retos_out, int *cantidad_out) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, titulo, descripcion, dificultad, estado, puntos "
                      "FROM Retos WHERE id_organizador = ?";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    sqlite3_bind_int(stmt, 1, id_usuario);

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < MAX_RETOS) {

    	const unsigned char *txt;

    	// TÍTULO
    	txt = sqlite3_column_text(stmt, 1);
    	if (txt != NULL) {
    	    snprintf(retos_out[count].titulo,
    	             sizeof(retos_out[count].titulo),
    	             "%s", txt);
    	} else {
    	    retos_out[count].titulo[0] = '\0';
    	}

    	// DESCRIPCIÓN
    	txt = sqlite3_column_text(stmt, 2);
    	if (txt != NULL) {
    	    snprintf(retos_out[count].descripcion,
    	             sizeof(retos_out[count].descripcion),
    	             "%s", txt);
    	} else {
    	    retos_out[count].descripcion[0] = '\0';
    	}

        retos_out[count].id     = sqlite3_column_int(stmt, 0);
        retos_out[count].dificultadReto = string_a_dificultad((char *)sqlite3_column_text(stmt, 3));
        retos_out[count].estadoReto = string_a_estado_reto((char *)sqlite3_column_text(stmt, 4));
        retos_out[count].puntos = sqlite3_column_int(stmt, 5);
        count++;
    }

    *cantidad_out = count;
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

//Funciones reto activo
// Rol del usuario en un reto concreto
int obtenerRolEnReto(sqlite3 *db, int id_usuario, int id_reto, TipoRol *rol_out) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id_rol FROM Participaciones "
                      "WHERE id_usuario = ? AND id_reto = ?";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    sqlite3_bind_int(stmt, 1, id_usuario);
    sqlite3_bind_int(stmt, 2, id_reto);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        *rol_out = (TipoRol)sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }

    sqlite3_finalize(stmt);
    return 1;
}

// Puesto del usuario en el ranking de un reto concreto
int obtenerPuestoEnReto(sqlite3 *db, int id_usuario, int id_reto, int *puesto_out) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT COUNT(*) + 1 FROM Participaciones "
                      "WHERE id_reto = ? AND puntos > "
                      "(SELECT puntos FROM Participaciones "
                      " WHERE id_usuario = ? AND id_reto = ?)";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    sqlite3_bind_int(stmt, 1, id_reto);
    sqlite3_bind_int(stmt, 2, id_usuario);
    sqlite3_bind_int(stmt, 3, id_reto);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        *puesto_out = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }

    sqlite3_finalize(stmt);
    return 1;
}

// Tiempo restante del reto en dias
int obtenerDiasRestantes(sqlite3 *db, int id_reto, int *dias_out) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT CAST(julianday(fecha_fin) - julianday('now') AS INTEGER) "
                      "FROM Retos WHERE id = ?";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    sqlite3_bind_int(stmt, 1, id_reto);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        *dias_out = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }

    sqlite3_finalize(stmt);
    return 1;
}

	//Obtener puntuaciones
// Posicion del usuario en el ranking global
int obtenerRankingUsuario(sqlite3 *db, int id_usuario, int *ranking_out) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT COUNT(*) + 1 FROM Usuarios "
                      "WHERE total_puntos > (SELECT total_puntos FROM Usuarios WHERE id = ?)";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    sqlite3_bind_int(stmt, 1, id_usuario);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        *ranking_out = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }

    sqlite3_finalize(stmt);
    return 1;
}

// Puntos obtenidos en retos en los que participa
int obtenerPuntosParticipacion(sqlite3 *db, int id_usuario, int *puntos_out) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT COALESCE(SUM(p.puntos), 0) "
                      "FROM Participaciones p "
                      "WHERE p.id_usuario = ?";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    sqlite3_bind_int(stmt, 1, id_usuario);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        *puntos_out = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }

    sqlite3_finalize(stmt);
    return 1;
}

// Puntos obtenidos en retos organizados por el usuario
int obtenerPuntosOrganizacion(sqlite3 *db, int id_usuario, int *puntos_out) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT COALESCE(SUM(puntos), 0) "
                      "FROM Retos "
                      "WHERE id_organizador = ?";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    sqlite3_bind_int(stmt, 1, id_usuario);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        *puntos_out = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }

    sqlite3_finalize(stmt);
    return 1;
}
//Funcion para gestionar el puntero al resto
// mysqlite.c
int obtenerRetoPorId(sqlite3 *db, int id, Reto *reto_out) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, titulo, descripcion, dificultad, estado, tipo, "
                      "puntos, id_organizador, fecha_inicio_inscripcion, "
                      "fecha_fin_inscripcion, fecha_inicio, fecha_fin "
                      "FROM Retos WHERE id = ?";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    sqlite3_bind_int(stmt, 1, id);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        reto_out->id     = sqlite3_column_int(stmt, 0);
        strncpy(reto_out->titulo,      (char *)sqlite3_column_text(stmt, 1), 127);
        strncpy(reto_out->descripcion, (char *)sqlite3_column_text(stmt, 2), 511);
        reto_out->dificultadReto = string_a_dificultad((char *)sqlite3_column_text(stmt, 3));
        reto_out->estadoReto     = string_a_estado_reto((char *)sqlite3_column_text(stmt, 4));
        reto_out->tipoReto       = string_a_tipo_reto((char *)sqlite3_column_text(stmt, 5));
        reto_out->puntos         = sqlite3_column_int(stmt, 6);
        reto_out->id_organizador = sqlite3_column_int(stmt, 7);
        strncpy(reto_out->fecha_i_inscripcion, (char *)sqlite3_column_text(stmt, 8), 10);
        strncpy(reto_out->fecha_f_inscripcion, (char *)sqlite3_column_text(stmt, 9), 10);
        strncpy(reto_out->fecha_i,        (char *)sqlite3_column_text(stmt, 10), 10);
        strncpy(reto_out->fecha_f,           (char *)sqlite3_column_text(stmt, 11), 10);
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }

    sqlite3_finalize(stmt);
    return 1;
}

// Equipos
int insertarEquipo(sqlite3 *db, const char *nombre, int id_reto, const char *ids_usuarios) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO Equipos (nombre, id_reto, ids_usuarios) VALUES (?, ?, ?)";
    
    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }
    
    sqlite3_bind_text(stmt, 1, nombre, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, id_reto);
    sqlite3_bind_text(stmt, 3, ids_usuarios, -1, SQLITE_STATIC);
    
    resultado = sqlite3_step(stmt);
    if (resultado != SQLITE_DONE) {
        printf(" Error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return resultado;
    }
    
    printf("Equipo insertado (ID: %lld) - %s\n", sqlite3_last_insert_rowid(db), nombre);
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int obtenerEquipo(sqlite3 *db, int id, char *nombre) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT nombre FROM Equipos WHERE id = ?";
    
    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }
    
    sqlite3_bind_int(stmt, 1, id);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        strcpy(nombre, (char *)sqlite3_column_text(stmt, 0));
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }
    
    sqlite3_finalize(stmt);
    return 1;
}

int listarEquiposPorReto(sqlite3 *db, int id_reto) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, nombre, num_miembros FROM Equipos WHERE id_reto = ?";
    
    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf(" Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }
    
    sqlite3_bind_int(stmt, 1, id_reto);
    
    printf("\n Equipos del reto %d:\n", id_reto);
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char *nombre = (char *)sqlite3_column_text(stmt, 1);
        int miembros = sqlite3_column_int(stmt, 2);
        
        printf("• [%d] %s (%d miembros)\n", id, nombre, miembros);
        count++;
    }
    
    printf("Total: %d equipos\n\n", count);
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

//Funciones para apuntarse
// Comprobar si un reto es de tipo HACKATHON (requiere equipo)
int retoRequiereEquipo(sqlite3 *db, int id_reto, int *requiere_out) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT tipo FROM Retos WHERE id = ?";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    sqlite3_bind_int(stmt, 1, id_reto);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char *tipo = (char *)sqlite3_column_text(stmt, 0);
        *requiere_out = (strcmp(tipo, "HACKATHON") == 0) ? 1 : 0;
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }

    sqlite3_finalize(stmt);
    return 1;
}

// Buscar equipo por nombre dentro de un reto
int obtenerEquipoPorNombre(sqlite3 *db, const char *nombre, int id_reto,
                            int *id_equipo_out) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id FROM Equipos WHERE nombre = ? AND id_reto = ?";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    sqlite3_bind_text(stmt, 1, nombre,   -1, SQLITE_STATIC);
    sqlite3_bind_int (stmt, 2, id_reto);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        *id_equipo_out = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }

    sqlite3_finalize(stmt);
    return 1;  // no encontrado
}

// Unirse a equipo
int unirseAEquipo(sqlite3 *db, int id_usuario, int id_equipo) {
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE Equipos SET num_miembros = num_miembros + 1 "
                      "WHERE id = ?";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    sqlite3_bind_int(stmt, 1, id_equipo);

    resultado = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return (resultado == SQLITE_DONE) ? SQLITE_OK : resultado;
}

//Funciones para el ranking
// Ranking global: todos los usuarios ordenados por puntos
int listarRankingGlobal(sqlite3 *db, Usuario *usuarios_out, int *cantidad_out) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, nombre, total_puntos "
                      "FROM Usuarios "
                      "ORDER BY total_puntos DESC";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < MAX_USUARIOS) {
        usuarios_out[count].id           = sqlite3_column_int(stmt, 0);
        strncpy(usuarios_out[count].nombre,
                (char *)sqlite3_column_text(stmt, 1), 63);
        usuarios_out[count].total_puntos = sqlite3_column_int(stmt, 2);
        count++;
    }

    *cantidad_out = count;
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

// Ranking de un reto concreto: participantes ordenados por puntos
int listarRankingReto(sqlite3 *db, int id_reto,
                       Usuario *usuarios_out, int *cantidad_out) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT u.id, u.nombre, p.puntos "
                      "FROM Participaciones p "
                      "INNER JOIN Usuarios u ON p.id_usuario = u.id "
                      "WHERE p.id_reto = ? AND p.estado = 'ACEPTADO' "
                      "ORDER BY p.puntos DESC";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    sqlite3_bind_int(stmt, 1, id_reto);

    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW && count < MAX_USUARIOS) {
        usuarios_out[count].id           = sqlite3_column_int(stmt, 0);
        strncpy(usuarios_out[count].nombre,
                (char *)sqlite3_column_text(stmt, 1), 63);
        usuarios_out[count].total_puntos = sqlite3_column_int(stmt, 2);
        count++;
    }

    *cantidad_out = count;
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

// Participacion
int insertarParticipacion(sqlite3 *db, int id_usuario, int id_reto, 
                          int id_equipo, const char *motivacion) {
    sqlite3_stmt *stmt;
    const char *sql = "INSERT INTO Participaciones (id_usuario, id_reto, id_equipo, motivacion) VALUES (?, ?, ?, ?)";
    
    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf(" Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }
    
    sqlite3_bind_int(stmt, 1, id_usuario);
    sqlite3_bind_int(stmt, 2, id_reto);
    sqlite3_bind_int(stmt, 3, id_equipo);
    sqlite3_bind_text(stmt, 4, motivacion, -1, SQLITE_STATIC);
    
    resultado = sqlite3_step(stmt);
    if (resultado != SQLITE_DONE) {
        printf(" Error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return resultado;
    }
    
    printf("Participación insertada (ID: %lld)\n", sqlite3_last_insert_rowid(db));
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int obtenerParticipacion(sqlite3 *db, int id) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id_usuario, id_reto, estado, puntos FROM Participaciones WHERE id = ?";
    
    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf(" Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }
    
    sqlite3_bind_int(stmt, 1, id);
    
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        int id_usuario = sqlite3_column_int(stmt, 0);
        int id_reto = sqlite3_column_int(stmt, 1);
        const char *estado = (char *)sqlite3_column_text(stmt, 2);
        int puntos = sqlite3_column_int(stmt, 3);
        
        printf("Participación #%d - Usuario: %d | Reto: %d | Estado: %s | Puntos: %d\n",
               id, id_usuario, id_reto, estado, puntos);
        
        sqlite3_finalize(stmt);
        return SQLITE_OK;
    }
    
    sqlite3_finalize(stmt);
    return 1;
}

int listarParticipacionesPorUsuario(sqlite3 *db, int id_usuario) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, id_reto, estado, puntos FROM Participaciones WHERE id_usuario = ?";
    
    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }
    
    sqlite3_bind_int(stmt, 1, id_usuario);
    
    printf("\nParticipaciones del usuario %d:\n", id_usuario);
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int id_reto = sqlite3_column_int(stmt, 1);
        const char *estado = (char *)sqlite3_column_text(stmt, 2);
        int puntos = sqlite3_column_int(stmt, 3);
        
        printf("• [%d] Reto: %d | Estado: %s | Puntos: %d\n", id, id_reto, estado, puntos);
        count++;
    }
    
    printf("Total: %d participaciones\n\n", count);
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int listarParticipacionesPorReto(sqlite3 *db, int id_reto) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT id, id_usuario, estado, puntos FROM Participaciones WHERE id_reto = ?";
    
    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf(" Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }
    
    sqlite3_bind_int(stmt, 1, id_reto);
    
    printf("\n Participaciones en reto %d:\n", id_reto);
    
    int count = 0;
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        int id_usuario = sqlite3_column_int(stmt, 1);
        const char *estado = (char *)sqlite3_column_text(stmt, 2);
        int puntos = sqlite3_column_int(stmt, 3);
        
        printf("• [%d] Usuario: %d | Estado: %s | Puntos: %d\n", id, id_usuario, estado, puntos);
        count++;
    }
    
    printf("Total: %d participaciones\n\n", count);
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int actualizarEstadoParticipacion(sqlite3 *db, int id, const char *nuevo_estado) {
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE Participaciones SET estado = ? WHERE id = ?";
    
    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf(" Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }
    
    sqlite3_bind_text(stmt, 1, nuevo_estado, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, id);
    
    resultado = sqlite3_step(stmt);
    if (resultado != SQLITE_DONE) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return resultado;
    }
    
    printf("Estado participación actualizado a: %s\n", nuevo_estado);
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int actualizarPuntosParticipacion(sqlite3 *db, int id, int puntos) {
    sqlite3_stmt *stmt;
    const char *sql = "UPDATE Participaciones SET puntos = ? WHERE id = ?";
    
    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (resultado != SQLITE_OK) {
        printf(" Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }
    
    sqlite3_bind_int(stmt, 1, puntos);
    sqlite3_bind_int(stmt, 2, id);
    
    resultado = sqlite3_step(stmt);
    if (resultado != SQLITE_DONE) {
        printf(" Error: %s\n", sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return resultado;
    }
    
    printf(" Puntos participación actualizados: %d\n", puntos);
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

//Funciones auxiliares conversion de enums
// Convierte el string de la BD al enum DificultadReto
static DificultadReto string_a_dificultad(const char *str) {
    if (strcmp(str, "MEDIO")   == 0) return MEDIO;
    if (strcmp(str, "DIFICIL") == 0) return DIFICIL;
    return FACIL;  // valor por defecto
}

// Convierte el string de la BD al enum TipoReto
static TipoReto string_a_tipo_reto(const char *str) {
    if (strcmp(str, "HACKATHON") == 0) return HACKATHON;
    return CTF;  // valor por defecto
}

// Convierte el string de la BD al enum EstadoReto
static EstadoReto string_a_estado_reto(const char *str) {
    if (strcmp(str, "EN_CURSO")   == 0) return EN_CURSO;
    if (strcmp(str, "FINALIZADO") == 0) return FINALIZADO;
    return SIN_COMENZAR;  // valor por defecto
}

//Evaluaciones
int insertarEvaluacion(sqlite3 *db,
                       int id_usuario,
                       int id_reto,
                       int puntuacion,
                       const char *comentario) {

    sqlite3_stmt *stmt;

    const char *sql =
        "INSERT INTO Evaluaciones "
        "(id_usuario, id_reto, puntuacion, comentario) "
        "VALUES (?, ?, ?, ?)";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    sqlite3_bind_int(stmt, 1, id_usuario);
    sqlite3_bind_int(stmt, 2, id_reto);
    sqlite3_bind_int(stmt, 3, puntuacion);
    sqlite3_bind_text(stmt, 4, comentario, -1, SQLITE_STATIC);

    resultado = sqlite3_step(stmt);

    if (resultado != SQLITE_DONE) {
        printf("Error insertando evaluacion\n");
        sqlite3_finalize(stmt);
        return resultado;
    }

    printf("Evaluacion insertada correctamente\n");

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

float obtenerMediaReto(sqlite3 *db, int id_reto) {

    sqlite3_stmt *stmt;

    const char *sql =
        "SELECT AVG(puntuacion) "
        "FROM Evaluaciones "
        "WHERE id_reto = ?";

    float media = 0;

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    sqlite3_bind_int(stmt, 1, id_reto);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        media = (float)sqlite3_column_double(stmt, 0);
    }

    sqlite3_finalize(stmt);

    return media;
}

//Solicitudes
int aceptarParticipacion(sqlite3 *db, int id_participacion) {

    sqlite3_stmt *stmt;

    const char *sql =
        "UPDATE Participaciones "
        "SET estado = 'ACEPTADO' "
        "WHERE id = ?";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    sqlite3_bind_int(stmt, 1, id_participacion);

    resultado = sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    if (resultado == SQLITE_DONE) {
        printf("Participacion aceptada\n");
        return SQLITE_OK;
    }

    return resultado;
}
int rechazarParticipacion(sqlite3 *db, int id_participacion) {

    sqlite3_stmt *stmt;

    const char *sql =
        "UPDATE Participaciones "
        "SET estado = 'RECHAZADO' "
        "WHERE id = ?";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    sqlite3_bind_int(stmt, 1, id_participacion);

    resultado = sqlite3_step(stmt);

    sqlite3_finalize(stmt);

    if (resultado == SQLITE_DONE) {
        printf("Participacion rechazada\n");
        return SQLITE_OK;
    }

    return resultado;
}
int listarSolicitudesPendientes(sqlite3 *db, int id_reto) {

    sqlite3_stmt *stmt;

    const char *sql =
        "SELECT id, id_usuario, motivacion "
        "FROM Participaciones "
        "WHERE id_reto = ? AND estado = 'PENDIENTE'";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    sqlite3_bind_int(stmt, 1, id_reto);

    printf("\nSOLICITUDES PENDIENTES:\n");

    while (sqlite3_step(stmt) == SQLITE_ROW) {

        int id = sqlite3_column_int(stmt, 0);
        int usuario = sqlite3_column_int(stmt, 1);

        const char *motivacion =
            (char *)sqlite3_column_text(stmt, 2);

        printf("Solicitud %d | Usuario %d\n",
               id, usuario);

        printf("Motivacion: %s\n\n", motivacion);
    }

    sqlite3_finalize(stmt);

    return SQLITE_OK;
}
//Ranking
int listarRankingEquipos(sqlite3 *db, int id_reto) {

    sqlite3_stmt *stmt;

    const char *sql =
        "SELECT nombre, puntuacion "
        "FROM Equipos "
        "WHERE id_reto = ? "
        "ORDER BY puntuacion DESC";

    int resultado = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

    if (resultado != SQLITE_OK) {
        printf("Error: %s\n", sqlite3_errmsg(db));
        return resultado;
    }

    sqlite3_bind_int(stmt, 1, id_reto);

    printf("\nRANKING EQUIPOS:\n");

    int posicion = 1;

    while (sqlite3_step(stmt) == SQLITE_ROW) {

        const char *nombre =
            (char *)sqlite3_column_text(stmt, 0);

        int puntuacion =
            sqlite3_column_int(stmt, 1);

        printf("%d. %s - %d puntos\n",
               posicion,
               nombre,
               puntuacion);

        posicion++;
    }

    sqlite3_finalize(stmt);

    return SQLITE_OK;
}
