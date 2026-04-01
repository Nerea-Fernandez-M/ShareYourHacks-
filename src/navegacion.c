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
	"----Bienvenid@ a ShareYourHacks -----\n"
	"1) Ver tus retos activos\n"
	"2) Ver próximos retos\n"
	"3) Ver todos los retos\n"
	"4) Ver tu perfil\n"
	"5) Ver Ranking\n"
	"6) Organizar un reto\n"
	"Introduce uno de los siguientes numeros para moverte por el menu:  \n"

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
		//Print del texto
		"Hola nombre!\n" //TODO introucir datos de la BD
		"Tu ranking: \n"
		"0) Para volver al menu"
		"1) Ver tus retos activos  (x pts)\n"
		"2) Ver tus retos organizados (x pts)\n"
		"Que quieres hacer:  \n"

		//Gestion de la consola
	    int opcion;
	    scanf("%d", &opcion);
	    while (getchar() != '\n'); //Evita errores

		//Llamada a navegar
	    switch (opcion) {
	        case 1: case 2: //Todo cambiar opcion de retos organizados
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
		"Quieres iniciar sesion (1) o registrarte (2): "

		//Gestion de la consola
	    int opcion;
	    scanf("%d", &opcion);
	    while (getchar() != '\n');

	    //Gestion de opciones
	    switch(opcion){
	    case 1:
	    	iniciarSesion(v);
	    case 2:
	    	registrar(v);
	    }
	}
}

void funcionalidadVerRetos(Ventana *v){

	//Hacer query a la BD

	//Print del texto
	“Print de los retos correspondientes”
	"Pulsa el numero de el reto cuyos datos quieras ver\n"
	"Pulsa 0 para volver a la ventana anterior\n"
	"Que quieres hacer:  \n"

	//Gestion de la consola
    int opcion;
    scanf("%d", &opcion);
    while (getchar() != '\n'); //Evita errores

	//Llamada a navegar
    if (opcion == 0) { //Vuelve para atras
        db_free_retos(retos);
        volver(v);
        return;
    }

    //Navega por los retos
    if (opcion >= 1 && opcion <= cantidad) { //n de retos obtenidos
        v->reto_seleccionado_id = retos[opcion - 1].id;  // guardas el id antes de liberar
        db_free_retos(retos);
        navegar(v, VENTANA_RETO);
        return;
    }

    // Opcion no valida: liberar y volver a mostrar la misma ventana
    printf("Opcion no valida, intentalo de nuevo.\n");
    db_free_retos(retos);
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
void iniciarSesion(Ventana *v){

    char nombre[64];
    char password[128];

    while (1) {
        // Pedir credenciales
        printf("Nombre de usuario: ");
        scanf("%63s", nombre);
        while (getchar() != '\n');

        printf("Contrasena: ");
        scanf("%127s", password);
        while (getchar() != '\n');

        // Consultar la BD
        Usuario *usuario = db_get_usuario(nombre, password);

        if (usuario != NULL) {
            // Credenciales correctas
            v->usuario = usuario;  // guardas el usuario en la ventana
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

void registrar(Ventana *v){
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
            v->usuario->id     = id;
            v->usuario->total_puntos = puntos;
            strncpy(v->usuario->nombre, nombre, sizeof(v->usuario->nombre) - 1);

            navegar(v, VENTANA_PERFIL);
            return;
        }

        // Error en la BD (nombre o email duplicado, u otro error)
        printf("No se ha podido registrar el usuario, intentalo de nuevo.\n\n");
    }
}
