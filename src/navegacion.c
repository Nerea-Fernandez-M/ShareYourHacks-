#include <stdio.h>
#include "sqlite3.h"
#include "navegacion.h"

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

//Funciones de ventanas
void funcionalidadMenu(Ventana *v){

	//Print del texto
	printf("----Bienvenid@ a ShareYourHacks -----\n");
	printf("1) Ver tus retos activos\n");
	printf("2) Ver próximos retos\n");
	printf("3) Ver todos los retos\n");
	printf("4) Ver tu perfil\n");
	printf("5) Ver Ranking\n");
	printf("6) Organizar un reto\n");
	printf("Introduce uno de los siguientes numeros para moverte por el menu:  \n");

	//Gestion de la consola
    int opcion;
    scanf("%d", &opcion);
    while (getchar() != '\n'); //Evita errores

    //Llamada a navegar
    switch (opcion) {
        case 1: case 2: case 3:
            v->filtro = opcion;
            navegar(v, VENTANA_VER_RETOS);
            break;
        case 4:
            navegar(v, VENTANA_PERFIL);
            break;
        case 5:
            navegar(v, VENTANA_RANKING);
            break;
        case 6:
        	navegar(v,VENTANA_ORGANIZAR_RETO);
        	break;
        case 0:
            v->actual = VENTANA_EXIT;
            break;
        default:
            printf("Opcion no valida, intentalo de nuevo.\n");
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

		obtenerRankingUsuario     (v->db, v->usuario->id, &ranking);
		obtenerPuntosParticipacion(v->db, v->usuario->id, &puntos_activos);
		obtenerPuntosOrganizacion (v->db, v->usuario->id, &puntos_organizados);

		//Print del texto
		printf("Hola %s!\n", v->usuario->nombre);
		printf("Tu ranking: #%d\n", ranking);
		printf("0) Para volver al menu\n");
		printf("1) Ver tus retos activos  (%d pts)\n", puntos_activos);
		printf("2) Ver tus retos organizados (%d pts)\n", puntos_organizados);
		printf("Que quieres hacer:  \n");

		//Gestion de la consola
	    int opcion;
	    scanf("%d", &opcion);
	    while (getchar() != '\n'); //Evita errores

		//Llamada a navegar
	    switch (opcion) {
	        case 1: case 2:
	            v->filtro = opcion;
	            navegar(v, VENTANA_VER_RETOS);
	            break;
	        case 0:
	            v->actual = VENTANA_EXIT;
	            break;
	        default:
	            printf("Opcion no valida, intentalo de nuevo.\n");
	            break;
	    }

	}else{ //Sesion no iniciada

		//Print del texto
		printf("Quieres iniciar sesion (1) o registrarte (2): ");

		//Gestion de la consola
	    int opcion;
	    scanf("%d", &opcion);
	    while (getchar() != '\n');

	    //Gestion de opciones
	    switch(opcion){
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
        printf("No hay retos para mostrar.\n");
    } else {
        for (int i = 0; i < cantidad; i++) {
            printf("%d) %-40s | %s | %d pts\n",
                   i + 1,
                   retos[i].titulo,
                   retos[i].estadoReto,
                   retos[i].puntos);
        }
    }

    printf("Pulsa el numero del reto cuyos datos quieras ver\n");
    printf("Pulsa 0 para volver a la ventana anterior\n");
    printf("Que quieres hacer: ");

    int opcion;
    scanf("%d", &opcion);
    while (getchar() != '\n');

    if (opcion == 0) {
        volver(v);
        return;
    }

    if (opcion >= 1 && opcion <= cantidad) {
        v->retoSeleccionado = retos[opcion - 1].id;
        navegar(v, VENTANA_RETO);
        return;
    }

    printf("Opcion no valida, intentalo de nuevo.\n");
}

void funcionalidadReto(Ventana *v){

	//gestionar estado del reto
	if(v->retoSeleccionado->estadoReto == SIN_COMENZAR){

		//Llamadas a BD
		char nombre[30] = "";
		char email[50] = "";
		obtenerUsuario(v->db, v->retoSeleccionado->id_organizador, &nombre, &email);

		//Prints
		printf("%s\n",v->retoSeleccionado->titulo);
		printf("Creado por:%s \n", nombre);
		printf("Resumen: %s\n",v->retoSeleccionado->descripcion);
		printf("Tipo de reto: %s\n",tipo_reto_a_string(v->retoSeleccionado->tipoReto));
		printf("Inscripcion: %s-%s\n",v->retoSeleccionado->fecha_i_inscripcion,v->retoSeleccionado->fecha_f_inscripcion );
		printf("Dificultad: %s\n",dificultad_reto_a_string(v->retoSeleccionado->dificultadReto));
		printf("Puntos: %d\n",v->retoSeleccionado->puntos);
		printf("Pulsa 1 para apuntarte o 0 para volver a la pagina anterior:  \n");

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
		            printf("Opcion no valida, intentalo de nuevo.\n");
		            break;
	    	    }
	}else if(v->retoSeleccionado->estadoReto == EN_CURSO){

		//Llamadas a BD
		char nombre[30] = "";
		char email[50] = "";
		obtenerUsuario(v->db, v->retoSeleccionado->id_organizador, &nombre, &email);

		TipoRol rol = HACKER;
		int puesto = 0;
		int dias_restantes = 0;
		obtenerRolEnReto (v->db, v->usuario->id, v->retoSeleccionado->id, &rol);
		obtenerPuestoEnReto (v->db, v->usuario->id, v->retoSeleccionado->id, &puesto);
		obtenerDiasRestantes (v->db, v->retoSeleccionado->id, &dias_restantes);

		//Prints
		printf("%s\n",v->retoSeleccionado->titulo);
		printf("Creado por:%s \n", nombre);
		printf("Resumen: %s\n",v->retoSeleccionado->descripcion);
		printf("Tu rol: %s\n", tipo_rol_a_string(rol));
		printf("Tipo de reto: %s\n",tipo_reto_a_string(v->retoSeleccionado->tipoReto));
		printf("Tiempo restante: %d dias\n", dias_restantes);
		printf("Tu puesto: #%d\n",  puesto);
		printf("1) Ver detalles\n");
		printf("2) Ver ranking del reto\n");
		printf("0) Volver a la pantalla anterior\n");
		printf("Que quieres hacer: \n");

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
	    	navegar(v,VENTANA_RANKING);
	    	break;
        default:
            printf("Opcion no valida, intentalo de nuevo.\n");
            break;
	    }

	}else{ //Reto finalizado

		//Llamadas a BD
		char nombre[30] = "";
		char email[50] = "";
		obtenerUsuario(v->db, v->retoSeleccionado->id_organizador, &nombre, &email);

		TipoRol rol = HACKER;
		obtenerRolEnReto        (v->db, v->usuario->id, v->retoSeleccionado->id, &rol);

		//Prints
		printf("%s\n",v->retoSeleccionado->titulo);
		printf("Creado por:%s\n", nombre);
		printf("Resumen: %s\n",v->retoSeleccionado->descripcion);
		printf("Este reto ya ha finalizado\n");
		printf("Tipo de reto: %s\n",tipo_reto_a_string(v->retoSeleccionado->tipoReto));
		printf("Pulsa 1 para ver el ranking del reto o 0 para volver a la ventana anterior:  \n");

		//Gestion navegacion
	    int opcion;
	    scanf("%d", &opcion);
	    while (getchar() != '\n');

	    switch(opcion){
	    case 0:
	    	volver(v);
	    	break;
	    case 1:
	    	navegar(v,VENTANA_RANKING);
	    	break;
        default:
            printf("Opcion no valida, intentalo de nuevo.\n");
            break;
	    }
	}
}

