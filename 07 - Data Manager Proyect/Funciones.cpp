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

bool Funciones::intcompareID(const intindicePrimario& a, const intindicePrimario& b) {
    return a.id < b.id;
}

bool Funciones::stringcompareID(const stringindicePrimario& a, const stringindicePrimario& b) {
    return a.id < b.id;
}

string Funciones::ConstruccionHeader (json &dataJSON, string &header, string &secKeys, string &jsonName, int &recordSize) {
    for (size_t i = 0; i < dataJSON["secondary-key"].size(); i++) { //string de llaves secundarias para print
        string tempKey = dataJSON["secondary-key"].at(i);
        if (i == dataJSON["secondary-key"].size()-1) {
            secKeys += "\"" + jsonName + "-" + (tempKey).substr(0,tempKey.size()) + ".sdx\"";
            ofstream sdxFile(jsonName + "-" + (tempKey).substr(0,tempKey.size()) + ".sdx");
            sdxFile.close();
        } else {
            secKeys += "\"" + jsonName + "-" + (tempKey).substr(0,tempKey.size()) + ".sdx\", ";
            ofstream sdxFile(jsonName + "-" + (tempKey).substr(0,tempKey.size()) + ".sdx");
            sdxFile.close();
        }   
    }

    header.append(to_string(dataJSON["fields"].size()) + ',');
    vector<int> posSK;

    for (size_t i = 0; i < dataJSON["fields"].size(); i++){
        json element = dataJSON["fields"].at(i);
        string name = to_string(element["name"]).substr(1,to_string(element["name"]).size()-2);
        string type = to_string(element["type"]).substr(1,to_string(element["type"]).size()-2);
        string length = to_string(element["length"]);

        for (size_t j = 0; j < dataJSON["secondary-key"].size(); j++){ //Obtiene posicion de SKs en Fields
            string nameCompare = dataJSON["secondary-key"].at(j);
            if (name.compare(nameCompare) == 0){
                posSK.push_back(i);
            }
        }

        header.append(name + '/' + type + '/' + length + ',');
        recordSize += element["length"].template get<int>();
    }
    header.append(to_string(recordSize) + ',');
    
    header.append("0," + to_string(dataJSON["primary-key"]).substr(1, to_string(dataJSON["primary-key"]).size()-2) + ',');
    header.append(to_string(dataJSON["secondary-key"].size()) + ',');
    for (size_t i = 0; i < dataJSON["secondary-key"].size(); i++) { 
        string tempKey = dataJSON["secondary-key"].at(i);
        if (i == dataJSON["secondary-key"].size()-1) {
            header.append(to_string(posSK.at(i)) + "," + tempKey.substr(0, tempKey.size()) + '\n');
        } else {
            header.append(to_string(posSK.at(i)) + "," + tempKey.substr(0, tempKey.size()) + ',');
        }   
    }
    header.insert(0, to_string(header.length()) + ',');

    return header;

}

vector<string> Funciones::DeconstruccionHeader (string &binName, string &header) {
    ifstream binFile(binName, std::ios::binary); vector<string> vectorInfoHeader; string bycommas, byslash;

    getline(binFile, header);
    stringstream headerData(header);

    while (getline(headerData, bycommas, ',')) {
        std::stringstream insidecommas(bycommas);
        std::string byslash;
        while (std::getline(insidecommas, byslash, '/')) {
            vectorInfoHeader.push_back(byslash);
        }
    } 
    binFile.close();

    // for (size_t i = 0; i < vectorInfoHeader.size(); ++i) {
    //     cout << vectorInfoHeader[i] << endl;
    // }

    return vectorInfoHeader;
}

bool Funciones::CheckCSVFields (ifstream &csvFile, vector<string> &vectorInfoHeader) {
    string firstRow;
    vector<string> typesOnCSV;
    if (getline(csvFile, firstRow)) {
        stringstream CSVTypes(firstRow);
        string typestemp;
        while (getline(CSVTypes,typestemp,',')) {
            typesOnCSV.push_back(typestemp);
        }
        int saltos = 2;
        for (int i = 0; i < stoi(vectorInfoHeader[1]); i++) {
            if (vectorInfoHeader[saltos].compare(typesOnCSV[i]) == 0) {
            } else {
                return false;
            }
            saltos += 3;
        }
    }
    return true;
}

void Funciones::AssignFieldVectors (vector<string> &vectorInfoHeader, vector<string> &fieldsName, vector<string> &fieldsCapacity, vector<string> &vectorDataType) {
    string csvFieldName, csvFieldCapacity;
    for (int i = 0; i < stoi(vectorInfoHeader[1]); i++) { 
        csvFieldName = vectorInfoHeader[2+(3*i)];
        fieldsName.push_back(csvFieldName);
        csvFieldCapacity = vectorInfoHeader[4+(3*i)];
        fieldsCapacity.push_back(csvFieldCapacity);
        vectorDataType.push_back(vectorInfoHeader[3+(3*i)]);
    }
}

void Funciones::ViewFieldVectors (vector<string> &vectorInfoHeader, vector<string> &fieldsName, vector<string> &fieldsCapacity, vector<string> &vectorDataType) {
    cout << "fieldsName, DataType & fieldCapacity" << endl;
    for (size_t i = 0; i < fieldsName.size(); i++) { 
        cout << i << ") " << fieldsName[i] << " (" << vectorDataType[i] << " - " << fieldsCapacity[i] << ")" << endl;
    }
}

void Funciones::CSVtoJSON (ifstream &csvFile, vector<string> &vectorInfoHeader, vector<string> &fieldsInfo, vector<string> &fieldsName, vector<string> &fieldsCapacity, vector<string> &vectorDataType, vector<json> &registros) {
    string csvLine, csvLineCampo, csvFieldName, csvFieldCapacity; bool seAgrega;
    

    while (!csvFile.eof()){
        seAgrega = true;
        getline(csvFile, csvLine);
        // cout << "Linea: " << csvLine << endl;
        stringstream ss_csvLine(csvLine);
        while (getline(ss_csvLine, csvLineCampo, ',')) {
            fieldsInfo.push_back(csvLineCampo);
        }
        // cout << "fieldsInfo" << endl;
        for (size_t i = 0; i < fieldsInfo.size(); i++) { 
            // cout << i << ") : " << fieldsInfo[i] << endl;
            if (fieldsInfo[0].compare("") == 0) {
                seAgrega = false;
            } else {

            }
        }

        AssignFieldVectors(vectorInfoHeader, fieldsName, fieldsCapacity, vectorDataType);

        json jsonObject;
        if (!seAgrega) {
            
        } else {   
            
            for (int i = 0; i < stoi(vectorInfoHeader[1]); i++) {
                fieldsInfo[i].resize(stoi(fieldsCapacity[i]),'\0');
                jsonObject[fieldsName[i]] = fieldsInfo[i];
                /*
                jsonObject["primary-key"] = primarykeyValue;
                string seckeys;
                for (int i = 0; i < stoi(vectorInfoHeader[stoi(vectorInfoHeader[1])*3+5]); ++i) {
                    seckeys.append("LS" + to_string(i+1));
                    jsonObject["secondary-key"] = seckeys;
                }
                jsonObject["secondary-key"] = seckeys;
                */
            }
        }
        registros.push_back(jsonObject);
        fieldsInfo.clear();
        fieldsName.clear();
    }
    csvFile.close();
}

void Funciones::CuentaRegistros (vector<json> &registros) {
    int CantidadRegistrosOriginales = registros.size();
    
    for (size_t i = 0; i < registros.size(); i++) {
        if (registros[i].is_null()) {
            registros.erase(registros.begin() + i);
        } else {
            
        }
    }

    if (CantidadRegistrosOriginales != (int)registros.size()) {
        int skipped = CantidadRegistrosOriginales - registros.size();
        cout << "{\"result\": \"WARNING\", \"records\": " << CantidadRegistrosOriginales << ", \"skipped\": " << skipped << "}" << endl;
    } else {
        cout << "{\"result\": \"OK\", \"records\": \""<< registros.size() << "\"}" << endl;
    }
}

