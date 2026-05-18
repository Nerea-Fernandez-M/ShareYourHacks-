/*
 * log.cpp
 *
 *  Created on: 18 may 2026
 *      Author: nerea.garona
 */

#include "log.h"
#include <iostream>
#include <fstream>  // Necesario para gestionar ficheros en C++ (std::ofstream)
#include <iomanip>  // Necesario para formatear la fecha/hora
#include <ctime>

void guardar_log(const char *mensaje) {
    // std::ios::app abre el fichero en modo 'append' (añadir al final)
    std::ofstream fichero("servidor.log", std::ios::app);

    if (!fichero.is_open()) {
        return; // Si no se puede abrir, salimos de la función
    }

    // Obtener la hora actual del sistema
    std::time_t t = std::time(nullptr);
    std::tm *tm_info = std::localtime(&t);

    // Escribimos en el fichero usando los operadores de flujo << típicos de C++
    // Formato: [YYYY-MM-DD HH:MM:SS] Mensaje
    fichero << "["
            << std::put_time(tm_info, "%Y-%m-%d %H:%M:%S")
            << "] " << mensaje << std::endl;

    // El fichero se cierra automáticamente al salir de la función gracias al destructor de std::ofstream
}


