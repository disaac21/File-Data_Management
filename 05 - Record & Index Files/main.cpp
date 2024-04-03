/*
    Title:      Archivos de Índices
    Purpose:    Trabajar con Archivos de Índices Lineales
    Author:     Daniel Isaac Juárez
    Date:       Junio 13 de 2023
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstring>
#include <sstream>
#include <algorithm>

using std::cout;
using std::cin;
using std::endl;
using std::cerr;
using std::ios;
using std::vector;
using std::stringstream;
using std::string;
using std::ofstream;
using std::ifstream;
using std::fstream;

// struct record {
// 	int rowId;
// 	char orderId[14];
// 	char orderDate[10];
// 	char shipDate[10];
// 	char shipMode[14];
// 	char customerId[8];
// 	char customerName[20];
// 	char segment[11];
// 	char country[13];
// 	char city[20];
// 	char state[20];
// 	int postalCode;
// 	char region[7];
// 	char productId[15];
// 	char category[15];
// 	char subCategory[11];
// 	char productName[120];
// 	float sales;
// 	int quantity;
// 	float discount;
// 	float profit;
// };

struct order {
	char orderId[15];
	char orderDate[11];
	char shipDate[11];
	char shipMode[15];
	char customerId[9];
};

struct details {
	char orderId[15];
	char productId[16];
	char sales[11];
	char quantity[4];
	char discount[8];
	char profit[11];
};

struct customers {
	char customerId[9];
	char customerName[21];
	char segment[12];
	char country[14];
	char city[21];
	char state[21];
	char postalCode[6];
	char region[8];
};

struct products {
	char productId[16];
	char category[16];
	char subCategory[12];
	char productName[121];
};

struct indiceorder {
	char llave[15];
	char offset[7];
};

struct indicecustomer {
	char llave[9];
	char offset[7];
};

struct indiceproducts {
	char llave[15];
	char offset[7];
};

void LineByLine (string, vector<string>&);

void fillOrders (vector<string>&);
void showOrders ();
void fillOrdersIdx (vector<string>&);
void showOrdersIdx ();

void fillDetails (vector<string> &infoRecord);
void showDetails ();

void fillCustomers (vector<string> &infoRecord);
void showCustomers ();
void fillCustomersIdx (vector<string>&);
void showCustomersIdx ();

void fillProducts (vector<string> &infoRecord);
void showProducts ();
void fillProductsIdx (vector<string>&);
void showProductsIdx();

int OrdersBinSearch(vector<indiceorder>&, string);
int CustomersBinSearch(vector<indicecustomer>&, string);
int ProductBinSearch(vector<indiceproducts>&, string);

/*
build/indices superstore.csv -normalize
build/indices superstore.csv -create-indices
build/indices -search order=CA-2015-149587
*/

