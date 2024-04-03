/*
    Title:      Fixed Records Search || Convertir CSV y Buscar por RRN
    Purpose:    Trabajar con Archivos CSV y Busqueda de Registros de Longitud Fija
    Author:     Daniel Isaac Juárez
    Date:       Mayo 26 de 2023
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>

using std::vector;
using std::cout;
using std::endl;
using std::cerr;
using std::string;
using std::stringstream;
using std::ofstream;
using std::ifstream;

int main(int argc, char* argv[]){

    if (argc != 4) { // Validación Input cout errors
		switch (argc) {
		case 1:
			cerr << "ERROR -- Faltan 3 Parámetros.\nUse:\n" << argv[0] << " [./data/superstore.csv] [-convert] [length-file.txt]\n" << argv[0] << " [./data/superstore.fixed] [-retrieve] [12]" << endl;
			return 1;
			break;
		case 2:
			cerr << "ERROR -- Faltan 2 Parámetros.\nUse:\n" << argv[0] << " " << argv[1] << " [-convert] [length-file.txt]\n" << argv[0] << " " << argv[1] << " [-retrieve] [12]" << endl;
			return 1;
			break;
		case 3:
			cerr << "ERROR -- Falta 1 Parámetro.\nUse:\n" << argv[0] << " " << argv[1] << " " << argv[2] << " [length-file.txt]\n" << argv[0] << " " << argv[1] << " " << argv[2] << " [RRN to search]" << endl;
			return 1;
			break;
		case 4:
			cerr << "ERROR -- Ingresó Demasiados Parámetros.\nUse:\n" << argv[0] << "[./build/basic-search] [./data/superstore.csv] [-convert] [length-file.txt]\n[./build/basic-search] [./data/superstore.fixed] [-retrieve] [12]" << endl;
			return 1;
			break;			
		default:
			break;
		}
	} else {
		string Flag = argv[2];
		if (Flag.compare("-convert") == 0){ // -convert (Pasa de .csv a un Fixed Length File)
			string superstoreFilename = argv[1]; //Variables Superstore.csv
			ifstream superstoreFile;
			string superstoreData;
			vector<string> vectorpreInfo; //Variables FixedLengthFile
			ofstream superstoreFixed("./data/superstore.fixed");
			string writeThis;
			string toWrite;
			size_t charPosition;
			vector<string>caracteresReplace = {"†", "‡", "‰", "ì", "î", "ˆ", "È", "¸", "æ"};
			string lengthfileName = argv[3]; //Varaibles Length.txt
			ifstream lengthFile;
			string lengthfileData;
			vector<string> lengthfileInfo;

			lengthFile.open(lengthfileName);
			if (!lengthFile.fail()){ //Recuperando Información Fields' Length
				while (getline(lengthFile, lengthfileData, '\n')) {
					lengthfileInfo.push_back(lengthfileData.substr(14,3));
				}
			}
			lengthFile.close();

			superstoreFile.open(superstoreFilename);
			if (!superstoreFile.fail()){
				while (getline(superstoreFile, superstoreData,'\n')) { //Captura cada Record
					for (size_t i = 0; i < superstoreData.size(); i++) { //Obtiene la información de cada Field
						if (superstoreData.at(i) == ',' || i+1 == superstoreData.size()) {
							vectorpreInfo.push_back(writeThis);
							writeThis = "";
							continue;
						} else if (superstoreData.at(i) == '\"') {
							for (size_t j = i+1; j < superstoreData.size(); j++) {
								if (superstoreData.at(j) == '\"' && superstoreData.at(j+1) == ',') {
									vectorpreInfo.push_back(writeThis);
									writeThis = "";
									i=j+2;
									break;
								} else {
									writeThis += superstoreData.at(j);
								}
							}
						} else {
							writeThis += superstoreData.at(i); //Prelim Write
						}
					}
					for	(size_t g = 0; g < lengthfileInfo.size(); g++){ //Reemplaza caracteres
						toWrite = vectorpreInfo.at(g);
						for (size_t i = 0; i < caracteresReplace.size(); i++) {
							if (i < 2) {
								while ((charPosition = toWrite.find(caracteresReplace.at(i))) != string::npos) {
									toWrite.replace(charPosition, 3, "   ");
								}
							} else {
								while ((charPosition = toWrite.find(caracteresReplace.at(i))) != string::npos) {
									toWrite.replace(charPosition, 2, "   ");
								}
							}
						}
						toWrite.resize(stoi(lengthfileInfo.at(g)),' ');
						superstoreFixed << toWrite; //Write a Fixed
					}
					superstoreFixed << endl;
					vectorpreInfo.clear();
				}
			}
			superstoreFixed.close();
			superstoreFile.close();

		} else if (Flag.compare("-retrieve") == 0) { // -retrieve (Encuentra un record específico y lo despliega de forma ordenada)
			
			ifstream lengthFile;  //Varaibles Length.txt
			int byteCount = 1;
			vector<string> lengthfileInfo;
			string lengthfileData;
			ifstream fixedFile; //Out of Fixed and toPrint
			string fixedFilename = argv[1];
			int findingRecord = atoi(argv[3]);
			string data;
			vector<string> inandOut;
			int lastpos = 0;

			lengthFile.open("length-file.txt");
			if (!lengthFile.fail()){
				while (getline(lengthFile, lengthfileData, '\n')) { //Recupera información del archivo LengthFile
					byteCount += stoi(lengthfileData.substr(14,3));
					lengthfileInfo.push_back(lengthfileData.substr(0,17));
				}
			}
			lengthFile.close();

			fixedFile.open(fixedFilename);
			if (!fixedFile.fail()){
				fixedFile.seekg(((byteCount*findingRecord)), std::ios_base::beg); //Se situa al inicio del record indicado
				getline(fixedFile,data,'\n');
				for (size_t i = 0; i < lengthfileInfo.size(); i++) { //Muestra el record validando los valores decimales especificados
					if (i == 17 || i == 19 || i == 20) {
						cout << lengthfileInfo.at(i).substr(0,14);
						printf("%.2f",stof(data.substr(lastpos, stoi(lengthfileInfo.at(i).substr(14,3)))));
						cout << endl;
						lastpos += stoi(lengthfileInfo.at(i).substr(14,3));
					} else {
						cout << lengthfileInfo.at(i).substr(0,14) << data.substr(lastpos, stoi(lengthfileInfo.at(i).substr(14,3))) << endl;
						lastpos += stoi(lengthfileInfo.at(i).substr(14,3));
					}
				}
			}
			fixedFile.close();
		} else {
			cerr << "Ingrese una Bandera Válida." << endl;
		}
	}
    return 0;
}