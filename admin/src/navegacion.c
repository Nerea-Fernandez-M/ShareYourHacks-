#include <stdio.h>
#include "../../src/sqlite3.h"
#include "../../src/navegacion.h"

//Funciones de gestion
void navegar(Ventana *v, TipoVentana destino) {
    v->historial[v->historialTop++] = v->actual;
    v->actual = destino;
}

void volver(Ventana *v) {
    if (v->historialTop > 0)
        v->actual = v->historial[--v->historialTop];
    else
        v->actual = VENTANA_MENU_MAIN;
}

void ventanaLimpiar(Ventana *v) {
    if (v->usuario != NULL) {
        free(v->usuario);
        v->usuario = NULL;
    }
}

//Funciones de ventanas
void funcionalidadMenu(Ventana *v){

	//Print del texto
    printf("----Bienvenid@ a ShareYourHacks -----\n"); fflush(stdout);
    printf("1) Ver tus retos activos\n"); fflush(stdout);
    printf("2) Ver proximos retos\n"); fflush(stdout);
    printf("3) Ver todos los retos\n"); fflush(stdout);
    printf("4) Ver tu perfil\n"); fflush(stdout);
    printf("5) Ver Ranking\n"); fflush(stdout);
    printf("6) Organizar un reto\n"); fflush(stdout);
    printf("Introduce un numero: "); fflush(stdout);

	//Gestion de la consola
    int opcion;
    scanf("%d", &opcion);
    while (getchar() != '\n'); //Evita errores

    //Llamada a navegar
    switch (opcion) {
		case 1:
		    if (v->usuario == NULL) {
		        printf("Debes iniciar sesion para ver tus retos activos.\n");
		        break;
		    }
		    v->filtro = FILTRO_ACTIVOS_USUARIO;
		    navegar(v, VENTANA_VER_RETOS);
		    break;
		case 2:
			v->filtro = FILTRO_ACTIVOS;
			navegar(v, VENTANA_VER_RETOS);
			break;
		case 3:
			v->filtro = FILTRO_TODOS;
			navegar(v, VENTANA_VER_RETOS);
			break;
        case 4:
            navegar(v, VENTANA_PERFIL);
            break;
        case 5:
        	v->tipoRanking = RANKING_GLOBAL;
            navegar(v, VENTANA_RANKING);
            break;
        case 6:
        	navegar(v,VENTANA_ORGANIZAR_RETO);
        	break;
        case 0:
            volver(v);
            break;
        default:
            printf("Opcion no valida, intentalo de nuevo.\n"); fflush(stdout);
            break;
    }
}

void funcionalidadPerfil(Ventana *v){

	//Sesion iniciada
	if (v->usuario != NULL){

		//Llamadas a la BD
		int ranking            = 0;
		int puntos_activos     = 0;
		int puntos_organizados = 0;

		obtenerRankingUsuario(v->db, v->usuario->id, &ranking);
		obtenerPuntosParticipacion(v->db, v->usuario->id, &puntos_activos);
		obtenerPuntosOrganizacion (v->db, v->usuario->id, &puntos_organizados);

		//Print del texto
		printf("Hola %s!\n", v->usuario->nombre); fflush(stdout);
		printf("Tu ranking: #%d\n", ranking); fflush(stdout);
		printf("0) Para volver al menu\n"); fflush(stdout);
		printf("1) Ver tus retos activos  (%d pts)\n", puntos_activos); fflush(stdout);
		printf("2) Ver tus retos organizados (%d pts)\n", puntos_organizados); fflush(stdout);
		printf("Que quieres hacer:  \n"); fflush(stdout);

		//Gestion de la consola
	    int opcion;
	    scanf("%d", &opcion);
	    while (getchar() != '\n'); //Evita errores

		//Llamada a navegar
	    switch (opcion) {
	    case 1:
	        v->filtro = FILTRO_ACTIVOS_USUARIO;
	        navegar(v, VENTANA_VER_RETOS);
	        break;
	    case 2:
	        v->filtro = FILTRO_ORGANIZADOS_USUARIO;
			navegar(v, VENTANA_VER_RETOS);
			break;
		case 0:
			volver(v);
			break;
		default:
			printf("Opcion no valida, intentalo de nuevo.\n"); fflush(stdout);
			break;
	    }

	}else{ //Sesion no iniciada

		//Print del texto
		printf("Quieres iniciar sesion (1) o registrarte (2): "); fflush(stdout);

		//Gestion de la consola
	    int opcion;
	    scanf("%d", &opcion);
	    while (getchar() != '\n');

	    //Gestion de opciones
	    switch(opcion){
	    case 0:
	    	volver(v);
	    	break;
	    case 1:
	    	iniciarSesion(v);
	    	break;
	    case 2:
	    	registrar(v);
	    	break;
	    }
	}
}