void Funciones::EscribeHeader (ofstream &binFile, int &sizebson, string &binName, string &header, vector<string> &vectorInfoHeader, vector<json> &registros) {
    binFile.open(binName, std::ios::trunc);
    int sizeLess = vectorInfoHeader.size()-1;
    if (sizeLess != (stoi(vectorInfoHeader[1])*3)+5 + stoi(vectorInfoHeader[(stoi(vectorInfoHeader[1])*3)+5])*2) {
        if (stoi(vectorInfoHeader[((stoi(vectorInfoHeader[1])*3)+5)+1+stoi(vectorInfoHeader[((stoi(vectorInfoHeader[1])*3)+5)])*2]) == sizebson) {
            if (vectorInfoHeader[stoi(vectorInfoHeader[1])*3+5 + stoi(vectorInfoHeader[stoi(vectorInfoHeader[1])*3+5])*2 + 2] == "****-1") {
                header.append("\n");
                binFile.write(header.c_str(), header.length());
                for (size_t i = 0; i < registros.size(); i++) { 
                    json jsonWrite = registros.at(i);
                    vector<char> buffer;
                    json::to_bson(jsonWrite,buffer);
                    binFile.write(buffer.data(), buffer.size());
                }
            } else {
                header.append(",****-1\n");
                binFile.write(header.c_str(), header.length());
                for (size_t i = 0; i < registros.size(); i++) { 
                    json jsonWrite = registros.at(i);
                    vector<char> buffer;
                    json::to_bson(jsonWrite,buffer);
                    binFile.write(buffer.data(), buffer.size());
                }
            }
        }
    } else {
        header.append("," + to_string(sizebson) + ",****-1\n");
        binFile.write(header.c_str(), header.length());
        for (size_t i = 0; i < registros.size(); i++) { 
            json jsonWrite = registros.at(i);
            //cout << jsonWrite.dump(4) << endl;
            vector<char> buffer;
            json::to_bson(jsonWrite,buffer);
            binFile.write(buffer.data(), buffer.size());
        }
    }
    binFile.close();
}

void Funciones::CrearIndicesPrimarios (ofstream &idxFile, string &idxName, vector<string> &vectorInfoHeader, vector<string> &fieldsName, vector<string> &vectorDataType, vector<json> &registros) {
    
    idxFile.open(idxName,std::ios::trunc);                
    string keyType = vectorDataType[stoi(vectorInfoHeader[(stoi(vectorInfoHeader[1])*3+3)])]; //* Pos en vector para PK

    if (keyType.compare("int") == 0 || keyType.compare("float") == 0) { //* Diferencia para Llave Númerica
        vector<intindicePrimario> indicesPrimarios;

        //* Recolecta Indices y Offsets
        for (size_t i = 0; i < registros.size(); i++) { 
            json jsonWrite = registros.at(i);
            //cout << jsonWrite["name"] << endl;
            string index = jsonWrite[fieldsName[stoi(vectorInfoHeader[(stoi(vectorInfoHeader[1])*3+3)])]];
            long offset = stol(vectorInfoHeader[((stoi(vectorInfoHeader[1])*3)+5)+1+stoi(vectorInfoHeader[((stoi(vectorInfoHeader[1])*3)+5)])*2])*i;
            indicesPrimarios.push_back({stoi(index),offset});    
        }
        
        //* Ordena
        sort(indicesPrimarios.begin(),indicesPrimarios.end(), intcompareID);

        //* Escribe tamaño de los Índices
        intindicePrimario toGetSize;
        toGetSize.id = indicesPrimarios[0].id;
        toGetSize.offset = indicesPrimarios[0].offset;
        string idxSize = (to_string(sizeof(toGetSize))+'\n');
        idxFile.write(idxSize.c_str(),idxSize.size());

        //* Escribe al Archivo de Indices Primarios
        for (size_t i = 0; i < indicesPrimarios.size(); ++i) {
            intindicePrimario intoBinary;
            intoBinary.id = indicesPrimarios[i].id;
            intoBinary.offset = indicesPrimarios[i].offset;
            idxFile.write(reinterpret_cast<char*>(&intoBinary), sizeof(intoBinary));
        }
        
    } else { //* Diferencia para Llave de Caracteres
        vector<stringindicePrimario> indicesPrimarios;

        //* Recolecta Indices y Offsets
        for (size_t i = 0; i < registros.size(); i++) { 
            json jsonWrite = registros.at(i);
            string index = jsonWrite[fieldsName[stoi(vectorInfoHeader[(stoi(vectorInfoHeader[1])*3+3)])]];
            long offset = stol(vectorInfoHeader[((stoi(vectorInfoHeader[1])*3)+5)+1+stoi(vectorInfoHeader[((stoi(vectorInfoHeader[1])*3)+5)])*2])*i;
            indicesPrimarios.push_back({index,offset});
        }

        //* Ordena
        sort(indicesPrimarios.begin(),indicesPrimarios.end(), stringcompareID);

        //* Toma Objeto de Indices, Convierte a String para Escribir
        string alBin = indicesPrimarios[0].id + "," +to_string(indicesPrimarios[0].offset) + "\n";

        //* Escribe al Archivo de Indices Primarios
        for (size_t i = 0; i < indicesPrimarios.size(); ++i) {
            alBin = "";
            stringindicePrimario intoBinary = indicesPrimarios[i];
            cout << i << ") " << intoBinary.id << "," << intoBinary.offset << endl;
            alBin = "/" + intoBinary.id + "," +to_string(intoBinary.offset) + "\n";
            idxFile.write(alBin.c_str(),sizeof(alBin));
        }
    }

    idxFile.close();
}

void Funciones::CrearIndicesSecundarios (string &fileName, vector<string> &vectorInfoHeader, vector<string> &fieldsName, vector<json> &registros) {

    int reachSKCount = 1;
    for (int i = 0; i < stoi(vectorInfoHeader[stoi(vectorInfoHeader[1])*3+5]); i++) { //* Ciclo a cantidad de SK's
        string testsecKey = fieldsName[stoi(vectorInfoHeader[((stoi(vectorInfoHeader[1])*3)+5)+reachSKCount])];
        string sdxName = fileName + "-" + testsecKey + ".sdx";
        ofstream sdxFile(sdxName,std::ios::trunc);

        if (testsecKey.compare("int") == 0 || testsecKey.compare("float") == 0) { //* Diferencia para Llave Númerica
            vector<intindicePrimario> indicesPrimarios;

            //* Recolecta Indices y Offsets
            for (size_t i = 0; i < registros.size(); i++) { 
                json jsonWrite = registros.at(i);
                string index = jsonWrite[testsecKey];
                long offset = stol(vectorInfoHeader[((stoi(vectorInfoHeader[1])*3)+5)+1+stoi(vectorInfoHeader[((stoi(vectorInfoHeader[1])*3)+5)])*2])*i;
                indicesPrimarios.push_back({stoi(index),offset});    
            }
            
            //* Ordena
            sort(indicesPrimarios.begin(),indicesPrimarios.end(), intcompareID);

             //* Escribe tamaño de los Índices
            intindicePrimario toGetSize;
            toGetSize.id = indicesPrimarios[0].id;
            toGetSize.offset = indicesPrimarios[0].offset;
            string idxSize = (to_string(sizeof(toGetSize))+'\n');
            sdxFile.write(idxSize.c_str(),idxSize.size());

            //* Escribe al Archivo de Indices Primarios
            for (size_t i = 0; i < indicesPrimarios.size(); ++i) {
                intindicePrimario intoBinary;
                intoBinary.id = indicesPrimarios[i].id;
                intoBinary.offset = indicesPrimarios[i].offset;
                sdxFile.write(reinterpret_cast<char*>(&intoBinary), sizeof(intoBinary));
            }
            
        } else { //* Diferencia para Llave de Caracteres
            vector<stringindicePrimario> indicesPrimarios;

            //* Recolecta Indices y Offsets
            for (size_t i = 0; i < registros.size(); i++) { 
                json jsonWrite = registros.at(i);
                string index = jsonWrite[testsecKey];
                long offset = stol(vectorInfoHeader[((stoi(vectorInfoHeader[1])*3)+5)+1+stoi(vectorInfoHeader[((stoi(vectorInfoHeader[1])*3)+5)])*2])*i;
                indicesPrimarios.push_back({index,offset});
            }
            
            //* Ordena
            sort(indicesPrimarios.begin(),indicesPrimarios.end(), stringcompareID);

            //* Toma Objeto de Indices, Convierte a String para Escribir y Escribe al Archivo de Indices Secundarios
            for (size_t i = 0; i < indicesPrimarios.size(); ++i) {
                stringindicePrimario intoBinary = indicesPrimarios[i];
                string alBin;
                if (intoBinary.offset == 0){
                    alBin = "/" + intoBinary.id + ",00" + to_string(intoBinary.offset) + "/\n\0";
                } else {
                    alBin = "/" + intoBinary.id + ",00" + to_string(intoBinary.offset) + "/\n\0";
                }
                sdxFile.write(alBin.c_str(),sizeof(alBin));   
            }
        }
        sdxFile.close();
        reachSKCount = reachSKCount+2;
    }
}

vector<json> Funciones::JSONsFromBin (string &header, string &binName, int &sizebson, vector<string> &fieldsName, vector<json> &vectorJSON) {
    ifstream readfrom(binName, std::ios::binary);
    readfrom.seekg(header.size()+1, std::ios::beg);
    while (readfrom.peek() != EOF){
        vector<char> buffer(sizebson);
        readfrom.read(buffer.data(), buffer.size());
        json j_from_bson = json::from_bson(buffer);

        //cout << "JSONWorking[fieldsName[0]]: " << j_from_bson[fieldsName[0]] << endl;
        string checking = j_from_bson[fieldsName[0]];
        if (checking.at(0) == '*') {
            
        } else {
            vectorJSON.push_back(j_from_bson);
        }
    }
    readfrom.close();
    return vectorJSON;
}