int main(int argc, char* argv[]) {

	// Count Input Parámetros
	if (argc != 3) { // Validación Input cout errors
		switch (argc) {
		case 1:
			cerr << "ERROR -- Faltan 2 Parámetros.\nUse: " << argv[0] << " [superstore.csv / -search] [-normalize / -create-indices / order=CA-2015-149587]" << endl;
			return 1;
			break;
		case 2:
			cerr << "ERROR -- Falta 1 Parámetro.\nUse: " << argv[0] << " " << argv[1] << " [-normalize / -create-indices / order=CA-2015-149587]" << endl;
			return 1;
			break;
		case 3:
			cerr << "ERROR -- Ingresó Demasiados Parámetros.\nUse: " << argv[0] << " [superstore.csv / -search] [-normalize / -create-indices / order=CA-2015-149587]" << endl;
			return 1;
			break;
		default:
			break;
		}
	} else {
		string FlagOne = argv[1]; string FlagTwo = argv[2];

		if (FlagOne.compare("superstore.csv") == 0) {
			
			string superstoreFilename = "./data/superstore.csv"; //Ruta Superstore.csv
			ifstream superstoreFile; //Archivo Lectura
			string superstoreData; //Record Linea a Linea
			vector<string> infoRecord;
			vector<string> ordersIDX;
			vector<string> customersIDX;
			vector<string> productsIDX;
			string writeThis;

			if (FlagTwo.compare("-normalize") == 0) {
				superstoreFile.open(superstoreFilename);
				if (!superstoreFile.fail()){

					while (getline(superstoreFile, superstoreData,'\n')) { //Captura cada Record

						LineByLine(superstoreData, infoRecord);

						fillOrders(infoRecord);
						fillDetails(infoRecord);
						fillCustomers(infoRecord);
						fillProducts(infoRecord);

						infoRecord.clear();
					}
				}
			}

			if (FlagTwo.compare("-create-indices") == 0) {
				fillOrdersIdx(ordersIDX);
				fillCustomersIdx(customersIDX);
				fillProductsIdx(productsIDX);
			}
		}
		if (FlagOne.compare("-search") == 0) {
			size_t posIgual = FlagTwo.find("=");
			if (posIgual != string::npos) {
				string tag1 = FlagTwo.substr(0, posIgual);
				string tag2 = FlagTwo.substr(posIgual + 1);

				vector<indiceorder> IndicesOrdersBusqueda;
				ifstream inputorders("orders.idx", ios::in | ios::binary);
				indiceorder p;
				while (inputorders.read((char*)(&p), sizeof(indiceorder))){
					IndicesOrdersBusqueda.push_back(p);
				}
				inputorders.close();

				vector<indicecustomer> IndicesCustomersBusqueda;
				ifstream inputcustomers("customers.idx", ios::in | ios::binary);
				indicecustomer q;
				while (inputcustomers.read((char*)(&q), sizeof(q))){
					IndicesCustomersBusqueda.push_back(q);
				}
				inputcustomers.close();

				vector<indiceproducts> IndicesProductsBusqueda;
				ifstream inputproducts("products.idx", ios::in | ios::binary);
				indiceproducts d;
				while (inputproducts.read((char*)(&d), sizeof(indiceproducts))){
					IndicesProductsBusqueda.push_back(d);
				}
				inputproducts.close();

				if (tag1.compare("order") == 0) {

					// ----------------------------------------------------------------------------------

					int PosOrdersFound = OrdersBinSearch(IndicesOrdersBusqueda,tag2);
					int Ordersoffset = std::stoi(IndicesOrdersBusqueda.at(PosOrdersFound).offset);
					ifstream ordersdatreading("orders.dat", ios::in | ios::binary);
					ordersdatreading.seekg(Ordersoffset,std::ios_base::beg);
					order m;
					ordersdatreading.read((char*)(&m), sizeof(m));
					cout << "ORDER\n\tID:\t\t" << m.orderId << endl
						<< "\tDate:\t\t" << m.orderDate << endl
						<< "\tShipped:\t" << m.shipDate << endl
						<< "\tship Mode:\t" << m.shipMode << endl
						<< "CUSTOMER\n\tID:\t\t" << m.customerId << endl;
					ordersdatreading.close();
					
					// ----------------------------------------------------------------------------------

					string customersflag(m.customerId);
					int PosCustomersFound = CustomersBinSearch(IndicesCustomersBusqueda,customersflag);
					int Customersoffset = std::stoi(IndicesCustomersBusqueda.at(PosCustomersFound).offset);
					ifstream customerdatreading("customers.dat", ios::in | ios::binary);
					customerdatreading.seekg(Customersoffset,std::ios_base::beg);
					customers l;
					customerdatreading.read((char*)(&l), sizeof(l));
					cout <<"\tName:\t\t" << l.customerName << endl
						<< "\tSegment:\t" << l.segment << endl
						<< "\tCountry:\t" << l.country << endl
						<< "\tCity:\t\t" << l.city << endl
						<< "\tState:\t\t" << l.state << endl
						<< "\tPostal Code:\t" << l.postalCode << endl
						<< "\tRegion:\t\t" << l.region << endl;
					customerdatreading.close();

					// ---------------------------------------------------------------------------------

					vector<details> DetailsBusqueda;
					ifstream detailstream("details.dat", ios::in | ios::binary);
					details det;
					while (detailstream.read(reinterpret_cast<char*>(&det), sizeof(details))) {
						if ((string(det.orderId).compare(FlagTwo)) == 0) {
							DetailsBusqueda.push_back(det);
						}
					}
					detailstream.close();

					ifstream prodstream("products.dat", ios::in | ios::binary);
					float tot = 0;
					for	(size_t i = 0; i < DetailsBusqueda.size(); i++) {
						tot += ((std::stoi(DetailsBusqueda.at(i).sales)*std::stoi(DetailsBusqueda.at(i).quantity)*(1 - std::stoi(DetailsBusqueda.at(i).discount))));
						int pos = ProductBinSearch(IndicesProductsBusqueda, DetailsBusqueda.at(i).orderId);
						indiceproducts idxprod = IndicesProductsBusqueda.at(pos);
						products h;
						prodstream.seekg(std::stoi(idxprod.offset),std::ios_base::beg);
						prodstream.read((char*)(&h), sizeof(products));
						cout << h.productId << "\t" << h.category << "\t" << h.subCategory << "\t" << h.productName << endl;
					}
					prodstream.close();

					
				} else if (tag1.compare("customer") == 0) {

					int PosCustomersFound = CustomersBinSearch(IndicesCustomersBusqueda,tag2);
					int Customersoffset = std::stoi(IndicesCustomersBusqueda.at(PosCustomersFound).offset);
					ifstream customerdatreading("customers.dat", ios::in | ios::binary);
					customerdatreading.seekg(Customersoffset,std::ios_base::beg);
					customers l;
					customerdatreading.read((char*)(&l), sizeof(l));
					cout << "CUSTOMER\n\tID:\t\t" << l.customerId << endl
						<< "\tName:\t\t" << l.customerName << endl
						<< "\tSegment:\t" << l.segment << endl
						<< "\tCountry:\t" << l.country << endl
						<< "\tCity:\t\t" << l.city << endl
						<< "\tState:\t\t" << l.state << endl
						<< "\tPostal Code:\t" << l.postalCode << endl
						<< "\tRegion:\t\t" << l.region << endl;
					customerdatreading.close();


					
				} else if (tag1.compare("product") == 0) {

					int PosProductFound = ProductBinSearch(IndicesProductsBusqueda,tag2);
					if (PosProductFound < 0) {
						cout << "FIX" << endl;
					} else {
						int Productsoffset = std::stoi(IndicesProductsBusqueda.at(PosProductFound).offset);
						ifstream productdatreading("products.dat", ios::in | ios::binary);
						productdatreading.seekg(Productsoffset,std::ios::beg);
						products w;
						productdatreading.read((char*)(&w), sizeof(w));
						cout << "PRODUCT\n\tID:\t\t" << w.productId << endl
							<< "\tCategory:\t\t" << w.category << endl
							<< "\tSub-Category:\t" << w.subCategory << endl
							<< "\tName:\t" << w.productName << endl;
						productdatreading.close();
					}
				}
			}
		}
	}
    return 0;
}