void funcionalidadVerRetos(Ventana *v) {

    Reto retos[MAX_RETOS];
    int  cantidad = 0;

    // Seleccionar la query según el filtro
    switch (v->filtro) {
        case FILTRO_TODOS:
            listarTodosRetos(v->db, retos, &cantidad);
            break;
        case FILTRO_ACTIVOS:
            listarRetosActivos(v->db, retos, &cantidad);
            break;
        case FILTRO_USUARIO:
            listarRetosUsuario(v->db, v->usuario->id, retos, &cantidad);
            break;
        case FILTRO_ACTIVOS_USUARIO:
            listarRetosActivosUsuario(v->db, v->usuario->id, retos, &cantidad);
            break;
        case FILTRO_ORGANIZADOS_USUARIO:
            listarRetosOrganizadosUsuario(v->db, v->usuario->id, retos, &cantidad);
            break;
    }

    // Print de los retos
    if (cantidad == 0) {
        printf("No hay retos para mostrar.\n"); fflush(stdout);
    } else {
        for (int i = 0; i < cantidad; i++) {
        	printf("%d) %-40s | %s | %d pts\n",
        	       i + 1,
        	       retos[i].titulo,
        	       estado_reto_a_string(retos[i].estadoReto),
        	       retos[i].puntos);
        }
    }

    printf("Pulsa el numero del reto cuyos datos quieras ver\n"); fflush(stdout);
    printf("Pulsa 0 para volver a la ventana anterior\n"); fflush(stdout);
    printf("Que quieres hacer: "); fflush(stdout);

    int opcion;
    scanf("%d", &opcion);
    while (getchar() != '\n');

    if (opcion == 0) {
        volver(v);
        return;
    }

    if (opcion >= 1 && opcion <= cantidad) {
        v->retoSeleccionadoId = retos[opcion - 1].id;  // guardas solo el id
        navegar(v, VENTANA_RETO);
        return;
    }

    printf("Opcion no valida, intentalo de nuevo.\n"); fflush(stdout);
}