void Funciones::IDXsToVectors (string &keyType, ifstream &ArchivoIDX, vector<intindicePrimario> &vectorintIndices, vector<stringindicePrimario> &vectorstringIndices) {
    if (keyType.compare("int") == 0 || keyType.compare("float") == 0) {

        string sizeIDX;
        getline(ArchivoIDX,sizeIDX);
        intindicePrimario obj;
        while (ArchivoIDX.read(reinterpret_cast<char*>(&obj), sizeof(intindicePrimario))) {
            vectorintIndices.push_back(obj);
        }
        // for (size_t i = 0; i < vectorintIndices.size(); ++i) {
        //     cout << "IDs: " << to_string(vectorintIndices[i].id) << endl;
        // }
    } else {
        vector<string> lineas;
        string linea;

        while (ArchivoIDX.peek() != EOF) {
            getline(ArchivoIDX,linea);
            string newline = linea.substr(linea.find('/',0)+1,linea.size());
            lineas.push_back(newline);
        }
        for (size_t i = 0; i < lineas.size()-1; ++i) {
            stringindicePrimario obj;
            stringstream workline(lineas[i]);
            string porcomas;
            for (int i = 0; i<2; ++i){
                getline(workline, porcomas, ',');
                if (i == 0) {
                    obj.id = porcomas;
                } else if (i == 1) {
                    obj.offset = stol(porcomas);
                }
            }
            vectorstringIndices.push_back(obj);
        }
        // for (size_t i = 0; i < vectorstringIndices.size(); ++i) {
        //     cout << "IDs: " << vectorstringIndices[i].id << endl;
        // }
    }

    ArchivoIDX.close();
}

void Funciones::coutJSON (json &j_from_bson, vector<string> &fieldsName) {
    json JSONWorking = j_from_bson;
    cout << "{" << endl;
    for (size_t j = 0; j < fieldsName.size(); ++j) {
        bool esInt = false;
        string hold = JSONWorking[fieldsName[j]];
        if (hold.size() == 4) {
            esInt = true;
        }
        hold.erase(remove(hold.begin(), hold.end(), '\0'), hold.end());
        JSONWorking[fieldsName[j]] = hold;
        if (!esInt) {
            if ( j != fieldsName.size()-1){
                cout << "   \"" << fieldsName[j] << "\": " << JSONWorking[fieldsName[j]] << "," << endl;
            } else {
                cout << "   \"" << fieldsName[j] << "\": " << JSONWorking[fieldsName[j]] << endl;
            }
        } else {
            if ( j != fieldsName.size()-1){
                cout << "   \"" << fieldsName[j] << "\": " << hold << "," << endl;
            } else {
                cout << "   \"" << fieldsName[j] << "\": " << hold << endl;
            }
        }
    }
    cout << "}" << endl;
}

json Funciones::intPKSearch (vector<intindicePrimario> &vectorintIndices, long &offsetRecuperado, string &toSearch, string &fileName, vector<string> &fieldsName, vector<string> &vectorInfoHeader) {
    bool retrieved = false;
    for (size_t i = 0; i < vectorintIndices.size(); ++i) {
        // cout << "id: " << to_string(vectorintIndices[i].id) << "tosearch: " << toSearch << endl;
        if (to_string(vectorintIndices[i].id).compare(toSearch) == 0) {
            //cout << "ID: " << to_string(vectorintIndices[i].id) << ", Offset: " << to_string(vectorintIndices[i].offset) << endl;
            offsetRecuperado = vectorintIndices[i].offset;
            //cout << "offsetRecuperado: " << offsetRecuperado << endl;
            retrieved = true;
        }
    }

    if (retrieved){
        ifstream ArchivoBinario(fileName.substr(0, fileName.size()-4) + ".bin", std::ios::binary);
        ArchivoBinario.seekg(0,std::ios::beg); 
        //cout << "\nOpenpos: " << ArchivoBinario.tellg() << endl;
        string lineaheader;
        getline(ArchivoBinario, lineaheader);
        // cout << "Postpos: " << ArchivoBinario.tellg() << endl;

        std::streampos sizebson = stoi(vectorInfoHeader[vectorInfoHeader.size()-2]);
        // cout << "sizeBSON: " << sizebson << endl;
        std::streampos currentPosition = ArchivoBinario.tellg(); 
        // cout << "Currentpos: " << ArchivoBinario.tellg() << endl;
        std::streampos newPosition = currentPosition + offsetRecuperado;

        ArchivoBinario.seekg(newPosition); 
        // cout << "Newpos: " << ArchivoBinario.tellg() << endl;

        
        vector<char> buffer(sizebson);
        ArchivoBinario.read(buffer.data(), buffer.size());
        // for (char c : buffer) {
        //     std::cout << c;
        // }
        json j_from_bson = json::from_bson(buffer);

        ArchivoBinario.close();

        return j_from_bson;
    } else {
        return NULL;
    }
    
}

json Funciones::stringPKSearch (vector<stringindicePrimario> &vectorstringIndices, long &offsetRecuperado, string &toSearch, string &fileName, vector<string> &fieldsName, vector<string> &vectorInfoHeader) {
    bool retrieved = false;
    for (size_t i = 0; i < vectorstringIndices.size(); ++i) {
        string comparethis = vectorstringIndices[i].id;
        if (comparethis.find(toSearch) == 0) {
            //cout << "ID: " << vectorstringIndices[i].id << ", Offset: " << to_string(vectorstringIndices[i].offset) << endl;
            offsetRecuperado = vectorstringIndices[i].offset;
            retrieved = true;
        }
    }
    if (retrieved){
        ifstream ArchivoBinario(fileName.substr(0, fileName.size()-4) + ".bin", std::ios::binary);
        ArchivoBinario.seekg(0,std::ios::beg); //cout << "pos: " << ArchivoBinario.tellg() << endl;
        string lineaheader;
        getline(ArchivoBinario, lineaheader); //cout << "pos: " << ArchivoBinario.tellg() << endl;

        std::streampos sizebson = stoi(vectorInfoHeader[vectorInfoHeader.size()-2]);
        std::streampos currentPosition = ArchivoBinario.tellg();
        std::streampos newPosition = currentPosition + offsetRecuperado;

        ArchivoBinario.seekg(newPosition); //cout << "pos: " << ArchivoBinario.tellg() << endl;

        vector<char> buffer(sizebson);
        ArchivoBinario.read(buffer.data(), buffer.size());
        json j_from_bson = json::from_bson(buffer);

        ArchivoBinario.close();

        return j_from_bson;
    } else {
        return NULL;
    }
}

void Funciones::SDXsToVectors (string &testsecKey, ifstream &ArchivoIDX, vector<intindicePrimario> &vectorintIndices, vector<stringindicePrimario> &vectorstringIndices) {
    if (testsecKey.compare("int") == 0 || testsecKey.compare("float") == 0) {
        //cout << "lee de ints" << endl;
        
        string sizeIDX;
        getline(ArchivoIDX,sizeIDX);

        intindicePrimario obj;
        
        while (ArchivoIDX.read(reinterpret_cast<char*>(&obj), sizeof(intindicePrimario))) {
            vectorintIndices.push_back(obj);
        }

        for (size_t i = 0; i < vectorintIndices.size(); ++i) {
            //cout << "IDs: " << to_string(vectorintIndices[i].id) << endl;
        }

    } else {
        //cout << "lee de char" << endl;

        vector<string> lineas;
        string linea;

        while (ArchivoIDX.peek() != EOF) {
            getline(ArchivoIDX,linea);
            string newline = linea.substr(linea.find('/',0)+1,linea.size());
            if (newline.size() > 2) {
                //cout << "nueva linea: " <<  to_string(newline.size()) << ": "<< newline << endl;
                lineas.push_back(newline);
            } else {
                cout << "";
            }
        }

        for (size_t i = 0; i < lineas.size()-1; ++i) {
            stringindicePrimario obj;
            stringstream workline(lineas[i]);
            string porcomas;
            for (int i = 0; i<2; ++i){
                getline(workline, porcomas, ',');
                if (i == 0) {
                    //cout << "id: " << porcomas << endl;
                    obj.id = porcomas;
                } else if (i == 1) {
                    //cout << "offset: " << porcomas << endl;
                    obj.offset = stol(porcomas);
                }
            }
            vectorstringIndices.push_back(obj);
        }

        for (size_t i = 0; i < vectorstringIndices.size(); ++i) {
            //cout << "IDs: " << vectorstringIndices[i].id << endl;
        }
    }

    ArchivoIDX.close();
}

