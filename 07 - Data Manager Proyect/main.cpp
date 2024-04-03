/*
    Title:      Proyecto Final EDD2 Q2 '22 - Data Manager/Gestor de Tablas con Índices Lineales
    Purpose:    Implementar archivos de Registros Binarios, de Longitud Fija, Indices, Avail List y Header
    Author:     Daniel Isaac Juárez Funes
    Date:       10 de julio de 2023
*/

#include "Funciones.h"
#include "nlohmann/json.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <iomanip>

using std::cout;
using std::endl;
using std::cerr;
using std::to_string;

using std::string;
using std::ostringstream;
using std::stringstream;
using std::ofstream;
using std::ifstream;
using std::vector;
using nlohmann::json;

/**
 * Formato para PUT & POST
 * ./build/data-manager -file ./data/friends.bin -PUT -pk=111 data={"id":835,"name":"jen","age":24,"sex":"f","city":"miami"}
 * ./build/data-manager -file ./data/friends.bin -POST -data={"id":835,"name":"jen","age":24,"sex":"f","city":"miami"}
*/

int main(int argc, char* argv[]) {

    if (argc <= 2) {
        cerr << "ERROR: Ingrese un Parámetro Válido" << endl;
    } else {
        string Operacion;
        if (argc == 3) {
            Operacion = argv[1];
            if (Operacion.compare("-create") == 0) { //* ---------- Crear Archivo ----------
                Funciones::CrearArchivo(argc, argv);
            }
        } else if (argc == 5) { 
            Operacion = argv[3];
            if (Operacion.compare("-load") == 0) { //* ---------- Cargar Datos ---------- 
                Funciones::CargarArchivo(argc, argv);
            } else if (Operacion.compare("-DELETE") == 0) { //* ---------- Borrar Registro ---------- 
                Funciones::BorrarRegistro (argc, argv);
            }
        } else if (argc == 4) { 
            Operacion = argv[3];
            if (Operacion.compare("-describe") == 0) { //* ---------- Describir ----------
                Funciones::Describir(argc,argv);
            } else if (Operacion.compare("-GET") == 0) { //* ---------- Listar ----------
                Funciones::Listar(argc,argv);
            } else if (Operacion.compare("-compact") == 0) { //* ---------- Compactar ----------
                Funciones::Compactar (argc, argv);
            } else if (Operacion.compare("-reindex") == 0) { //* ---------- Reindexar ----------
                Funciones::UpdateIndices (argc, argv);
                cout << "{\"result\":\"OK\", \"all-indices-processed\"}" << endl;
            }
        } else if (argc == 6) { 
            Operacion = argv[4];
            if (Operacion.compare("-pk") == 0) { //* ---------- Buscar PK ----------
                Funciones::BuscarPK(argc,argv);
            } else if (Operacion.substr(0,3).compare("-sk") == 0) { //* ---------- Buscar SK ----------
                Funciones::BuscarSK (argc, argv, Operacion);
            } 
        } else if (argc >= 6) {
            Operacion = argv[3];
            if (Operacion.compare("-PUT") == 0) { //* ---------- Modificar Registro ----------
                Funciones::ModificarRegistro (argc, argv);
            } else if (Operacion.compare("-POST") == 0) { //* ---------- Agregar Registro ---------- 
                Funciones::AgregarRegistro (argc, argv);
            }
        }
    }
    return 0;
    
}