void funcionalidadReto(Ventana *v){

	//gestionar estado del reto
	Reto reto;
	obtenerRetoPorId(v->db, v->retoSeleccionadoId, &reto);  // nueva funcion de BD

	if(reto.estadoReto == SIN_COMENZAR){

		//Llamadas a BD
		char nombre[30] = "";
		char email[50] = "";
		obtenerUsuario(v->db, reto.id_organizador, &nombre, &email);

		//Prints
		printf("%s\n",reto.titulo); fflush(stdout);
		printf("Creado por:%s \n", nombre); fflush(stdout);
		printf("Resumen: %s\n",reto.descripcion); fflush(stdout);
		printf("Tipo de reto: %s\n",tipo_reto_a_string(reto.tipoReto)); fflush(stdout);
		printf("Inscripcion: %s-%s\n",reto.fecha_i_inscripcion,reto.fecha_f_inscripcion ); fflush(stdout);
		printf("Dificultad: %s\n",dificultad_reto_a_string(reto.dificultadReto)); fflush(stdout);
		printf("Puntos: %d\n",reto.puntos); fflush(stdout);
		printf("Pulsa 1 para apuntarte o 0 para volver a la pagina anterior:  \n"); fflush(stdout);

		//Gestion navegacion
	    int opcion;
	    scanf("%d", &opcion);
	    while (getchar() != '\n');

	    switch(opcion){
	    	    case 0:
	    	    	volver(v);
	    	    	break;
	    	    case 1:
	    	    	navegar(v,VENTANA_RETO_COMPLETO);
	    	    	break;
		        default:
		            printf("Opcion no valida, intentalo de nuevo.\n"); fflush(stdout);
		            break;
	    	    }
	}else if(reto.estadoReto == EN_CURSO){

		//Llamadas a BD
		char nombre[30] = "";
		char email[50] = "";
		obtenerUsuario(v->db, reto.id_organizador, &nombre, &email);

		TipoRol rol = HACKER;
		int puesto = 0;
		int dias_restantes = 0;
		obtenerRolEnReto (v->db, v->usuario->id,reto.id, &rol);
		obtenerPuestoEnReto (v->db, v->usuario->id, reto.id, &puesto);
		obtenerDiasRestantes (v->db, reto.id, &dias_restantes);

		//Prints
		printf("%s\n",reto.titulo); fflush(stdout);
		printf("Creado por:%s \n", nombre); fflush(stdout);
		printf("Resumen: %s\n",reto.descripcion); fflush(stdout);
		printf("Tu rol: %s\n", tipo_rol_a_string(rol)); fflush(stdout);
		printf("Tipo de reto: %s\n",tipo_reto_a_string(reto.tipoReto)); fflush(stdout);
		printf("Tiempo restante: %d dias\n", dias_restantes); fflush(stdout);
		printf("Tu puesto: #%d\n",  puesto); fflush(stdout);
		printf("1) Ver detalles\n"); fflush(stdout);
		printf("2) Ver ranking del reto\n"); fflush(stdout);
		printf("0) Volver a la pantalla anterior\n"); fflush(stdout);
		printf("Que quieres hacer: \n"); fflush(stdout);

		//Gestion navegacion
	    int opcion;
	    scanf("%d", &opcion);
	    while (getchar() != '\n');

	    switch(opcion){
	    case 0:
	    	volver(v);
	    	break;
	    case 1:
	    	navegar(v,VENTANA_RETO_COMPLETO);
	    	break;
	    case 2:
	    	v->tipoRanking = RANKING_RETO;
	    	navegar(v,VENTANA_RANKING);
	    	break;
        default:
            printf("Opcion no valida, intentalo de nuevo.\n"); fflush(stdout);
            break;
	    }

	}else{ //Reto finalizado

	    Reto reto;
	    obtenerRetoPorId(v->db, v->retoSeleccionadoId, &reto);

		//Llamadas a BD
		char nombre[30] = "";
		char email[50] = "";
		obtenerUsuario(v->db, reto.id_organizador, &nombre, &email);

		TipoRol rol = HACKER;
		obtenerRolEnReto        (v->db, v->usuario->id, reto.id, &rol);

		//Prints
		printf("%s\n",reto.titulo); fflush(stdout);
		printf("Creado por:%s\n", nombre); fflush(stdout);
		printf("Resumen: %s\n",reto.descripcion); fflush(stdout);
		printf("Este reto ya ha finalizado\n"); fflush(stdout);
		printf("Tipo de reto: %s\n",tipo_reto_a_string(reto.tipoReto)); fflush(stdout);
		printf("Pulsa 1 para ver el ranking del reto o 0 para volver a la ventana anterior:  \n"); fflush(stdout);

		//Gestion navegacion
	    int opcion;
	    scanf("%d", &opcion);
	    while (getchar() != '\n');

	    switch(opcion){
	    case 0:
	    	volver(v);
	    	break;
	    case 1:
	    	v->tipoRanking = RANKING_RETO;
	    	navegar(v,VENTANA_RANKING);
	    	break;
        default:
            printf("Opcion no valida, intentalo de nuevo.\n"); fflush(stdout);
            break;
	    }
	}
}

void funcionalidadRetoCompleto(Ventana *v){

    Reto reto;
    obtenerRetoPorId(v->db, v->retoSeleccionadoId, &reto);

	///Llamdas A Bd
	char nombre[30] = "";
	char email[50] = "";
	obtenerUsuario(v->db, reto.id_organizador, &nombre, &email);

	TipoRol rol = HACKER;
	obtenerRolEnReto (v->db, v->usuario->id, reto.id, &rol);
	int puesto = 0;
	obtenerPuestoEnReto (v->db, v->usuario->id,reto.id, &puesto);

	//Print del texto
	printf("%s\n",reto.titulo); fflush(stdout);
	printf("Creado por:%s \n", nombre); fflush(stdout);
	printf("Resumen: %s\n",reto.descripcion); fflush(stdout);
	printf("Tipo de reto: %s\n",tipo_reto_a_string(reto.tipoReto)); fflush(stdout);
	printf("Tu puesto: #%d\n",  puesto); fflush(stdout);
	printf("Pulsa 1 para descargar archivos adicionales o 0 para volver al menu: "); fflush(stdout);

	//Gestion de la consola
    int opcion;
    scanf("%d", &opcion);
    while (getchar() != '\n');

    switch(opcion){
    case 0:
    	volver(v);
    	break;
    case 1:
    	printf("No hay archivos extra para este reto"); fflush(stdout);
    	navegar(v,VENTANA_RETO);
    	break;
    default:
        printf("Opcion no valida, intentalo de nuevo.\n"); fflush(stdout);
        break;
    }
}

