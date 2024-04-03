#include <iostream>
#include <fstream>

using std::cout;
using std::cerr;
using std::endl;
using std::cin;
using std::string;
using std::fstream;

int main(int argc, char* argv[]){

	if (argc != 3) {
		if (argc == 1) {
			cerr << "ERROR -- Faltan 2 Parámetros.\nUse: " << argv[0] << " [archivo.txt] [palabra]" << endl;
			return 1;
		}
		if (argc == 2) {
			cerr << "ERROR -- Falta 1 Parámetro.\nUse: " << argv[0] << " " << argv[1] << " [palabra]" << endl;
			return 1;
		}
		if (argc > 3) {
			cerr << "ERROR -- Ingresó Demasiados Parámetros.\nUse: " << argv[0] << " [archivo.txt] [palabra]" << endl;
		return 1;
		}
	} else {
		string Filename = argv[1];
		string SearchPhrase = argv[2];
		string Linea;
		int Occurances = 0;
		int ContLinea = 1;
		fstream fs;
		fs.open(Filename);
		if	(fs.fail()){
			cerr << "No Pude Abrir el Archivo " << Filename << endl;
			return 1;
		} else {
			while (!fs.eof()) {
        		getline(fs, Linea);
				Linea += " \n";
				if (Linea.find(SearchPhrase) != -1){
					if (Linea.at(Linea.find(SearchPhrase) + SearchPhrase.size()) == ' ' || Linea.at(Linea.find(SearchPhrase) + SearchPhrase.size()) == ',' || Linea.at(Linea.find(SearchPhrase) + SearchPhrase.size()) == '.'){
						Linea.erase(Linea.size()-2, Linea.size());
						cout << "Linea " << ContLinea << ": " << Linea << endl;
						Occurances++;
					}
				}
				ContLinea++;
			}
			if (Occurances != 0)
				cout << "\nOcurrencias: " << Occurances << endl;
			else
				cout << "No Se Encontraron Ocurrencias." << endl;
		}
		fs.close();
	}
	return 0;
}