void Funciones::intSKSearch (vector<intindicePrimario> &vectorintIndices, long &offsetRecuperado, string &toSearch, string &fileName, vector<string> &fieldsName, vector<string> &vectorInfoHeader) {
    vector<intindicePrimario> found;
    for (size_t i = 0; i < vectorintIndices.size(); ++i) {
        if (to_string(vectorintIndices[i].id).compare(toSearch) == 0) {
            //cout << "ID: " << to_string(vectorintIndices[i].id) << ", Offset: " << to_string(vectorintIndices[i].offset) << endl;
            found.push_back(vectorintIndices[i]);
        }
    }
    if (found.size() == 0) {
        cout << "[]" << endl;
    } else {
        cout << "[\n" << endl;
        for ( size_t i = 0; i < found.size(); ++i) {
            offsetRecuperado = found[i].offset;
            ifstream ArchivoBinario(fileName.substr(0, fileName.size()-4) + ".bin", std::ios::binary);

            ArchivoBinario.seekg(0,std::ios::beg); //cout << "pos: " << ArchivoBinario.tellg() << endl;
            string lineaheader;
            getline(ArchivoBinario, lineaheader); //cout << "pos: " << ArchivoBinario.tellg() << endl;

            std::streampos sizebson = stoi(vectorInfoHeader[vectorInfoHeader.size()-2]);
            std::streampos currentPosition = ArchivoBinario.tellg();
            std::streampos newPosition = currentPosition + offsetRecuperado;

            ArchivoBinario.seekg(newPosition); //cout << "pos: " << ArchivoBinario.tellg() << endl;

            vector<char> buffer(sizebson);
            ArchivoBinario.read(buffer.data(), buffer.size());
            json j_from_bson = json::from_bson(buffer);

            vector<string> fieldsCapacity, fieldsName;
            string csvFieldCapacity, csvFieldName;
            for (int i = 0; i < stoi(vectorInfoHeader[1]); i++) {
                csvFieldName = vectorInfoHeader[2+(3*i)];
                fieldsName.push_back(csvFieldName); 
            }
            coutJSON(j_from_bson, fieldsName);
            ArchivoBinario.close();
        }
        cout << "]" << endl;
    }
    
}

void Funciones::stringSKSearch (vector<stringindicePrimario> &vectorstringIndices, long &offsetRecuperado, string &toSearch, string &fileName, vector<string> &fieldsName, vector<string> &vectorInfoHeader) {
    vector<stringindicePrimario> found;
    for (size_t i = 0; i < vectorstringIndices.size(); ++i) {
        string comparethis = vectorstringIndices[i].id;
        if (comparethis.find(toSearch) == 0) {
            found.push_back(vectorstringIndices[i]);
        }
    }
    if (found.size() == 0) {
        cout << "[]" << endl;
    } else {
        cout << "[" << endl;
        for (size_t i = 0; i < found.size(); ++i) {
            string comparethis = found[i].id;
            if (comparethis.find(toSearch) == 0) {
                //cout << "ID: " << vectorstringIndices[i].id << ", Offset: " << to_string(vectorstringIndices[i].offset) << endl;
                offsetRecuperado = found[i].offset;
                ifstream ArchivoBinario(fileName.substr(0, fileName.size()-4) + ".bin", std::ios::binary);

                ArchivoBinario.seekg(0,std::ios::beg); //cout << "pos: " << ArchivoBinario.tellg() << endl;
                string lineaheader;
                getline(ArchivoBinario, lineaheader); //cout << "pos: " << ArchivoBinario.tellg() << endl;

                std::streampos sizebson = stoi(vectorInfoHeader[vectorInfoHeader.size()-2]);
                std::streampos currentPosition = ArchivoBinario.tellg();
                std::streampos newPosition = currentPosition + offsetRecuperado;

                ArchivoBinario.seekg(newPosition); //cout << "pos: " << ArchivoBinario.tellg() << endl;

                vector<char> buffer(sizebson);
                ArchivoBinario.read(buffer.data(), buffer.size());
                json j_from_bson = json::from_bson(buffer);

                vector<string> fieldsCapacity, fieldsName;
                string csvFieldCapacity, csvFieldName;
                for (int i = 0; i < stoi(vectorInfoHeader[1]); i++) {
                    csvFieldName = vectorInfoHeader[2+(3*i)];
                    fieldsName.push_back(csvFieldName); 
                }
                coutJSON(j_from_bson, fieldsName);
                ArchivoBinario.close();
            }
        }
        cout << "]" << endl;
    }
}

void Funciones::CMDWork (int argc, char* argv[], string &jsonInfo, string &binName, string &NewJSONInfo, string &bycommas, string &bycolon, vector<string> &AllData, vector<string> &Field, vector<string> &SubData) {
    //* Recupera Info del CMD


    if (argc >= 6) {
        for (int i = 0; i < argc; ++i) {
            if (i >= 4) {
                jsonInfo.append(argv[i]);
            }
        }
    }
    
    binName = argv[2];

    bool hayEquals = (jsonInfo.find('=') != string::npos);

    if (!hayEquals) {
        jsonInfo = jsonInfo.substr(7,jsonInfo.size()-1);
        NewJSONInfo = jsonInfo.substr(0, jsonInfo.size()-1);

        // cout << "NewJSONInfo: " << NewJSONInfo << endl;

        stringstream InfoForJSON(NewJSONInfo);
        
        //* Desglosa Info del CMD
        while (getline(InfoForJSON, bycommas, ',')) {
            std::stringstream insidecommas(bycommas);
            while (std::getline(insidecommas, bycolon, ':')) {
                AllData.push_back(bycolon);
            }
        }
        for (size_t i = 0; i < AllData.size(); i++) {
            if (i % 2 == 0) {
                Field.push_back(AllData[i]);
            } else {
                SubData.push_back(AllData[i]);
            }
        }

        cout << " ----- Field ------ " << endl;
        for (size_t i = 0; i < Field.size(); ++i) {
            cout << Field[i] << endl;
        }
        cout << " ----- SubData ------ " << endl;
        for (size_t i = 0; i < SubData.size(); ++i) {
            cout << SubData[i] << endl;
        }
    } else {
        // cout << "separamos aca que u" << endl;
        
        jsonInfo = jsonInfo.substr(0,jsonInfo.size());
        NewJSONInfo = jsonInfo.substr(0, jsonInfo.size());

        // cout << "NewJSONInfo: " << NewJSONInfo << endl;

        stringstream InfoForJSON(NewJSONInfo);
        while (getline(InfoForJSON, bycommas, '-')) {
            std::stringstream insidecommas(bycommas);
            while (std::getline(insidecommas, bycolon, ':')) {
                AllData.push_back(bycolon);
            }
        }
        for (size_t i = 0; i < AllData.size(); ++i) {
            //cout << AllData[i] << endl;
            if (AllData[i].find('=') != string::npos) {
                std::stringstream hayigual(AllData[i]);
                string igual;
                while (std::getline(hayigual, igual, '=')) {
                    if (igual.compare("data") != 0) {
                        Field.push_back(igual);
                    }
                }
            } else {
                SubData.push_back(AllData[i]);
            }
        }
    }
}

void Funciones::UpdateIndices (int argc, char* argv[]) {
    string binName, jsonName, fileName, idxName, header; vector<string> vectorInfoHeader, fieldsCapacity, fieldsName, vectorDataType; ofstream idxFile; vector<json> registros;

    jsonName = argv[2]; jsonName = jsonName.substr(0,jsonName.size()-4);
    binName = jsonName + ".bin";
    fileName = argv[2]; fileName = fileName.substr(0, fileName.size()-4);
    idxName = fileName + ".idx";

    vectorInfoHeader = DeconstruccionHeader(binName, header);
    AssignFieldVectors(vectorInfoHeader, fieldsName, fieldsCapacity, vectorDataType);
    int sizebson = stoi(vectorInfoHeader[((stoi(vectorInfoHeader[1])*3)+5)+1+stoi(vectorInfoHeader[((stoi(vectorInfoHeader[1])*3)+5)])*2]);

    int reachSK = 1;
    for (int i = 0; i < stoi(vectorInfoHeader[stoi(vectorInfoHeader[1])*3+5]); i++) {
        string testsecKey = fieldsName[stoi(vectorInfoHeader[((stoi(vectorInfoHeader[1])*3)+5)+reachSK])];
        string sdxName = fileName + "-" + testsecKey + ".sdx";
        reachSK = reachSK+2;
    }

    registros = JSONsFromBin(header, binName, sizebson, fieldsName, registros);


    //* Creacion Archivo de Indices Primarios
    CrearIndicesPrimarios(idxFile, idxName, vectorInfoHeader, fieldsName, vectorDataType, registros);

    //* Creacion Archivos de Indices Secundarios
    CrearIndicesSecundarios (fileName, vectorInfoHeader, fieldsName, registros);
}