// -----------------------------------------------------------------------

/**
* Line By Line
* Recibe la captura de los registros individuales del archivo .csv para separar sus campos y agregarlos a un vector
* @param Registro como linea. Vector para almacenar campos
* @returns void
*/
void LineByLine (string superstoreData, vector<string> &infoRecord) {
	string fields;
	for (size_t i = 0; i < superstoreData.size(); i++) { //Obtiene la información de cada Field
		if (superstoreData.at(i) == ',' || i+1 == superstoreData.size()) {
			infoRecord.push_back(fields);
			fields = "";
			continue;
		} else if (superstoreData.at(i) == '\"') {
			for (size_t j = i+1; j < superstoreData.size(); j++) {
				if (superstoreData.at(j) == '\"' && superstoreData.at(j+1) == ',') {
					infoRecord.push_back(fields);
					fields = "";
					i=j+2;
					break;
				} else {
					fields += superstoreData.at(j);
				}
			}
		} else {
			fields += superstoreData.at(i);
		}
	}
	fields.clear();
}

// -----------------------------------------------------------------------

/**
* Fill Orders
* Escribe el archivo .dat correspondiente a Orders
* @param Vector donde almacena campos
* @returns void
*/
void fillOrders(vector<string> &infoRecord) {
    order ordenlocal;
    strncpy(ordenlocal.orderId, infoRecord.at(1).c_str(), sizeof(ordenlocal.orderId));
    ordenlocal.orderId[sizeof(ordenlocal.orderId) - 1] = '\0';
    strncpy(ordenlocal.orderDate, infoRecord.at(2).c_str(), sizeof(ordenlocal.orderDate));
    ordenlocal.orderDate[sizeof(ordenlocal.orderDate) - 1] = '\0';
    strncpy(ordenlocal.shipDate, infoRecord.at(3).c_str(), sizeof(ordenlocal.shipDate));
    ordenlocal.shipDate[sizeof(ordenlocal.shipDate) - 1] = '\0';
    strncpy(ordenlocal.shipMode, infoRecord.at(4).c_str(), sizeof(ordenlocal.shipMode));
    ordenlocal.shipMode[sizeof(ordenlocal.shipMode) - 1] = '\0';
    strncpy(ordenlocal.customerId, infoRecord.at(5).c_str(), sizeof(ordenlocal.customerId));
    ordenlocal.customerId[sizeof(ordenlocal.customerId) - 1] = '\0';

	ofstream ordersdat("orders.dat", std::ios::binary | std::ios::app);
	ordersdat.write(reinterpret_cast<const char*>(&ordenlocal),sizeof(ordenlocal));
	ordersdat.close();
}