void funcionalidadApuntarse(Ventana *v){

    Reto reto;
    obtenerRetoPorId(v->db, v->retoSeleccionadoId, &reto);

	// Titulo del reto
	printf("%s\n", reto.titulo); fflush(stdout);

	// Elegir rol
	printf("Pulsa 1 para ser Hacker o 2 para ser miembro del Staff: "); fflush(stdout);
	int opcion_rol;
	scanf("%d", &opcion_rol);
	while (getchar() != '\n');

	int id_rol;
	if (opcion_rol == 2) {
		id_rol = 3;  // STAFF
	} else {
		id_rol = 2;  // HACKER
	}

	// Motivacion (puede contener espacios, usamos fgets)
	char motivacion[256];
	printf("Escribe brevemente tu motivacion para participar en este reto: "); fflush(stdout);
	fgets(motivacion, sizeof(motivacion), stdin);
	motivacion[strcspn(motivacion, "\n")] = '\0';

	// Confirmar inscripcion
	printf("Pulsa 1 para apuntarte o 0 para volver al menu: "); fflush(stdout);
	int confirmar;
	scanf("%d", &confirmar);
	while (getchar() != '\n');

	if (confirmar == 0) {
		volver(v);
		return;
	}

	// Comprobar si el reto requiere equipo
	int requiere_equipo = 0;
	retoRequiereEquipo(v->db,reto.id, &requiere_equipo);

	if (!requiere_equipo) {
		//Inscripcion directa
		int resultado = insertarParticipacion(v->db, v->usuario->id,
											  reto.id,
											  0, motivacion);
		if (resultado == SQLITE_OK){
			printf("Te has inscrito en %s correctamente.\n",reto.titulo); fflush(stdout);
		}
		else{
			printf("Algo ha ido mal al inscribirse, intentalo de nuevo.\n"); fflush(stdout);
		}
		volver(v);
		return;
	}

	// S requiere qeuipo hay que gestionarlo
	printf("Pulsa 1 para unirte a un equipo o 2 para crear uno: "); fflush(stdout);
	int opcion_equipo;
	scanf("%d", &opcion_equipo);
	while (getchar() != '\n');

	int id_equipo = 0;

	if (opcion_equipo == 1) {

		// Unirse a equipo existente por nombre
		while (1) {
			printf("Introduce el nombre de tu equipo: "); fflush(stdout);
			char nombre_equipo[64];
			scanf("%63s", nombre_equipo);
			while (getchar() != '\n');

			int encontrado = obtenerEquipoPorNombre(v->db, nombre_equipo,
													reto.id,
													&id_equipo);
			if (encontrado == SQLITE_OK) {
				int unido = unirseAEquipo(v->db, v->usuario->id, id_equipo);
				if (unido == SQLITE_OK) {
					printf("Te has unido al grupo %s correctamente.\n",nombre_equipo);  fflush(stdout);
					break;
				}
			}

			printf("Algo ha ido mal, vuelve a introducir el nombre de tu equipo o pulsa 0 para salir: "); fflush(stdout);
			int salir;
			scanf("%d", &salir);
			while (getchar() != '\n');

			if (salir == 0) {
				volver(v);
				return;
			}
		}

	} else {
		// Crear equipo nuevo
		while (1) {
			printf("Introduce el nombre de tu equipo: "); fflush(stdout);
			char nombre_equipo[64];
			scanf("%63s", nombre_equipo);
			while (getchar() != '\n');

			// El codigo del equipo es su id en la BD
			char ids_usuario[16];
			snprintf(ids_usuario, sizeof(ids_usuario), "%d", v->usuario->id);

			int resultado = insertarEquipo(v->db, nombre_equipo,
										   reto.id,
										   ids_usuario);
			if (resultado == SQLITE_OK) {
				// Recuperar el id del equipo recien creado
				obtenerEquipoPorNombre(v->db, nombre_equipo,
									   reto.id,
									   &id_equipo);
				printf("Este es el codigo de tu equipo: %d\n", id_equipo); fflush(stdout);

				// Actualizar num_miembros
				unirseAEquipo(v->db, v->usuario->id, id_equipo);
				break;
			}
			printf("Este nombre no es valido, prueba con otro.\n"); fflush(stdout);
		}
	}

	// Insertar participacion con el equipo asignado
	int resultado = insertarParticipacion(v->db, v->usuario->id,
										  reto.id,
										  id_equipo, motivacion);
	if (resultado == SQLITE_OK){
		printf("Te has inscrito en %s correctamente.\n",reto.titulo); fflush(stdout);
	}
	else{
		printf("Algo ha ido mal al inscribirse, intentalo de nuevo.\n"); fflush(stdout);
	}
	volver(v);
}


