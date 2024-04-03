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

int main(int argc, char* argv[]) {

    if (argc != 3) { // Validación Input cout errors
		switch (argc) {
		case 1:
			cerr << "ERROR -- Faltan 2 Parámetros.\nUse:\n" << argv[0] << " [./data/superstore.csv] [CA-2015-149587]" << endl;
			return 1;
			break;
		case 2:
			cerr << "ERROR -- Faltan 1 Parámetros.\nUse:\n" << argv[0] << " " << argv[1] << " [CA-2015-149587]" << endl;
			return 1;
			break;
		case 3:
			cerr << "ERROR -- Ingresó Demasiados Parámetros.\nUse:\n" << argv[0] << "[./build/basic-search] [./data/superstore.csv] [CA-2015-149587]" << endl;
			return 1;
			break;			
		default:
			break;
		}
	} else {
        string Filename = argv[1];
        string KeyToSearch = argv[2];

        ifstream SuperstoreFile;
        string SuperstoreData;
        vector<string> DataInfo;
        vector<string> FixedDataInfo;
        vector<string> Orders;
        string Write = "";
        string toWrite = "";
        int RecordCount = 0;
        float TotalSales = 0;

        size_t charPosition;
        vector<string>caracteresReplace = {"†", "‡", "‰", "ì", "î", "ˆ", "È", "¸", "æ"};

        SuperstoreFile.open(Filename);
        if (!SuperstoreFile.fail()) {
            while (getline(SuperstoreFile, SuperstoreData,'\n')) {
                for (size_t i = 0; i < SuperstoreData.size(); i++) { //Obtiene la información de cada Field
                    if (SuperstoreData.at(i) == ',' || i+1 == SuperstoreData.size()) {
                        DataInfo.push_back(Write);
                        Write = "";
                        continue;
                    } else if (SuperstoreData.at(i) == '\"') {
                        for (size_t j = i+1; j < SuperstoreData.size(); j++) {
                            if (SuperstoreData.at(j) == '\"' && SuperstoreData.at(j+1) == ',') {
                                DataInfo.push_back(Write);
                                Write = "";
                                i=j+1;
                                break;
                            } else {
                                Write += SuperstoreData.at(j);
                            }
                        }
                    } else {
                        Write += SuperstoreData.at(i); //Prelim Write in case de Cambiar Caracteres
                    }
                }

                                for	(size_t g = 0; g < DataInfo.size(); g++){ //Reemplaza caracteres
                    toWrite = DataInfo.at(g);
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

                    FixedDataInfo.push_back(toWrite);
                }
                
                for (size_t i = 0; i < FixedDataInfo.size(); ++i) {
                    if (FixedDataInfo[1].compare(KeyToSearch) == 0) {
                        Orders.push_back(FixedDataInfo[1]);
                        Orders.push_back(FixedDataInfo[16]);
                        Orders.push_back(FixedDataInfo[17]);
                        Orders.push_back(FixedDataInfo[18]);
                        Orders.push_back(FixedDataInfo[19]);
                        TotalSales += (stof(FixedDataInfo[17])*(stoi(FixedDataInfo[18])*(1-(stof(FixedDataInfo[19])))));
                        RecordCount++;
                    }
                    break;
                }
                for (size_t i = 0; i < Orders.size(); ++i) {
                    if ( (i+1) % 5 == 0) {
                        cout << Orders[i] << endl;
                    } else {
                        cout << Orders[i] << ",";
                    }
                }
                FixedDataInfo.clear();
                DataInfo.clear();
                Orders.clear();
            }

            cout << endl << "Total Records: " << RecordCount << endl << "Total Sale: " << TotalSales << endl;
        } else {
            cout << "nel" << endl;
        }
        SuperstoreFile.close();
    }
    return 0;
}