void Funciones::HeaderandVectorsSetUp (ifstream &ArchivoIDX, string &binName, string &header, string &keyType, string &idxFile, long &AvailListPos, vector<string> &vectorInfoHeader, vector<string> &fieldsName, vector<string> &fieldsCapacity, vector<string> &vectorDataType, vector<intindicePrimario> &vectorintIndices, vector<stringindicePrimario> &vectorstringIndices) {
    //* Recupera Header
    vectorInfoHeader = DeconstruccionHeader(binName, header);
    AssignFieldVectors(vectorInfoHeader, fieldsName, fieldsCapacity, vectorDataType);

    string ComingFromHeader = vectorInfoHeader[vectorInfoHeader.size()-1];
    ComingFromHeader.erase(remove(ComingFromHeader.begin(), ComingFromHeader.end(), '*'), ComingFromHeader.end());
    AvailListPos = stol(ComingFromHeader);
    
    keyType = vectorDataType[stoi(vectorInfoHeader[(stoi(vectorInfoHeader[1])*3+3)])];
    
    ArchivoIDX.open(idxFile,std::ios::binary);

    //* Carga Indices
    IDXsToVectors(keyType, ArchivoIDX, vectorintIndices, vectorstringIndices);
}

void Funciones::ReWriteHeader (std::fstream &binFile, string &binName, long &offsetRecuperado, vector<string> &vectorInfoHeader) {
    // cout << "\nheader: " << header << endl;
    // cout << "vectorInfoHeader" << endl;
    for (size_t i = 0; i < vectorInfoHeader.size(); ++i) {
        // cout << vectorInfoHeader[i] << endl;
    }
    // cout << "New AvailListPos: " << offsetRecuperado << endl;

    string GoingToHeader = to_string(offsetRecuperado);
    while (GoingToHeader.size() < 6) {
        GoingToHeader.insert(0,"*");
    }

    string updateHeader;

    int cont = 1;
    int inside = 2;
    int vuelta = 0;
    for (size_t i = 0; i < vectorInfoHeader.size(); ++i) {
        if (i == vectorInfoHeader.size()-1) {
            updateHeader.append(GoingToHeader);
        } else {
            if (i == 2) {
                while (cont <= stoi(vectorInfoHeader[1])) {
                    if (vuelta == 2) {
                        updateHeader.append(vectorInfoHeader[inside] + ",");
                        inside++;
                        cont++;
                        vuelta = 0;
                    } else {
                        updateHeader.append(vectorInfoHeader[inside] + "/");
                        inside++;
                        vuelta++;
                    }
                }
                i = inside-1;
            } else {
                updateHeader.append(vectorInfoHeader[i] + ",");
            }
        }
    }

    updateHeader.append("\n");
    // cout << "updateHeader: " << updateHeader << endl;
    binFile.open(binName, std::ios::binary | std::ios::in | std::ios::out);
    // cout << "seek pointer: " << binFile.tellp() << endl;
    binFile.write(updateHeader.c_str(), updateHeader.length());
    // cout << "seek pointer: " << binFile.tellp() << endl;
    binFile.close();
}

void Funciones::TallyJSON (std::fstream &binFile, long &AvailListPos, long &offsetRecuperado, json &FoundIt, string &binName, vector<string> &fieldsName, vector<string> &fieldsCapacity) {
    string toKey = "*" + to_string(AvailListPos);
    toKey.resize(stoi(fieldsCapacity[0]),'\0');
    FoundIt[fieldsName[0]] = toKey;

    //cout << FoundIt.dump(4) << endl;

    binFile.open(binName, std::ios::binary | std::ios::in | std::ios::out);
    //cout << "seek pointer: " << binFile.tellp() << endl;
    vector<char> buffer;
    json::to_bson(FoundIt,buffer);
    string header;
    getline(binFile, header);
    //cout << "seek pointer: " << binFile.tellp() << endl;
    binFile.seekp(binFile.tellp() + offsetRecuperado,std::ios::beg);
    //cout << "seek pointer: " << binFile.tellp() << endl;
    binFile.write(buffer.data(), buffer.size());
    //cout << "seek pointer: " << binFile.tellp() << endl;
    binFile.close();
}

//* Funciones para cada Operación -----------------------------------------------------------------------------------------------------------------------------
void Funciones::CrearArchivo (int argc, char* argv[]) {
    ifstream jsonFile; string jsonFileRoute, jsonName, jsonLine, jsonDescription, binName, idxName, secKeys, header; json dataJSON; bool correctTypes; int recordSize;

    jsonName = argv[2]; jsonName = jsonName.substr(0,jsonName.size()-5);
    binName = jsonName + ".bin"; idxName = jsonName + ".idx";
    jsonFileRoute.append(argv[2]);
    recordSize = 0;

    jsonFile.open(jsonFileRoute);
    if (jsonFile.fail()){
        cerr << "{\"result\": \"ERROR\", \"error\": \"json file not found\"}" << endl;
    } else {
        while (getline(jsonFile, jsonLine, '\n')) {
            jsonDescription.append(jsonLine.append("\n"));
        }
        try {
            dataJSON = json::parse(jsonDescription);
        } catch (json::parse_error& FormatError) {
            cerr << "{\"result\": \"ERROR\", \"error\": \"Format not recognized\"}" << endl;
        }

        for (size_t i = 0; i < dataJSON["fields"].size(); i++){
            auto& element = dataJSON["fields"].at(i);
            string elementType = element["type"];
            if (elementType.compare("char") == 0 || elementType.compare("int") == 0 || elementType.compare("float") == 0){
                correctTypes = true;
            } else {
                correctTypes = false;
            }
        }

        if (dataJSON["primary-key"] == nullptr) {
            cerr << "{\"result\": \"ERROR\", \"error\": \"primary index field does not exist\"}" << endl;
        } else if (dataJSON["secondary-key"] == nullptr || dataJSON["secondary-key"] == dataJSON["primary-key"]) {
            cerr << "{\"result\": \"ERROR\", \"error\": \"secondary index field does not exist\"}" << endl;
        } else if (!correctTypes) {
            cerr << "{\"result\": \"ERROR\", \"error\": \"invalid type\"}" << endl;
        } else {
            header = ConstruccionHeader(dataJSON, header, secKeys, jsonName, recordSize);
            cout << "{\"result\": \"OK\", \"fields-count\": " << dataJSON["fields"].size() << ", \"file\": \"" << binName << "\", \"index\": \"" << idxName << "\", \"secondary\": [" << secKeys << "]}" << endl;
        }
    }
    jsonFile.close();

    ofstream binFile(binName, std::ios::binary);
    binFile.write(header.c_str(), header.length());
    binFile.close();

    ofstream idxFile(idxName);
    idxFile.close();
}

void Funciones::CargarArchivo (int argc, char* argv[]) {
    string binName, csvName, header; vector<string> vectorInfoHeader; ifstream csvFile; bool alltypes;
    binName = argv[2]; csvName = argv[4];

    vectorInfoHeader = DeconstruccionHeader(binName, header); //* Desglosa el Header en string y sub-vectores
    csvFile.open(csvName);
    alltypes = CheckCSVFields(csvFile, vectorInfoHeader); //* Corrobora Types en CSV
    
    if (!alltypes){
        cerr << "{\"result\": \"ERROR\", \"error\": \"CSV fields do not match file structure\"}" << endl;
    } else {
        vector<string> fieldsInfo, fieldsName, fieldsCapacity, vectorDataType; vector<json> registros; int sizebson; ofstream binFile;

        //* --------- Trabajo en CSV ----------
        CSVtoJSON(csvFile, vectorInfoHeader, fieldsInfo, fieldsName, fieldsCapacity, vectorDataType, registros); //! Intentar Validar Missing SK's
        //* ---------- Cuenta de Registros ----------
        CuentaRegistros(registros);
        //* ---------- Tamaño BSON ----------
        for (size_t i = 0; i < registros.size(); i++) { 
            json jsonWrite = registros.at(i);
            vector<char> buffer;
            json::to_bson(jsonWrite,buffer);
            sizebson = buffer.size();
        }
        //* ---------- Implementa Header ----------
        EscribeHeader (binFile, sizebson, binName, header, vectorInfoHeader, registros);
        
        //* ---------- ---------- ---------- TRABAJO CON INDICES ---------- ---------- ----------

        string binName, jsonName, fileName, idxName; vector<string> vectorInfoHeader; ofstream idxFile;

        jsonName = argv[2]; jsonName = jsonName.substr(0,jsonName.size()-4);
        binName = jsonName + ".bin";
        fileName = argv[2]; fileName = fileName.substr(0, fileName.size()-4);
        idxName = fileName + ".idx";

        vectorInfoHeader = DeconstruccionHeader(binName, header);
        AssignFieldVectors(vectorInfoHeader, fieldsName, fieldsCapacity, vectorDataType);
        //ViewFieldVectors(vectorInfoHeader, fieldsName, fieldsCapacity, vectorDataType);

        int reachSK = 1;
        for (int i = 0; i < stoi(vectorInfoHeader[stoi(vectorInfoHeader[1])*3+5]); i++) {
            string testsecKey = fieldsName[stoi(vectorInfoHeader[((stoi(vectorInfoHeader[1])*3)+5)+reachSK])];
            string sdxName = fileName + "-" + testsecKey + ".sdx";
            reachSK = reachSK+2;
        }


        //* Creacion Archivo de Indices Primarios
        CrearIndicesPrimarios(idxFile, idxName, vectorInfoHeader, fieldsName, vectorDataType, registros);

        //* Creacion Archivos de Indices Secundarios
        CrearIndicesSecundarios (fileName, vectorInfoHeader, fieldsName, registros);
    }
}