/**
* Show Orders
* Muestra la informacion del archivo .dat correspondiente a Orders
* @param 
* @returns void
*/
void showOrders () {
	ifstream input("orders.dat", ios::in | ios::binary);
	order p;
	while (input.read((char*)(&p), sizeof(p))){
		cout << "orderId:  " << p.orderId << endl
		<< "orderDate:  " << p.orderDate << endl
		<< "shipDate:   " << p.shipDate << endl
		<< "shipMode:   " << p.shipMode << endl
		<< "customerId:   " << p.customerId << endl;
	}
	input.close();
}


/**
* Fill OrdersIDX
* Llena y Ordena los indices para los Orders
* @param vector de string para trabajar previo a converrtir a indice
* @returns void
*/
void fillOrdersIdx (vector<string> &ordersIDX) {
	ifstream input("orders.dat", ios::in | ios::binary);
	int contoffset = 0;
	order o;
	while (input.read((char*)(&o), sizeof(order))) {
		string aid = o.orderId;
		ordersIDX.push_back(aid + " " + std::to_string(contoffset * sizeof(order)));
		contoffset++;
	}
	input.close();

	sort(ordersIDX.begin(),ordersIDX.end());

	vector<indiceorder> IndicesOrders;
	for (size_t i = 0; i < ordersIDX.size(); i++){
		indiceorder idxorder;
		size_t posIgual = ordersIDX.at(i).find(" ");
		if (posIgual != string::npos) {
			string llave = ordersIDX.at(i).substr(0, posIgual);
			string offset = ordersIDX.at(i).substr(posIgual + 1);
			cout << "llave: " << llave << " offset: " << offset << endl;

			strncpy(idxorder.llave, llave.c_str(), sizeof(idxorder.llave));
			idxorder.llave[sizeof(idxorder.llave) - 1] = '\0';
			strncpy(idxorder.offset, offset.c_str(), sizeof(idxorder.offset));
			idxorder.offset[sizeof(idxorder.offset) - 1] = '\0';
			IndicesOrders.push_back(idxorder);
		}
	}

	ofstream ordersdat("orders.idx", std::ios::binary | std::ios::app);
	for (size_t i = 0; i < IndicesOrders.size(); i++){
		ordersdat.write(reinterpret_cast<const char*>(&IndicesOrders.at(i)),sizeof(indiceorder));
	}
	ordersdat.close();
}

/**
* Show Orders
* Muestra la informacion del archivo .idx correspondiente a Orders
* @param 
* @returns void
*/
void showOrdersIdx() {
	ifstream input("orders.idx", ios::in | ios::binary);
	indiceorder p;
	while (input.read((char*)(&p), sizeof(indiceorder))){
		cout << "llave:  " << p.llave << endl << "offset:  " << p.offset << endl;
	}
	input.close();
}

// -----------------------------------------------------------------------