void funcionalidadRanking(Ventana *v) {

    Reto reto;
    obtenerRetoPorId(v->db, v->retoSeleccionadoId, &reto);

    Usuario usuarios[MAX_USUARIOS];
    int     cantidad = 0;

    // Cargar datos y titulo según el tipo de ranking
    if (v->tipoRanking == RANKING_GLOBAL) {
        printf("=== Ranking global ===\n"); fflush(stdout);
        listarRankingGlobal(v->db, usuarios, &cantidad);
    } else {
        printf("=== Ranking: %s ===\n",reto.titulo); fflush(stdout);
        listarRankingReto(v->db, reto.id, usuarios, &cantidad);
    }

    // Print del ranking
    if (cantidad == 0) {
        printf("No hay datos para mostrar.\n"); fflush(stdout);
    } else {
        for (int i = 0; i < cantidad; i++) {
            printf("#%-3d %-30s %d pts\n",
                   i + 1,
                   usuarios[i].nombre,
                   usuarios[i].total_puntos);fflush(stdout);
        }
    }

    printf("Pulsa 0 para volver: ");fflush(stdout);

    int opcion;
    scanf("%d", &opcion);
    while (getchar() != '\n');

    volver(v);
}

void funcionalidadOrganizarReto(Ventana *v) {

    char titulo[128];
    char resumen[512];
    char tipo[16];
    char fecha_inicio_insc[11];
    char fecha_fin_insc[11];
    char fecha_inicio[11];
    char fecha_fin[11];
    char dificultad[16];
    int  puntos = 0;
    int limite_de_plazas = 0;

    printf("Introduce los siguientes datos de tu reto\n");fflush(stdout);

    printf("Titulo del reto: \n");fflush(stdout);
    fgets(titulo, sizeof(titulo), stdin);
    titulo[strcspn(titulo, "\n")] = '\0';

    printf("Resumen: \n");fflush(stdout);
    fgets(resumen, sizeof(resumen), stdin);
    resumen[strcspn(resumen, "\n")] = '\0';

    // Tipo: hace la validacion previa
    while (1) {
        printf("Tipo (CTF / HACKATHON): \n");fflush(stdout);
        scanf("%15s", tipo);
        while (getchar() != '\n');

        // Convertir a mayusculas para comparar
        for (int i = 0; tipo[i]; i++)
            tipo[i] = toupper(tipo[i]);

        if (strcmp(tipo, "CTF") == 0 || strcmp(tipo, "HACKATHON") == 0)
            break;

        printf("Tipo no valido, escribe CTF o HACKATHON.\n");fflush(stdout);
    }

    printf("Escribe las siguientes fechas en formato (dd/mm/aaaa)\n");fflush(stdout);

    printf("Inicio inscripcion: \n");fflush(stdout);
    scanf("%10s", fecha_inicio_insc);
    while (getchar() != '\n');

    printf("Fin inscripcion: \n");fflush(stdout);
    scanf("%10s", fecha_fin_insc);
    while (getchar() != '\n');

    printf("Inicio reto: \n");fflush(stdout);
    scanf("%10s", fecha_inicio);
    while (getchar() != '\n');

    printf("Fin reto: \n");fflush(stdout);
    scanf("%10s", fecha_fin);
    while (getchar() != '\n');

    // Dificultad: Hace la validacion previa
    while (1) {
        printf("Dificultad (FACIL / MEDIO / DIFICIL): \n");fflush(stdout);
        scanf("%15s", dificultad);
        while (getchar() != '\n');

        for (int i = 0; dificultad[i]; i++)
            dificultad[i] = toupper(dificultad[i]);

        if (strcmp(dificultad, "FACIL")  == 0 ||
            strcmp(dificultad, "MEDIO")  == 0 ||
            strcmp(dificultad, "DIFICIL") == 0)
            break;

        printf("Dificultad no valida, escribe FACIL, MEDIO o DIFICIL.\n");fflush(stdout);
    }

    printf("Puntos: \n");fflush(stdout);
    scanf("%d", &puntos);
    while (getchar() != '\n');

    printf("Limite de plazas: \n");fflush(stdout);
    scanf("%d", &limite_de_plazas);
    while (getchar() != '\n');

    // Confirmar
    printf("Para crear el reto pulsa 1, para volver al menu pulsa 0: \n");fflush(stdout);
    int confirmar;
    scanf("%d", &confirmar);
    while (getchar() != '\n');

    if (confirmar == 0) {
        volver(v);
        return;
    }

    int resultado = insertarReto(v->db, titulo, resumen,
                                 dificultad, limite_de_plazas,
                                 fecha_inicio, fecha_fin,
                                 fecha_inicio_insc, fecha_fin_insc,
                                 puntos, v->usuario->id);
    if (resultado == SQLITE_OK){
        printf("Reto \"%s\" creado correctamente.\n", titulo);fflush(stdout);
    }else{
        printf("Algo ha ido mal al crear el reto, intentalo de nuevo.\n");fflush(stdout);
	}
    volver(v);
}