void funcionalidadRetoCompleto(Ventana *v){

	///Llamdas A Bd
	char nombre[30] = "";
	char email[50] = "";
	obtenerUsuario(v->db, v->retoSeleccionado->id_organizador, &nombre, &email);

	TipoRol rol = HACKER;
	obtenerRolEnReto (v->db, v->usuario->id, v->retoSeleccionado->id, &rol);
	int puesto = 0;
	obtenerPuestoEnReto (v->db, v->usuario->id, v->retoSeleccionado->id, &puesto);

	//Print del texto
	printf("%s\n",v->retoSeleccionado->titulo);
	printf("Creado por:%s \n", nombre);
	printf("Resumen: %s\n",v->retoSeleccionado->descripcion);
	printf("Tipo de reto: %s\n",tipo_reto_a_string(v->retoSeleccionado->tipoReto));
	printf("Tu puesto: #%d\n",  puesto);
	printf("Pulsa 1 para descargar archivos adicionales o 0 para volver al menu: ");

	//Gestion de la consola
    int opcion;
    scanf("%d", &opcion);
    while (getchar() != '\n');

    switch(opcion){
    case 0:
    	volver(v);
    	break;
    case 1:
    	printf("No hay archivos extra para este reto");
    	navegar(v,VENTANA_RETO);
    	break;
    default:
        printf("Opcion no valida, intentalo de nuevo.\n");
        break;
    }
}

