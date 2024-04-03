/*
    Title:      Proyecto Final EDD2 Q2 '22 - Data Manager/Gestor de Tablas con Índices Lineales
    Purpose:    Implementar archivos de Registros Binarios, de Longitud Fija, Indices, Avail List y Header
    Author:     Daniel Isaac Juárez Funes
    Date:       10 de julio de 2023
*/

#pragma once

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

class Funciones {
    private:
    public:
        Funciones();
        ~Funciones();
        //* Structs ---------------------------------------------------------------------------------------------------------------------------------------------------
        struct intindicePrimario {
            int id;
            long offset;
        };
        struct stringindicePrimario {
            string id;
            long offset;
        };
        //* Funciones Regulares ---------------------------------------------------------------------------------------------------------------------------------------
        /**
         * compareID para INTs
         * Compara IDs de tipo INT para ordenar Indices
        */
        static bool intcompareID(const intindicePrimario& a, const intindicePrimario& b);

        /**
         * compareID para STRINGs
         * Compara IDS de tipo STRING para ordenar Indices
        */
        static bool stringcompareID(const stringindicePrimario& a, const stringindicePrimario& b);

        /**
         * Construccion de Header
         * Extrae la información del JSON para incluirla y ensamblar el Header para el archivo binario
        */
        static string ConstruccionHeader (json &dataJSON, string &header, string &secKeys, string &jsonName, int &recordSize) ;

        /**
         * Deconstruccion de Header
         * Extrta el Header del Archivo Binario y lo alamcena en un vector de tipo STRINGs
        */
        static vector<string> DeconstruccionHeader (string &binName, string &header);

        /**
         * Check Campos de archivo CSV
         * Compara si la primera linea (descriptiva) del CSV coincide con la información extraida del JSON
        */
        static bool CheckCSVFields (ifstream &csvFile, vector<string> &vectorInfoHeader);

        /**
         * Asiganción Vectores de Campos
         * Distribuye la información en el Header acerca de los campos de los registros en sub-vectores para facilitar acceso a información y validaciones
        */
        static void AssignFieldVectors (vector<string> &vectorInfoHeader, vector<string> &fieldsName, vector<string> &fieldsCapacity, vector<string> &vectorDataType);

        /**
         * Visualizar Vectores de Campos
         * Imprime los sub-vectores con infromación de los campos de los registros
        */
        static void ViewFieldVectors (vector<string> &vectorInfoHeader, vector<string> &fieldsName, vector<string> &fieldsCapacity, vector<string> &vectorDataType);

        /**
         * CSV a JSON
         * Convierte cada registro extraido del CSV a un objeto de tipo JSON. Almacena los objetos de tipo JSON en un vector de JSONs
        */
        static void CSVtoJSON (ifstream &csvFile, vector<string> &vectorInfoHeader, vector<string> &fieldsInfo, vector<string> &fieldsName, vector<string> &fieldsCapacity, vector<string> &vectorDataType, vector<json> &registros);

        /**
         * Cuenta de Registros
         * Recorre un vector de JSONs para contabilizar cuantos registros fueron convertidos exitosamente a JSON
        */
        static void CuentaRegistros (vector<json> &registros);

        /**
         * Escritura de Header
         * Recupera el vector que almcena el Header y valida sus posiciones para escribir correctamente el encabezado para el archivo binario
        */
        static void EscribeHeader (ofstream &binFile, int &sizebson, string &binName, string &header, vector<string> &vectorInfoHeader, vector<json> &registros);

        /**
         * Creacion de Indices Primarios
         * Toma los registros convertidos exitosamente a JSON, extrae sus llaves primarias y crea el archivo de indices primarios
        */
        static void CrearIndicesPrimarios (ofstream &idxFile, string &idxName, vector<string> &vectorInfoHeader, vector<string> &fieldsName, vector<string> &vectorDataType, vector<json> &registros);

        /**
         * Creacion de Indices Secundarios
         * Toma los registros convertidos exitosamente a JSON, extrae sus llaves secundarias y crea el/los archivo(s) de indices secundarios
        */
        static void CrearIndicesSecundarios (string &fileName, vector<string> &vectorInfoHeader, vector<string> &fieldsName, vector<json> &registros);

        /**
         * Lectura de JSONs del Archivo Binario
         * Recupera todos los objetos BSON del archivo binario, los convierte a JSON y los agrega a un vector de JSONs
        */
        static vector<json> JSONsFromBin (string &header, string &binName, int &sizebson, vector<string> &fieldsName, vector<json> &vectorJSON);

        /**
         * Indices Primarios a Vectores
         * Carga los indices primarios de los archivos a memoria y los almacena en vectores dependiendo del tipo de la operacion
        */
        static void IDXsToVectors (string &keyType, ifstream &ArchivoIDX, vector<intindicePrimario> &vectorintIndices, vector<stringindicePrimario> &vectorstringIndices);