/**
* Fill Details
* Escribe el archivo .dat correspondiente a Details
* @param Vector donde almacena campos
* @returns void
*/
void fillDetails (vector<string> &infoRecord) {
	details detailslocal;
	strncpy(detailslocal.orderId, infoRecord.at(1).c_str(), sizeof(detailslocal.orderId) - 1);
	detailslocal.orderId[sizeof(detailslocal.orderId) - 1] = '\0';
	strncpy(detailslocal.productId, infoRecord.at(13).c_str(), sizeof(detailslocal.productId) - 1);
	detailslocal.productId[sizeof(detailslocal.productId) - 1] = '\0';
	strncpy(detailslocal.sales, infoRecord.at(17).c_str(), sizeof(detailslocal.sales) - 1);
	detailslocal.sales[sizeof(detailslocal.sales) - 1] = '\0';
	strncpy(detailslocal.quantity, infoRecord.at(18).c_str(), sizeof(detailslocal.quantity) - 1);
	detailslocal.quantity[sizeof(detailslocal.quantity) - 1] = '\0';
	strncpy(detailslocal.discount, infoRecord.at(19).c_str(), sizeof(detailslocal.discount) - 1);
	detailslocal.discount[sizeof(detailslocal.discount) - 1] = '\0';
	strncpy(detailslocal.profit, infoRecord.at(20).c_str(), sizeof(detailslocal.profit) - 1);
	detailslocal.profit[sizeof(detailslocal.profit) - 1] = '\0';

	ofstream ordersdat("details.dat", std::ios::binary | std::ios::app);
	ordersdat.write(reinterpret_cast<const char*>(&detailslocal),sizeof(detailslocal));
	ordersdat.close();
}

/**
* Show Details
* Muestra la informacion del archivo .dat correspondiente a Details
* @param 
* @returns void
*/
void showDetails () {
	ifstream input("details.dat", ios::in | ios::binary);
	details p;
	while (input.read((char*)(&p), sizeof(p))){
		cout << "discount:  " << p.discount << endl
		<< "orderId:  " << p.orderId << endl
		<< "productId:   " << p.productId << endl
		<< "profit:   " << p.profit << endl
		<< "quantity:   " << p.quantity << endl
		<< "sales:   " << p.sales << endl;
	}
}

// -----------------------------------------------------------------------

/**
* Fill Customers
* Escribe el archivo .dat correspondiente a Customers
* @param Vector donde almacena campos
* @returns void
*/
void fillCustomers (vector<string> &infoRecord) {
	customers customerslocal;
	strncpy(customerslocal.customerId, infoRecord.at(5).c_str(), sizeof(customerslocal.customerId) - 1);
	customerslocal.customerId[sizeof(customerslocal.customerId) - 1] = '\0';

	strncpy(customerslocal.customerName, infoRecord.at(6).c_str(), sizeof(customerslocal.customerName) - 1);
	customerslocal.customerName[sizeof(customerslocal.customerName) - 1] = '\0';

	strncpy(customerslocal.segment, infoRecord.at(7).c_str(), sizeof(customerslocal.segment) - 1);
	customerslocal.segment[sizeof(customerslocal.segment) - 1] = '\0';

	strncpy(customerslocal.country, infoRecord.at(8).c_str(), sizeof(customerslocal.country) - 1);
	customerslocal.country[sizeof(customerslocal.country) - 1] = '\0';

	strncpy(customerslocal.city, infoRecord.at(9).c_str(), sizeof(customerslocal.city) - 1);
	customerslocal.city[sizeof(customerslocal.city) - 1] = '\0';

	strncpy(customerslocal.state, infoRecord.at(10).c_str(), sizeof(customerslocal.state) - 1);
	customerslocal.state[sizeof(customerslocal.state) - 1] = '\0';

	strncpy(customerslocal.postalCode, infoRecord.at(11).c_str(), sizeof(customerslocal.state) - 1);
	customerslocal.postalCode[sizeof(customerslocal.postalCode) - 1] = '\0';

	strncpy(customerslocal.region, infoRecord.at(12).c_str(), sizeof(customerslocal.region) - 1);
	customerslocal.region[sizeof(customerslocal.region) - 1] = '\0';

	ofstream customerdat("customers.dat", std::ios::binary | std::ios::app);
	customerdat.write(reinterpret_cast<const char*>(&customerslocal),sizeof(customerslocal));
	customerdat.close();
}

/**
* Show Customers
* Muestra la informacion del archivo .dat correspondiente a Customers
* @param 
* @returns void
*/
void showCustomers () {
	ifstream input("customers.dat", ios::in | ios::binary);
	customers p;
	while (input.read((char*)(&p), sizeof(p))){
		cout << "customerId:  " << p.customerId << endl
		<< "customerName:  " << p.customerName << endl
		<< "segment:   " << p.segment << endl
		<< "country:   " << p.country << endl
		<< "city:   " << p.city << endl
		<< "state:   " << p.state << endl
		<< "postalCode:   " << p.postalCode << endl
		<< "region:   " << p.region << endl;
	}
}