void funcionalidadApuntarse(Ventana *v){

	void funcionalidadApuntarseReto(Ventana *v) {

	    // Titulo del reto
	    printf("%s\n", v->retoSeleccionado->titulo);

	    // Elegir rol
	    printf("Pulsa 1 para ser Hacker o 2 para ser miembro del Staff: ");
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
	    printf("Escribe brevemente tu motivacion para participar en este reto: ");
	    fgets(motivacion, sizeof(motivacion), stdin);
	    motivacion[strcspn(motivacion, "\n")] = '\0';

	    // Confirmar inscripcion
	    printf("Pulsa 1 para apuntarte o 0 para volver al menu: ");
	    int confirmar;
	    scanf("%d", &confirmar);
	    while (getchar() != '\n');

	    if (confirmar == 0) {
	        volver(v);
	        return;
	    }

	    // Comprobar si el reto requiere equipo
	    int requiere_equipo = 0;
	    retoRequiereEquipo(v->db, v->retoSeleccionado->id, &requiere_equipo);

	    if (!requiere_equipo) {
	        //Inscripcion directa
	        int resultado = insertarParticipacion(v->db, v->usuario->id,
	                                              v->retoSeleccionado->id,
	                                              0, motivacion);
	        if (resultado == SQLITE_OK)
	            printf("Te has inscrito en %s correctamente.\n",
	                   v->retoSeleccionado->titulo);
	        else
	            printf("Algo ha ido mal al inscribirse, intentalo de nuevo.\n");

	        volver(v);
	        return;
	    }

	    // S requiere qeuipo hay que gestionarlo
	    printf("Pulsa 1 para unirte a un equipo o 2 para crear uno: ");
	    int opcion_equipo;
	    scanf("%d", &opcion_equipo);
	    while (getchar() != '\n');

	    int id_equipo = 0;

	    if (opcion_equipo == 1) {

	        // Unirse a equipo existente por nombre
	        while (1) {
	            printf("Introduce el nombre de tu equipo: ");
	            char nombre_equipo[64];
	            scanf("%63s", nombre_equipo);
	            while (getchar() != '\n');

	            int encontrado = obtenerEquipoPorNombre(v->db, nombre_equipo,
	                                                    v->retoSeleccionado->id,
	                                                    &id_equipo);
	            if (encontrado == SQLITE_OK) {
	                int unido = unirseAEquipo(v->db, v->usuario->id, id_equipo);
	                if (unido == SQLITE_OK) {
	                    printf("Te has unido al grupo %s correctamente.\n",
	                           nombre_equipo);
	                    break;
	                }
	            }

	            printf("Algo ha ido mal, vuelve a introducir el nombre "
	                   "de tu equipo o pulsa 0 para salir: ");
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
	            printf("Introduce el nombre de tu equipo: ");
	            char nombre_equipo[64];
	            scanf("%63s", nombre_equipo);
	            while (getchar() != '\n');

	            // El codigo del equipo es su id en la BD
	            char ids_usuario[16];
	            snprintf(ids_usuario, sizeof(ids_usuario), "%d", v->usuario->id);

	            int resultado = insertarEquipo(v->db, nombre_equipo,
	                                           v->retoSeleccionado->id,
	                                           ids_usuario);
	            if (resultado == SQLITE_OK) {
	                // Recuperar el id del equipo recien creado
	                obtenerEquipoPorNombre(v->db, nombre_equipo,
	                                       v->retoSeleccionado->id,
	                                       &id_equipo);
	                printf("Este es el codigo de tu equipo: %d\n", id_equipo);

	                // Actualizar num_miembros
	                unirseAEquipo(v->db, v->usuario->id, id_equipo);
	                break;
	            }
	            printf("Este nombre no es valido, prueba con otro.\n");
	        }
	    }

	    // Insertar participacion con el equipo asignado
	    int resultado = insertarParticipacion(v->db, v->usuario->id,
	                                          v->retoSeleccionado->id,
	                                          id_equipo, motivacion);
	    if (resultado == SQLITE_OK)
	        printf("Te has inscrito en %s correctamente.\n",
	               v->retoSeleccionado->titulo);
	    else
	        printf("Algo ha ido mal al inscribirse, intentalo de nuevo.\n");

	    volver(v);
	}
}

void funcionalidadRanking(Ventana *v){

	//Print del texto

	//Gestion de la consola

	//Llamada a navegar
}

void funcionalidadOrganizarReto(Ventana *v){

	//Print del texto

	//Gestion de la consola

	//Llamada a navegar
}


//Funciones suplementarias
void iniciarSesion(Ventana *v) {

    char nombre[64];
    char contrasena[128];

    while (1) {
        // Pedir credenciales
        printf("Nombre de usuario: ");
        scanf("%63s", nombre);
        while (getchar() != '\n');

        printf("Contrasena: ");
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

            navegar(v, VENTANA_PERFIL);
            return;
        }

        // Credenciales incorrectas
        printf("Nombre de usuario o contrasena incorrectos\n");
        printf("Pulsa 1 para intentarlo de nuevo o 0 para volver: ");

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
        printf("Correo electronico: ");
        scanf("%127s", email);
        while (getchar() != '\n');

        printf("Nombre de usuario: ");
        scanf("%63s", nombre);
        while (getchar() != '\n');

        printf("Contrasena: ");
        scanf("%127s", contrasena);
        while (getchar() != '\n');

        printf("Repetir contrasena: ");
        scanf("%127s", contrasena2);
        while (getchar() != '\n');

        // Validar que las contraseñas coinciden
        if (strcmp(contrasena, contrasena2) != 0) {
            printf("Las contrasenas no coinciden, intentalo de nuevo.\n\n");
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

            navegar(v, VENTANA_PERFIL);
            return;
        }

        // Error en la BD (nombre o email duplicado, u otro error)
        printf("No se ha podido registrar el usuario, intentalo de nuevo.\n\n");
    }
}