//Funciones suplementarias
void iniciarSesion(Ventana *v) {

    char nombre[64];
    char contrasena[128];

    while (1) {
        // Pedir credenciales
        printf("Nombre de usuario: ");fflush(stdout);
        scanf("%63s", nombre);
        while (getchar() != '\n');

        printf("Contrasena: ");fflush(stdout);
        scanf("%127s", contrasena);
        while (getchar() != '\n');

        // Consultar la BD
        int id     = 0;
        int puntos = 0;
        int resultado = obtenerUsuarioPorCredenciales(v->db, nombre, contrasena,
                                                      &id, &puntos);

        if (resultado == SQLITE_OK) {
            // Credenciales correctas, rellenar el usuario en la ventana
            v->usuario = malloc(sizeof(Usuario));
            v->usuario->id          = id;
            v->usuario->total_puntos = puntos;
            strncpy(v->usuario->nombre, nombre, sizeof(v->usuario->nombre) - 1);

            return;
        }

        // Credenciales incorrectas
        printf("Nombre de usuario o contrasena incorrectos\n"); fflush(stdout);
        printf("Pulsa 1 para intentarlo de nuevo o 0 para volver: ");fflush(stdout);

        int opcion;
        scanf("%d", &opcion);
        while (getchar() != '\n');

        if (opcion == 0) {
            volver(v);
            return;
        }
        // si opcion == 1 el while repite y vuelve a pedir credenciales
    }
}

void registrar(Ventana *v) {

    char email[128];
    char nombre[64];
    char contrasena[128];
    char contrasena2[128];

    while (1) {
        printf("Correo electronico: ");fflush(stdout);
        scanf("%127s", email);
        while (getchar() != '\n');

        printf("Nombre de usuario: ");fflush(stdout);
        scanf("%63s", nombre);
        while (getchar() != '\n');

        printf("Contrasena: ");fflush(stdout);
        scanf("%127s", contrasena);
        while (getchar() != '\n');

        printf("Repetir contrasena: ");fflush(stdout);
        scanf("%127s", contrasena2);
        while (getchar() != '\n');

        // Validar que las contraseñas coinciden
        if (strcmp(contrasena, contrasena2) != 0) {
            printf("Las contrasenas no coinciden, intentalo de nuevo.\n\n"); fflush(stdout);
            continue;
        }

        // Insertar en la BD con rol 2 (Hacker)
        int resultado = insertarUsuario(v->db, nombre, email, contrasena, 2);

        if (resultado == SQLITE_OK) {
            // Registro correcto, iniciar sesion automaticamente
            int id     = 0;
            int puntos = 0;
            obtenerUsuarioPorCredenciales(v->db, nombre, contrasena, &id, &puntos);

            v->usuario = malloc(sizeof(Usuario));
            v->usuario->id           = id;
            v->usuario->total_puntos = puntos;
            strncpy(v->usuario->nombre, nombre, sizeof(v->usuario->nombre) - 1);

            return;
        }

        // Error en la BD (nombre o email duplicado, u otro error)
        printf("No se ha podido registrar el usuario, intentalo de nuevo.\n\n"); fflush(stdout);
    }
}