void Funciones::Describir (int argc, char* argv[]) {
    ifstream jsonFile, binFile; string jsonName, jsonLine, jsonDescription, binName, header; vector<string> vectorInfoHeader;

    jsonName = argv[2]; jsonName = jsonName.substr(0,jsonName.size()-4);
    binName = jsonName + ".bin";

    vectorInfoHeader = DeconstruccionHeader(binName, header); //* Desglosa el Header en string y sub-vectores

    unsigned int CompareHeaderSize = stoi(vectorInfoHeader[1])*3+5 + stoi(vectorInfoHeader[(stoi(vectorInfoHeader[1])*3+5)])*2;
    if (CompareHeaderSize == vectorInfoHeader.size()-1) {
        jsonFile.open(jsonName + ".json");
        if (jsonFile.fail()){
            cerr << "{\"result\": \"ERROR\", \"error\": \"json file not found\"}" << endl;
        } else {
            while (getline(jsonFile, jsonLine, '\n')) { //captura info del JSON
                jsonDescription.append(jsonLine.append("\n"));
            }
            string adicion = "  \"records\": 0\n";
            jsonDescription.insert(jsonDescription.size()-2,adicion);
            try {
                cout << jsonDescription << endl;
            } catch (json::parse_error& FormatError) {
                cerr << "{\"result\": \"ERROR\", \"error\": \"Format not recognized\"}" << endl;
            }
        }
    } else {
        int sizebson = stoi(vectorInfoHeader[((stoi(vectorInfoHeader[1])*3)+5)+1+stoi(vectorInfoHeader[((stoi(vectorInfoHeader[1])*3)+5)])*2]);
        int recordCount = 0;

        binFile.open(binName, std::ios::binary);
        getline(binFile,header);
        while (binFile.peek() != EOF){
            vector<char> buffer(sizebson);
            binFile.read(buffer.data(), buffer.size());
            json j_from_bson = json::from_bson(buffer);

            vector<string> fieldsName, fieldsCapacity, vectorDataType;
            AssignFieldVectors (vectorInfoHeader, fieldsName, fieldsCapacity, vectorDataType);

            string checking = j_from_bson[fieldsName[0]];
            if (checking.at(0) == '*') {
                
            } else {
                recordCount++;
            }
        }
        binFile.close();

        jsonFile.open(jsonName + ".json");
        if (jsonFile.fail()){
            cerr << "{\"result\": \"ERROR\", \"error\": \"json file not found\"}" << endl;
        } else {
            while (getline(jsonFile, jsonLine, '\n')) {
                jsonDescription.append(jsonLine.append("\n"));
            }
            string adicion = "  \"records\": " + to_string(recordCount) + '\n';
            jsonDescription.insert(jsonDescription.size()-2,adicion);
            try {
                cout << jsonDescription << endl;
            } catch (json::parse_error& FormatError) {
                cerr << "{\"result\": \"ERROR\", \"error\": \"Format not recognized\"}" << endl;
            }
        }
    }
    jsonFile.close();
}

void Funciones::Listar (int argc, char* argv[]) {
    ifstream jsonFile, binFile; string jsonName, jsonLine, jsonDescription, header, binName; vector<string> vectorInfoHeader, fieldsCapacity, fieldsName, vectorDataType; vector<json> vectorJSON;

    jsonName = argv[2]; jsonName = jsonName.substr(0,jsonName.size()-4);
    binName = jsonName + ".bin";

    vectorInfoHeader = DeconstruccionHeader(binName, header);
    

    if (!binFile.good()) {
        cerr << "{\"result\": \"ERROR\", \"error\": \"bin file not found\"}" << endl;
    } else {
        binFile.open(binName, std::ios::binary);
        getline(binFile, header);

        int sizebson = stoi(vectorInfoHeader[((stoi(vectorInfoHeader[1])*3)+5)+1+stoi(vectorInfoHeader[((stoi(vectorInfoHeader[1])*3)+5)])*2]);
        int recordCount = 0;
        while (binFile.peek() != EOF){
            vector<char> buffer(sizebson);
            binFile.read(buffer.data(), buffer.size());
            json j_from_bson = json::from_bson(buffer);
            recordCount++;
        }
        binFile.close();

        AssignFieldVectors (vectorInfoHeader, fieldsName, fieldsCapacity, vectorDataType);
        vectorJSON = JSONsFromBin(header, binName, sizebson, fieldsName, vectorJSON);

        cout << "[" << endl;
        for (size_t i = 0; i < vectorJSON.size(); ++i) {
            json JSONWorking = vectorJSON[i];
            //cout << "JSONWorking[fieldsName[0]]: " << JSONWorking[fieldsName[0]] << endl;
            string checking = JSONWorking[fieldsName[0]];
            if (checking.at(0) == '*') {
                
            } else {
                cout << " {" << endl;
                for (size_t j = 0; j < fieldsName.size(); ++j) {
                    bool esInt = false;
                    string hold = JSONWorking[fieldsName[j]];
                    if (hold.size() == 4) {
                        esInt = true;
                    }
                    hold.erase(remove(hold.begin(), hold.end(), '\0'), hold.end());
                    JSONWorking[fieldsName[j]] = hold;
                    if (!esInt) {
                        if ( j != fieldsName.size()-1){
                            cout << "  \"" << fieldsName[j] << "\": " << JSONWorking[fieldsName[j]] << "," << endl;
                        } else {
                            cout << "  \"" << fieldsName[j] << "\": " << JSONWorking[fieldsName[j]] << endl;
                        }
                    } else {
                        if ( j != fieldsName.size()-1){
                            cout << "  \"" << fieldsName[j] << "\": " << hold << "," << endl;
                        } else {
                            cout << "  \"" << fieldsName[j] << "\": " << hold << endl;
                        }
                    }
                }
                if (i != vectorJSON.size()-1) {
                    cout << " }," << endl;
                } else {
                    cout << " }" << endl;
                }
            }
        }
        cout << "]" << endl;
    }
}

void Funciones::BuscarPK (int argc, char* argv[]) {
    ifstream ArchivoIDX; long offsetRecuperado; string binName, header, jsonName, keyType, fileName, idxFile, argumento, toSearch; 
    vector<string> vectorInfoHeader,fieldsName, fieldsCapacity, vectorDataType; 
    vector<intindicePrimario> vectorintIndices; vector<stringindicePrimario> vectorstringIndices;

    jsonName = argv[2]; jsonName = jsonName.substr(0,jsonName.size()-4);
    binName = jsonName + ".bin";
    argumento = argv[5]; toSearch = argumento.substr(7, argumento.size());

    //* Recupera Header
    vectorInfoHeader = DeconstruccionHeader(binName, header);
    AssignFieldVectors(vectorInfoHeader, fieldsName, fieldsCapacity, vectorDataType);
    
    keyType = vectorDataType[stoi(vectorInfoHeader[(stoi(vectorInfoHeader[1])*3+3)])];
    fileName = argv[2];
    idxFile = fileName.substr(0, fileName.size()-4) + ".idx";

    ArchivoIDX.open(idxFile,std::ios::binary);

    //* Carga Indices
    IDXsToVectors(keyType, ArchivoIDX, vectorintIndices, vectorstringIndices);;

    //* Busqueda
    if (keyType.compare("int") == 0 || keyType.compare("float") == 0) {
        if (intPKSearch(vectorintIndices, offsetRecuperado, toSearch, fileName, fieldsName, vectorInfoHeader) == NULL) {
            cerr << "{\n  \"result\": \"not found\"\n}" << endl;
        } else {
            json FoundIt = intPKSearch(vectorintIndices, offsetRecuperado, toSearch, fileName, fieldsName, vectorInfoHeader);
            coutJSON(FoundIt, fieldsName);
        }
        
    } else {
        if (stringPKSearch (vectorstringIndices, offsetRecuperado, toSearch, fileName, fieldsName, vectorInfoHeader) == NULL) {
            cerr << "{\n  \"result\": \"not found\"\n}" << endl;
        } else {
            json FoundIt = stringPKSearch (vectorstringIndices, offsetRecuperado, toSearch, fileName, fieldsName, vectorInfoHeader);
            coutJSON(FoundIt, fieldsName);
        }
        
    }
}