/**
* Fill CustomersIDX
* Llena y Ordena los indices para los Customers
* @param vector de string para trabajar previo a converrtir a indice
* @returns void
*/
void fillCustomersIdx (vector<string> &customersIDX) {
	ifstream input("customers.dat", ios::in | ios::binary);
	int contoffset = 0;
	customers o;
	while (input.read((char*)(&o), sizeof(customers))) {
		string aid = o.customerId;
		customersIDX.push_back(aid + " " + std::to_string(contoffset * sizeof(customers)));
		contoffset++;
	}

	input.close();

	sort(customersIDX.begin(),customersIDX.end());

	vector<indicecustomer> IndicesCustomers;
	for (size_t i = 0; i < customersIDX.size(); i++){
		indicecustomer idxcustomer;
		size_t posIgual = customersIDX.at(i).find(" ");
		if (posIgual != string::npos) {
			string llave = customersIDX.at(i).substr(0, posIgual);
			string offset = customersIDX.at(i).substr(posIgual + 1);
			cout << "llave: " << llave << " offset: " << offset << endl;

			strncpy(idxcustomer.llave, llave.c_str(), sizeof(idxcustomer.llave));
			idxcustomer.llave[sizeof(idxcustomer.llave) - 1] = '\0';
			strncpy(idxcustomer.offset, offset.c_str(), sizeof(idxcustomer.offset));
			idxcustomer.offset[sizeof(idxcustomer.offset) - 1] = '\0';
			IndicesCustomers.push_back(idxcustomer);
		}
	}


	ofstream ordersdat("customers.idx", std::ios::binary | std::ios::app);
	for (size_t i = 0; i < IndicesCustomers.size(); i++){
		ordersdat.write(reinterpret_cast<const char*>(&IndicesCustomers.at(i)),sizeof(indicecustomer));
	}
	ordersdat.close();
}

/**
* Show Customers
* Muestra la informacion del archivo .idx correspondiente a Customers
* @param 
* @returns void
*/
void showCustomersIdx() {
	ifstream input("customers.idx", ios::in | ios::binary);
	indicecustomer p;
	while (input.read((char*)(&p), sizeof(indicecustomer))){
		cout << "llave:  " << p.llave << endl << "offset:  " << p.offset << endl;
	}
	input.close();
}

// -----------------------------------------------------------------------

/**
* Fill Products
* Escribe el archivo .dat correspondiente a Products
* @param Vector donde almacena campos
* @returns void
*/
void fillProducts (vector<string> &infoRecord) {
	products productslocal;
	strncpy(productslocal.productId, infoRecord.at(13).c_str(), sizeof(productslocal.productId) - 1);
	productslocal.productId[sizeof(productslocal.productId) - 1] = '\0';
    strncpy(productslocal.category, infoRecord.at(14).c_str(), sizeof(productslocal.category) - 1);
	productslocal.category[sizeof(productslocal.category) - 1] = '\0';
    strncpy(productslocal.subCategory, infoRecord.at(15).c_str(), sizeof(productslocal.subCategory) - 1);
	productslocal.subCategory[sizeof(productslocal.subCategory) - 1] = '\0';
    strncpy(productslocal.productName, infoRecord.at(16).c_str(), sizeof(productslocal.productName) - 1);
	productslocal.productName[sizeof(productslocal.productName) - 1] = '\0';

	ofstream ordersdat("products.dat", std::ios::binary | std::ios::app);
	ordersdat.write(reinterpret_cast<const char*>(&productslocal),sizeof(productslocal));
	ordersdat.close();
}

/**
* Show Products
* Muestra la informacion del archivo .dat correspondiente a Products
* @param 
* @returns void
*/
void showProducts () {
	ifstream input("products.dat", ios::in | ios::binary);
	products p;
	while (input.read((char*)(&p), sizeof(p))){
		cout << "prodId:  " << p.productId << endl
		<< "category:  " << p.category << endl
		<< "category:   " << p.subCategory << endl
		<< "prodName:   " << p.productName << endl;
	}
}

