#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using std::cout;
using std::cin;
using std::endl;
using std::cerr;
using std::stringstream;
using std::string;
using std::ofstream;
using std::ifstream;
using std::fstream;

int main(int argc, char* argv[]){

    if (argc != 3) {
		if (argc == 1) {
			cerr << "ERROR -- Faltan 2 Parámetros.\nUse: " << argv[0] << " [data.csv] [-json/-fixed]" << endl;
			return 1;
		}
		if (argc == 2) {
			cerr << "ERROR -- Falta 1 Parámetro.\nUse: " << argv[0] << " " << argv[1] << " [-json/-fixed]" << endl;
			return 1;
		}
		if (argc > 3) {
			cerr << "ERROR -- Ingresó Demasiados Parámetros.\nUse: " << argv[0] << " [data.csv] [-json/-fixed]" << endl;
			return 1;
		}
	} else {
		string Filename = argv[1];
		string Flag = argv[2];
		ifstream CSVReading;
		string LineByLine;
		int CountLineas = 0;
		CSVReading.open(Filename);

		if (CSVReading.fail())
			cerr << "No Pude Abrir el Archivo." << endl;
		else {
			
			string id, name, category, price, aisle, bin;
			int idS = 0, nameS = 0, categoryS = 8, priceS = 0, aisleS = 5, binS = 3;

			//Temporal to Capture Input Lines
			int LinesOnCSV = 0;
			ifstream TempFile(Filename);
			string TempLine;
			while (getline(TempFile, TempLine))
				LinesOnCSV++;
			TempFile.close();

			//Temporal to Capture Longest per Line
			ifstream TempFile2(Filename);
			string TempLine2;
			while (getline(TempFile2, TempLine2)){
				stringstream WorkLine2(TempLine2);
				getline(WorkLine2, id, ','); getline(WorkLine2, name, ','); getline(WorkLine2, category, ','); getline(WorkLine2, price, ','); getline(WorkLine2, aisle, ','); getline(WorkLine2, bin, ',');
				if (idS < id.length()) {idS = id.length();}
				if (nameS < name.length()) {nameS = name.length();}
				if (priceS < price.length()) {priceS = price.length();}
			}
			TempFile.close();

			if (Flag.compare("-json") == 0){
				ofstream JSONFile("data.json");
				JSONFile << "[\n";
				while (!CSVReading.eof()){
					CountLineas++;
					getline(CSVReading, LineByLine);
					stringstream WorkLine(LineByLine);
					getline(WorkLine, id, ','); getline(WorkLine, name, ','); getline(WorkLine, category, ','); getline(WorkLine, price, ','); getline(WorkLine, aisle, ','); getline(WorkLine, bin, ',');

					//Work on 0s for BINs
					int countzeros = 0;
					while (bin[countzeros] == '0')
						countzeros++;
					bin.erase(0,countzeros);
					if (bin.empty())
						bin.append("0");

					if (CountLineas > 1 && CountLineas < LinesOnCSV)
						JSONFile << "\t{\"id\":" << id << ", \"name\": \"" << name << "\", \"category\":" << category << ", \"price\":" << price << ",  \"aisle\":" << aisle << ", \"bin\":" << bin << "},\n";
					if (CountLineas == LinesOnCSV)
						JSONFile << "\t{\"id\":" << id << ", \"name\": \"" << name << "\", \"category\":" << category << ", \"price\":" << price << ",  \"aisle\":" << aisle << ", \"bin\":" << bin << "}\n";
				}
				JSONFile << "]";
			} else if (Flag.compare("-fixed") == 0) {
				ofstream TXTFile("data.txt");
				while (!CSVReading.eof()){
					CountLineas++;
					getline(CSVReading, LineByLine);
					stringstream WorkLine(LineByLine);
					getline(WorkLine, id, ','); getline(WorkLine, name, ','); getline(WorkLine, category, ','); getline(WorkLine, price, ','); getline(WorkLine, aisle, ','); getline(WorkLine, bin, ',');
					if (bin.length()<3)
						for (int MissingZeros = 3-bin.length(); MissingZeros > 0; MissingZeros--)
							bin.assign('0'+bin);
					if (CountLineas != LinesOnCSV)
						TXTFile << std::left << std::setw(idS+3) << id << std::left << std::setw(nameS+3) << name << std::left << std::setw(categoryS+3) << category << std::left << std::setw(priceS+3) << price << std::left << std::setw(aisleS+3) << aisle << std::left << std::setw(binS) << bin << endl;
					else
						TXTFile << std::left << std::setw(idS+3) << id << std::left << std::setw(nameS+3) << name << std::left << std::setw(categoryS+3) << category << std::left << std::setw(priceS+3) << price << std::left << std::setw(aisleS+3) << aisle << std::left << std::setw(binS) << bin;
				}
			} else
				cerr << "Esta Opción no es Válida." << endl;
			CSVReading.close();
		}
	}
    return 0;
}