void Funciones::BuscarSK (int argc, char* argv[], string &Operacion) {
    string binName, jsonName, header, testsecKey, fieldName; vector<string> vectorInfoHeader, fieldsName, fieldsCapacity, vectorDataType;
    bool found; long offsetRecuperado;
    string fileName, idxFile, NombreCampo, argumento, toSearch, NombreArchivo; ifstream ArchivoIDX;
    vector<intindicePrimario> vectorintIndices; vector<stringindicePrimario> vectorstringIndices;

    jsonName = argv[2]; jsonName = jsonName.substr(0,jsonName.size()-4);
    binName = jsonName + ".bin";
    fieldName = Operacion.substr(4,Operacion.size());
    found = false;
    fileName = argv[2];
    idxFile = fileName.substr(0, fileName.size()-4) + ".idx";
    NombreCampo = Operacion.substr(4,Operacion.size());
    argumento = argv[5];
    toSearch = argumento.substr(7, argumento.size());
    NombreArchivo = fileName.substr(0, fileName.size()-4) + "-" + NombreCampo + ".sdx";

    vectorInfoHeader = DeconstruccionHeader(binName, header);

    //* Valor del Field de la Secondary Key
    for (size_t i = 0; i < vectorInfoHeader.size(); ++i) {
        if (vectorInfoHeader[i].compare(fieldName) == 0){
            testsecKey = vectorInfoHeader[i+1];
            found = true;
        }
        if (found) {
            break;
        }
    }

    ArchivoIDX.open(NombreArchivo,std::ios::binary);
    SDXsToVectors (testsecKey, ArchivoIDX, vectorintIndices, vectorstringIndices);
    AssignFieldVectors (vectorInfoHeader, fieldsName, fieldsCapacity, vectorDataType);

    //* Busqueda
    if (testsecKey.compare("int") == 0 || testsecKey.compare("float") == 0) {
        intSKSearch (vectorintIndices, offsetRecuperado, toSearch, fileName, fieldsName, vectorInfoHeader);
    } else {
        stringSKSearch (vectorstringIndices, offsetRecuperado, toSearch, fileName, fieldsName, vectorInfoHeader);
    }
}

void Funciones::ModificarRegistro (int argc, char* argv[]) {
    string jsonInfo, NewJSONInfo, bycommas, bycolon, binName, header, keyType, fileName, idxFile, toSearch;
    vector<string> AllData, Field, SubData, vectorInfoHeader, fieldsName, fieldsCapacity, vectorDataType;
    vector<intindicePrimario> vectorintIndices; vector<stringindicePrimario> vectorstringIndices;
    ifstream ArchivoIDX;
    long offsetRecuperado;

    CMDWork (argc, argv, jsonInfo, binName, NewJSONInfo, bycommas, bycolon, AllData, Field, SubData);

    Field.erase(Field.begin());
    Field.erase(Field.begin());

    // cout << "Field" << endl;
    // for (size_t i = 0; i < Field.size(); ++i) {
    //     cout << Field[i] << endl;
    // }
    // cout << "SubData" << endl;
    // for (size_t i = 0; i < SubData.size(); ++i) {
    //     cout << SubData[i] << endl;
    // }

    vectorInfoHeader = DeconstruccionHeader (binName, header);
    AssignFieldVectors (vectorInfoHeader, fieldsName, fieldsCapacity, vectorDataType);

    if (Field.size() == fieldsName.size()) {
        keyType = vectorDataType[stoi(vectorInfoHeader[(stoi(vectorInfoHeader[1])*3+3)])];
        fileName = argv[2];
        idxFile = fileName.substr(0, fileName.size()-4) + ".idx";

        ArchivoIDX.open(idxFile,std::ios::binary);

        //* Carga Indices
        IDXsToVectors(keyType, ArchivoIDX, vectorintIndices, vectorstringIndices);;

        //* Busqueda
        json FoundIt;
        string searching = argv[4];
        toSearch = searching.substr(4, searching.size());
        if (keyType.compare("int") == 0 || keyType.compare("float") == 0) {
            
            if (intPKSearch(vectorintIndices, offsetRecuperado, toSearch, fileName, fieldsName, vectorInfoHeader) == NULL) {
                cerr << "{\"result\":\"PRIMARY KEY NOT FOUND\"}" << endl;
            } else {
                FoundIt = intPKSearch(vectorintIndices, offsetRecuperado, toSearch, fileName, fieldsName, vectorInfoHeader);
                for (size_t i = 0; i < fieldsName.size(); i++) {
                    FoundIt[fieldsName[i]] = SubData[i];
                }
                std::fstream binFile(binName, std::ios::binary | std::ios::in | std::ios::out);
                json jsonWrite = FoundIt;

                for (int i = 0; i < stoi(vectorInfoHeader[1]); i++) {
                    SubData[i].resize(stoi(fieldsCapacity[i]),'\0');
                    jsonWrite[fieldsName[i]] = SubData[i];
                }

                vector<char> buffer;
                json::to_bson(jsonWrite,buffer);
                string header;
                getline(binFile, header);
                // cout << "seek pointer: " << binFile.tellp() << endl;
                binFile.seekp(binFile.tellp() + offsetRecuperado,std::ios::beg);
                // cout << "seek pointer: " << binFile.tellp() << endl;
                binFile.write(buffer.data(), buffer.size());
                // cout << "seek pointer: " << binFile.tellp() << endl;
                binFile.close();

                UpdateIndices(argc,argv);
                cout << "{\"result\":\"OK\"}" << endl;
            }
        } else {
            if (stringPKSearch (vectorstringIndices, offsetRecuperado, toSearch, fileName, fieldsName, vectorInfoHeader) == NULL) {
                cerr << "{\"result\":\"PRIMARY KEY NOT FOUND\"}" << endl;
            } else {
                json FoundIt = stringPKSearch (vectorstringIndices, offsetRecuperado, toSearch, fileName, fieldsName, vectorInfoHeader);
                for (size_t i = 0; i < fieldsName.size(); i++) {
                    FoundIt[fieldsName[i]] = SubData[i];
                }
                std::fstream binFile(binName, std::ios::binary | std::ios::in | std::ios::out);
                json jsonWrite = FoundIt;

                for (int i = 0; i < stoi(vectorInfoHeader[1]); i++) {
                    SubData[i].resize(stoi(fieldsCapacity[i]),'\0');
                    jsonWrite[fieldsName[i]] = SubData[i];
                }

                vector<char> buffer;
                json::to_bson(jsonWrite,buffer);
                string header;
                getline(binFile, header);
                binFile.seekp(binFile.tellp() + offsetRecuperado,std::ios::beg);
                binFile.write(buffer.data(), buffer.size());
                binFile.close();

                UpdateIndices(argc,argv);
                cout << "{\"result\":\"OK\"}" << endl;
            }
        }
    } else {
        cerr << "{\"result\":\"FIELDS DON'T MATCH STRUCTURE\"}" << endl;
    }
}

void Funciones::BorrarRegistro (int argc, char* argv[]) {
    ifstream ArchivoIDX; long offsetRecuperado, AvailListPos; string binName, header, jsonName, keyType, fileName, idxFile, argumento, toSearch; 
    vector<string> vectorInfoHeader,fieldsName, fieldsCapacity, vectorDataType; 
    vector<intindicePrimario> vectorintIndices; vector<stringindicePrimario> vectorstringIndices;

    jsonName = argv[2]; jsonName = jsonName.substr(0,jsonName.size()-4);
    binName = jsonName + ".bin";
    argumento = argv[4]; toSearch = argumento.substr(4, argumento.size());
    fileName = argv[2];
    idxFile = fileName.substr(0, fileName.size()-4) + ".idx";

    HeaderandVectorsSetUp (ArchivoIDX, binName, header, keyType, idxFile, AvailListPos, vectorInfoHeader, fieldsName, fieldsCapacity, vectorDataType, vectorintIndices, vectorstringIndices);

    //* Busqueda
    if (keyType.compare("int") == 0 || keyType.compare("float") == 0) {
        //cout << "headerARRIBA: " << header << endl;
        //offsetRecuperado = stol(vectorInfoHeader[vectorInfoHeader.size()-1]);
        if (intPKSearch(vectorintIndices, offsetRecuperado, toSearch, fileName, fieldsName, vectorInfoHeader) == NULL) {
            cerr << "{\n  \"result\": \"not found\"\n}" << endl;
        } else {
            json FoundIt = intPKSearch(vectorintIndices, offsetRecuperado, toSearch, fileName, fieldsName, vectorInfoHeader);
            //cout << "offsetRecuperado: " << offsetRecuperado << endl;
            
            std::fstream binFile;

            TallyJSON (binFile, AvailListPos, offsetRecuperado, FoundIt, binName, fieldsName, fieldsCapacity);
            UpdateIndices(argc,argv);
            ReWriteHeader (binFile, binName, offsetRecuperado, vectorInfoHeader);
            cout << "{\"result\":\"OK\"}" << endl;
        }
        
    } else {
        if (stringPKSearch (vectorstringIndices, offsetRecuperado, toSearch, fileName, fieldsName, vectorInfoHeader) == NULL) {
            cerr << "{\n  \"result\": \"not found\"\n}" << endl;
        } else {
            json FoundIt = stringPKSearch (vectorstringIndices, offsetRecuperado, toSearch, fileName, fieldsName, vectorInfoHeader);
            //coutJSON(FoundIt, fieldsName);
            std::fstream binFile;

            TallyJSON (binFile, AvailListPos, offsetRecuperado, FoundIt, binName, fieldsName, fieldsCapacity);
            UpdateIndices(argc,argv);
            ReWriteHeader (binFile, binName, offsetRecuperado, vectorInfoHeader);
            cout << "{\"result\":\"OK\"}" << endl;
        }
    }
}