        /**
         * Impresión de JSON
         * Imprime u nobjeto JSON utlizando de apoyo un vector con los nombres de sus correspondientes campos
        */
        static void coutJSON (json &j_from_bson, vector<string> &fieldsName);

        /**
         * Busqueda de Indice Primario para INTs
         * Realiza la busqueda del indice primario dado y devuelve el objeto JSON que haga match
        */
        static json intPKSearch (vector<intindicePrimario> &vectorintIndices, long &offsetRecuperado, string &toSearch, string &fileName, vector<string> &fieldsName, vector<string> &vectorInfoHeader);

        /**
         * Busqueda de Indice Primario para STRINGs
         * Realiza la busqueda del indice primario dado y devuelve el objeto JSON que haga match
        */
        static json stringPKSearch (vector<stringindicePrimario> &vectorstringIndices, long &offsetRecuperado, string &toSearch, string &fileName, vector<string> &fieldsName, vector<string> &vectorInfoHeader);

        /**
         * Indices Secundarios a Vectores
         * Carga los indices secundarios de los archivos a memoria y los almacena en vectores dependiendo del tipo de la operacion
        */
        static void SDXsToVectors (string &testsecKey, ifstream &ArchivoIDX, vector<intindicePrimario> &vectorintIndices, vector<stringindicePrimario> &vectorstringIndices);

        /**
         * Busqueda de Indices Secundarios para INTs
         * Realiza la busqueda del indice secundario dado y devuelve el/los objeto(s) JSON que haga(n) match
        */
        static void intSKSearch (vector<intindicePrimario> &vectorintIndices, long &offsetRecuperado, string &toSearch, string &fileName, vector<string> &fieldsName, vector<string> &vectorInfoHeader);

        /**
         * Busqueda de Indices Secundarios para STRINGSs
         * Realiza la busqueda del indice secundario dado y devuelve el/los objeto(s) JSON que haga(n) match
        */
        static void stringSKSearch (vector<stringindicePrimario> &vectorstringIndices, long &offsetRecuperado, string &toSearch, string &fileName, vector<string> &fieldsName, vector<string> &vectorInfoHeader);

        /**
         * Recuperación de Datos de Linea de Comandos
         * Une los datos pasados por parametro por medio de la linea de comandos a un string y actualiza los vectores que contienen dicha información
        */
        static void CMDWork (int argc, char* argv[], string &jsonInfo, string &binName, string &NewJSONInfo, string &bycommas, string &bycolon, vector<string> &AllData, vector<string> &Field, vector<string> &SubData);

        /**
         * Actualizar Indices
         * Actualiza los archivos de Indices tanto primarios como secundarios para los registros en el archivo binario
        */
        static void UpdateIndices (int argc, char* argv[]);

        /**
         * Actualizar Vectores y Header
         * Actualiza los datos en los vectores que se estpan utilizando, prepara el header para ser escrito al archivo binario tras operaciones que impliquen el Avail List
        */
        static void HeaderandVectorsSetUp (ifstream &ArchivoIDX, string &binName, string &header, string &keyType, string &idxFile, long &AvailListPos, vector<string> &vectorInfoHeader, vector<string> &fieldsName, vector<string> &fieldsCapacity, vector<string> &vectorDataType, vector<intindicePrimario> &vectorintIndices, vector<stringindicePrimario> &vectorstringIndices);

        /**
         * Re-escritura de Header
         * Recupera el Header actualizado y lo escribe al binario, reemplazando el anterior
        */
        static void ReWriteHeader (std::fstream &binFile, string &binName, long &offsetRecuperado, vector<string> &vectorInfoHeader);

        /**
         * Marca JSONs
         * Se encarga de marcar los archivos JSON en el archivo binario para mantener el enlace del Avail List
        */
        static void TallyJSON (std::fstream &binFile, long &AvailListPos, long &offsetRecuperado, json &FoundIt, string &binName, vector<string> &fieldsName, vector<string> &fieldsCapacity);

        //* Funciones para cada Operación -----------------------------------------------------------------------------------------------------------------------------
        static void CrearArchivo (int argc, char* argv[]);
        static void CargarArchivo (int argc, char* argv[]);
        static void Describir (int argc, char* argv[]);
        static void Listar (int argc, char* argv[]);
        static void BuscarPK (int argc, char* argv[]);
        static void BuscarSK (int argc, char* argv[], string &Operacion);
        static void ModificarRegistro (int argc, char* argv[]);
        static void BorrarRegistro (int argc, char* argv[]);
        static void AgregarRegistro (int argc, char* argv[]);
        static void Compactar (int argc, char* argv[]);
};