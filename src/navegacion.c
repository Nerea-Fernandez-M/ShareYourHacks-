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

	//Print del texto

	//Gestion de la consola

	//Llamada a navegar
}

void funcionalidadRetoCompleto(Ventana *v){

	//Print del texto

	//Gestion de la consola

	//Llamada a navegar
}

void funcionalidadApuntarse(Ventana *v){

	//Print del texto

	//Gestion de la consola

	//Llamada a navegar
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