void Funciones::AgregarRegistro (int argc, char* argv[]) {
    string binName, header, jsonInfo, toSearch, NewJSONInfo, bycommas, bycolon;
    vector<string> vectorInfoHeader, AllData, Field, SubData, fieldsName, fieldsCapacity, vectorDataType;

    CMDWork(argc, argv, jsonInfo, binName, NewJSONInfo, bycommas, bycolon, AllData, Field, SubData);

    vectorInfoHeader = DeconstruccionHeader (binName, header);
    AssignFieldVectors (vectorInfoHeader, fieldsName, fieldsCapacity, vectorDataType);
    //ViewFieldVectors (vectorInfoHeader, fieldsName, fieldsCapacity, vectorDataType);

    //vectorInfoHeader.at(vectorInfoHeader.size()-1) = to_string(231);

    if (vectorInfoHeader[vectorInfoHeader.size()-1] == ("****-1")) {
        if (Field.size() == fieldsName.size()) {
            bool match = true;
            for (size_t i = 0; i < Field.size(); ++i) {
                if (Field[i].compare(fieldsName[i]) != 0){
                    match = false;
                }
            }

            if (!match) {
                cerr << "{\"result\":\"FIELDS DON'T MATCH STRUCTURE\"}" << endl;
            } else {
                json jsonObject;
                for (int i = 0; i < stoi(vectorInfoHeader[1]); i++) {
                    SubData[i].resize(stoi(fieldsCapacity[i]),'\0');
                    jsonObject[fieldsName[i]] = SubData[i];
                }

                ofstream binFile;
                binFile.open(binName, std::ios::app);
                json jsonWrite = jsonObject;
                vector<char> buffer;
                json::to_bson(jsonWrite,buffer);
                binFile.write(buffer.data(), buffer.size());
                binFile.close();
                UpdateIndices(argc,argv);
                cout << "{\"result\":\"OK\"}" << endl;
            }
        } else {
            cerr << "{\"result\":\"FIELDS DON'T MATCH STRUCTURE\"}" << endl;
        }
    } else {
        cout << "Implementar con Avail List" << endl;
        if (Field.size() == fieldsName.size()) {
            bool match = true;
            for (size_t i = 0; i < Field.size(); ++i) {
                if (Field[i].compare(fieldsName[i]) != 0){
                    match = false;
                }
            }
            if (!match) {
                cerr << "{\"result\":\"FIELDS DON'T MATCH STRUCTURE\"}" << endl;
            } else {

                //* Recuperar Avail List = Offset Recuperado (usar AvailListPos)
                long AvailListPos;

                string ComingFromHeader = vectorInfoHeader[vectorInfoHeader.size()-1];
                ComingFromHeader.erase(remove(ComingFromHeader.begin(), ComingFromHeader.end(), '*'), ComingFromHeader.end());
                AvailListPos = stol(ComingFromHeader);
                //cout << "AvailListPos: " << AvailListPos << endl;

                //* Abrir Archivo, Recuperar ID del JSON en AvailListPos, escribirlo el en AvailList(Header Spot)

                ifstream captureBin;
                captureBin.open(binName, std::ios::binary);
                string headerplaceholder;
                getline(captureBin, headerplaceholder);
                long readingpointer = captureBin.tellg();
                long sizebson = stol(vectorInfoHeader[vectorInfoHeader.size()-2]);
                vector<char> buffer(sizebson);
                captureBin.seekg(readingpointer+AvailListPos, std::ios::beg);
                captureBin.read(buffer.data(), buffer.size());
                json j_from_bson = json::from_bson(buffer);
                captureBin.close();
                //cout << j_from_bson.dump(4) << endl;

                string hold = j_from_bson[fieldsName[0]];
                hold.erase(remove(hold.begin(), hold.end(), '\0'), hold.end());
                //cout << "hold: " << hold << endl;
                
                while (hold.size() < 6) {
                    hold.insert(0,"*");
                }

                std::fstream WriteHeader;
                string updateHeader;
                int cont = 1;
                int inside = 2;
                int vuelta = 0;
                for (size_t i = 0; i < vectorInfoHeader.size(); ++i) {
                    if (i == vectorInfoHeader.size()-1) {
                        updateHeader.append(hold);
                    } else {
                        if (i == 2) {
                            while (cont <= stoi(vectorInfoHeader[1])) {
                                if (vuelta == 2) {
                                    updateHeader.append(vectorInfoHeader[inside] + ",");
                                    inside++;
                                    cont++;
                                    vuelta = 0;
                                } else {
                                    updateHeader.append(vectorInfoHeader[inside] + "/");
                                    inside++;
                                    vuelta++;
                                }
                            }
                            i = inside-1;
                        } else {
                            updateHeader.append(vectorInfoHeader[i] + ",");
                        }
                    }
                }

                updateHeader.append("\n");
                WriteHeader.open(binName, std::ios::binary | std::ios::in | std::ios::out);
                WriteHeader.write(updateHeader.c_str(), updateHeader.length());
                WriteHeader.close();


                json jsonObject;
                for (int i = 0; i < stoi(vectorInfoHeader[1]); i++) {
                    SubData[i].resize(stoi(fieldsCapacity[i]),'\0');
                    jsonObject[fieldsName[i]] = SubData[i];
                }

                //cout << jsonObject.dump(4) << endl;

                json jsonWrite = jsonObject;

                std::fstream binFile;
                binFile.open(binName, std::ios::binary | std::ios::in | std::ios::out);
                vector<char> writingbuffer;
                json::to_bson(jsonWrite,writingbuffer);
                string header;
                getline(binFile, header);
                // cout << "seek pointer: " << binFile.tellp() << endl;
                binFile.seekp(binFile.tellp() + AvailListPos,std::ios::beg);
                // cout << "seek pointer: " << binFile.tellp() << endl;
                binFile.write(writingbuffer.data(), writingbuffer.size());
                //cout << "seek pointer: " << binFile.tellp() << endl;
                binFile.close();

                UpdateIndices(argc,argv);
                cout << "{\"result\":\"OK\"}" << endl;

            }
        } else {
            cerr << "{\"result\":\"FIELDS DON'T MATCH STRUCTURE\"}" << endl;
        }
    }
}

void Funciones::Compactar (int argc, char* argv[]) {
    vector<json> vectorJSON; 
    vector<string> vectorInfoHeader, fieldsName, vectorDataType, fieldsCapacity; 
    int sizebson; string header, binName, jsonName; ofstream binFile;

    jsonName = argv[2]; jsonName = jsonName.substr(0,jsonName.size()-4);
    binName = jsonName + ".bin";

    vectorInfoHeader = DeconstruccionHeader(binName, header);
    AssignFieldVectors(vectorInfoHeader, fieldsName, fieldsCapacity, vectorDataType);
    
    for (size_t i = 0; i < vectorInfoHeader.size(); ++i) {
        cout << vectorInfoHeader[i] << endl;
    }
    
    string capturabson = vectorInfoHeader[((stoi(vectorInfoHeader[1])*3)+5)+1+stoi(vectorInfoHeader[((stoi(vectorInfoHeader[1])*3)+5)])*2];
    sizebson = stoi(capturabson);

    JSONsFromBin (header, binName, sizebson, fieldsName, vectorJSON);

    std::fstream escribe;
    long minusone = -1;
    ReWriteHeader (escribe, binName, minusone, vectorInfoHeader);

    for (size_t i = 0; i < vectorInfoHeader.size(); ++i) {
        cout << vectorInfoHeader[i] << endl;
    }

    //EscribeHeader (binFile, sizebson, binName, header, vectorInfoHeader, vectorJSON);
}