/**
* Fill ProductsIDX
* Llena y Ordena los indices para los Products
* @param vector de string para trabajar previo a converrtir a indice
* @returns void
*/
void fillProductsIdx (vector<string> &productsIDX) {
	ifstream input("products.dat", ios::in | ios::binary);
	int contoffset = 0;
	products o;
	while (input.read((char*)(&o), sizeof(products))) {
		string aid = o.productId;
		productsIDX.push_back(aid + " " + std::to_string(contoffset * sizeof(products)));
		contoffset++;
	}

	input.close();

	sort(productsIDX.begin(),productsIDX.end());

	vector<indiceproducts> IndicesProducts;
	for (size_t i = 0; i < productsIDX.size(); i++){
		indiceproducts idxproducts;
		size_t posIgual = productsIDX.at(i).find(" ");
		if (posIgual != string::npos) {
			string llave = productsIDX.at(i).substr(0, posIgual);
			string offset = productsIDX.at(i).substr(posIgual + 1);
			cout << "llave: " << llave << " offset: " << offset << endl;

			strncpy(idxproducts.llave, llave.c_str(), sizeof(idxproducts.llave));
			idxproducts.llave[sizeof(idxproducts.llave) - 1] = '\0';
			strncpy(idxproducts.offset, offset.c_str(), sizeof(idxproducts.offset));
			idxproducts.offset[sizeof(idxproducts.offset) - 1] = '\0';
			IndicesProducts.push_back(idxproducts);
		}
	}

	ofstream ordersdat("products.idx", std::ios::binary | std::ios::app);
	for (size_t i = 0; i < IndicesProducts.size(); i++){
		ordersdat.write(reinterpret_cast<const char*>(&IndicesProducts.at(i)),sizeof(products));
	}
	ordersdat.close();
}

/**
* Show ProductsIDX
* Muestra la informacion del archivo .idx correspondiente a Products
* @param 
* @returns void
*/
void showProductsIdx() {
	ifstream input("orders.idx", ios::in | ios::binary);
	indiceproducts p;
	while (input.read((char*)(&p), sizeof(indiceproducts))){
		cout << "llave:  " << p.llave << endl << "offset:  " << p.offset << endl;
	}
	input.close();
}

// -----------------------------------------------------------------------

/**
* Orders Bin Search
* Realiza la Busqueda Binaria para Orders
* @param 
* @returns void
*/
int OrdersBinSearch(vector<indiceorder>& idxordervector, string keytofind) {
    int sideone = 0;
    int sidetwo = idxordervector.size() - 1;
    while (sideone <= sidetwo) {
        int halfsplit = sideone + (sidetwo - sideone) / 2;
        if (idxordervector.at(halfsplit).llave == keytofind) {
            return halfsplit;
        }
        if (idxordervector.at(halfsplit).llave < keytofind) {
            sideone = halfsplit + 1;
        } else {
            sidetwo = halfsplit - 1;
        }
    }
    return -1;
}

/**
* Customers Bin Search
* Realiza la Busqueda Binaria para Customers
* @param 
* @returns void
*/
int CustomersBinSearch(vector<indicecustomer>& idcustomervector, string keytofind) {
    int sideone = 0;
    int sidetwo = idcustomervector.size() - 1;
    while (sideone <= sidetwo) {
        int halfsplit = sideone + (sidetwo - sideone) / 2;
        if (idcustomervector.at(halfsplit).llave == keytofind) {
            return halfsplit;
        }
        if (idcustomervector.at(halfsplit).llave < keytofind) {
            sideone = halfsplit + 1;
        } else {
            sidetwo = halfsplit - 1;
        }
    }
    return -1;
}

/**
* Product Bin Search
* Realiza la Busqueda Binaria para Product
* @param 
* @returns void
*/
int ProductBinSearch(vector<indiceproducts>& idxproductsvector, string keytofind) {
    int sideone = 0;
    int sidetwo = idxproductsvector.size() - 1;
    while (sideone <= sidetwo) {
        int halfsplit = sideone + (sidetwo - sideone) / 2;
        if (idxproductsvector.at(halfsplit).llave == keytofind) {
            return halfsplit;
        }
        if (idxproductsvector.at(halfsplit).llave < keytofind) {
            sideone = halfsplit + 1;
        } else {
            sidetwo = halfsplit - 1;
        }
    }
    return -1;
}