#include <iostream>
#include <fstream>
#include <string>

using std::cout;
using std::string;
using std::endl;
using std::cin;
using std::cerr;
using std::ofstream;
using std::ifstream;

int main(int argc, char* argv[]){
	// Dependiendo de la bandera...
	// -write
	// Lee numeros del usuario y los guarda en un archivo
	// -read
	// Lee numeros del archivo y los muestra en pantalla
	if (argc != 3) {
		cerr << "Parametros Incorrectos, Uso: " << endl;
		cerr << argv[0] << " -flag archivo" << endl;
		cerr << "flag puede ser read o write" << endl;
		return 1;
	}

	string flag = argv[1];
	string filename = argv[2];
	if (flag != "-read" && flag != "-write"){
		cerr << "Bandera Inválida" << endl;
		cerr << argv[0] << " -flag archivo" << endl;
		cerr << "flag puede ser read o write" << endl;
	}
	if	(flag == "-write") {
		//cout << "write al archivo " << filename << endl;
		ofstream out(filename);
		if	(out.fail()){
			cerr << "No Pude Abrir el Archivo " << filename << endl;
			return 1;
		}
		int number;
		while (cin >> number) {
			out << number << endl;
		}
		out.close();
	} else {
		//cout << "read del archivo " << filename << endl;
		ifstream in(filename);
		if	(in.fail()){
			cerr << "No Pude Abrir el Archivo " << filename << endl;
			return 1;
		}
		int number;
		while (in >> number){
			cout << number << endl;
		}
		in.close();
	}

	return 0;
}
