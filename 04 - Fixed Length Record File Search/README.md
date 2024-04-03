[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/mvSNhB8w)
[![Open in Codespaces](https://classroom.github.com/assets/launch-codespace-7f7980b617ed060a017424585567c406b6ee15c891e84e1186181d67ecf80aa0.svg)](https://classroom.github.com/open-in-codespaces?assignment_repo_id=11144378)
# Trabajando con Archivos de Registros

Este programa tendrá dos funciones:
1. Leerá el archivo de datos `superstore` que actualmente es un archivo de registros de tamaño variable con delimitadores de campo y de registros. El programa lo convertirá en un archivo de registros de tamaño fijo y campos de tamaño fijo. El usuario determinará el tamaño de cada uno de los campos.
2. El usuario proporciona el RRN de un registro y el programa leerá el archivo y mostrará la información de dicho registro en pantalla mostrando los valores de cada campo línea por línea.

## Datos

A continuación se describe el archivo de datos que procesará para este programa.

Los datos fueron bajados de **kaggle** un sitio web que se especializa en minería de datos y hospeda varios conjuntos de datos que son utilizados para probar algoritmos de aprendizaje de máquina. En el caso de esta asignación utilizaremos el **Superstore Dataset**. Este set de datos contiene información sobre las órdenes hechas a una tienda. El formato del archivo es CSV, el primer renglón del archivo contiene el nombre de cada uno de los campos.

Los campos de este dataset son:

| Campo         | Descripción                              |
|---------------|------------------------------------------|
| Row ID        | Unique ID for each row.                  |
| Order ID      | Unique Order ID for each Customer.       |
| Order Date    | Order Date of the product.               |
| Ship Date     | Shipping Date of the Product.            |
| Ship Mode     | Shipping Mode specified by the Customer. |
| Customer ID   | Unique ID to identify each Customer.     |
| Customer Name | Name of the Customer.                    |
| Segment       | The segment where the Customer belongs.  |
| Country       | Country of residence of the Customer.    |
| City          | City of residence of of the Customer.    |
| State         | State of residence of the Customer.      |
| Postal Code   | Postal Code of every Customer.           |
| Region        | Region where the Customer belong.        |
| Product ID    | Unique ID of the Product.                |
| Category      | Category of the product ordered.         |
| Sub-Category  | Sub-Category of the product ordered.     |
| Product Name  | Name of the Product                      |
| Sales         | Sales of the Product.                    |
| Quantity      | Quantity of the Product.                 |
| Discount      | Discount provided.                       |
| Profit        | Profit/Loss incurred.                    |

Un renglón ejemplo:
```
1,CA-2016-152156,11/8/2016,11/11/2016,Second Class,CG-12520,Claire Gute,Consumer,United States,Henderson,Kentucky,42420,South,FUR-BO-10001798,Furniture,Bookcases,Bush Somerset Collection Bookcase,261.96,2,0,41.9136
```
Nótese que cada campo es separado por coma.

> NOTA
> Los datos de algunos campos podrían estar entre comillas, y entre estas comillas puede que hayan comas. Otros datos podrían tener comillas dentro del campo entre comillas, y hasta podrían haber más comillas consecutivas.

Un registro con comas en datos entre comillas:
```
6,CA-2014-115812,6/9/2014,6/14/2014,Standard Class,BH-11710,Brosina Hoffman,Consumer,United States,Los Angeles,California,90032,West,FUR-FU-10001487,Furniture,Furnishings,"Eldon Expressions Wood and Plastic Desk Accessories, Cherry Wood",48.86,7,0,14.1694
```

Un registro con comillas consecutivas:
```
72,CA-2017-114440,9/14/2017,9/17/2017,Second Class,TB-21520,Tracy Blumstein,Consumer,United States,Jackson,Michigan,49201,Central,OFF-PA-10004675,Office Supplies,Paper,"Telephone Message Books with Fax/Mobile Section, 5 1/2"" x 3 3/16""",19.05,3,0,8.763
```
Una descripción detallada de los datos se puede encontrar [aquí](https://www.kaggle.com/datasets/vivek468/superstore-dataset-final).


## Meta del Programa
Tu programa recibirá argumentos de línea de comando que determinarán lo que tu programa hará.
* `./build/intermediate-search ./data/superstore.csv -convert length-file.txt` En este caso el programa entenderá que debe convertir el archivo `superstore.csv` a un archivo de registros de tamaño fijo, esto es indicado por la bandera `-convert` el siguiente parámetro es el nombre de un archivo que contiene las longitudes de los campos.
* `./build/intermediate-search ./data/superstore.fixed -retrieve 12` En este caso el programa entenderá que debe recuperar el registro #12 (Registro con RRN 12) del archivo `superstore.fixed` y mostrarlo en pantalla de manera formateada.

### Convertir de Archivo Delimitado a Archivo de Registros de Tamaño Fijo
Dado el siguiente registro tomado del archivo `superstore.csv`:
```
93,CA-2015-149587,1/31/2015,2/5/2015,Second Class,KB-16315,Karl Braun,Consumer,United States,Minneapolis,Minnesota,55407,Central,OFF-PA-10003177,Office Supplies,Paper,Xerox 1999,12.96,2,0,6.2208
```

Y digamos que nuestro archivo `length-file.txt` tiene la siguiente información:
```
Row ID        4  INT
Order ID      14 CHAR
Order Date    10 DATE
Ship Date     10 DATE
Ship Mode     14 CHAR
Customer ID   8  CHAR
Customer Name 20 CHAR
Segment       11 CHAR
Country       13 CHAR
City          20 CHAR
State         20 CHAR
Postal Code   5  INT
Region        7  CHAR
Product ID    15 CHAR
Category      15 CHAR
Sub-Category  11 CHAR
Product Name  120CHAR
Sales         10 NUMBER(7.2)
Quantity      3  INT
Discount      4  NUMBER(4.2)
Profit        10 NUMBER(7.2)
```
Por ejemplo: `Row ID        4  INT` significa que el primer campo del archivo `superstore` se llama `Row ID` que la longitud del campo es de 4 posiciones y que almacenará un número entero.

Note también los campos "reales" que se describen como `NUMBER(4.2)`. Esto significa que es un número que utiliza 4 espacios, y de los 4, dos son para los decimales. Ejemplos de números que corresponden a este formato: `1.11`, `0.15`, `0.01`. El punto es incluido en los cuatro espacios disponibles para este campo.

Entonces, tu programa leerá este archivo `length-file.txt` y a partir de la información sobre la longitud de cada campo convertirá el archivo con formato CSV al formato de archivo de campos y registros de longitud fija.

### Buscar un Registro
Una vez tu programa haya convertido el archivo puedes proceder a recuperar un registro con un RRN provisto por el usuario. Algunas preguntas que deberás contestar para lograr esto:
1. ¿Cuál es la longitud del registro?
2. ¿Cuál es el offset al que debes ir cuando el usuario quiera el registro con RRN 12?
3. ¿Qué función (o funciones) te convendrá utilizar para este fin?


## Compilar y Correr tu Programa
Para compilar te estoy proveyendo de un `makefile` así que sólo tiene que ejecutar el comando `make` y tu programa será compilado.

```
prompt$ make
prompt$ ./build/intermediate-search ./data/superstore.csv -convert length-file.txt
prompt$ ./build/intermediate-search ./data/superstore.fixed -retrieve 12
Row ID          12
Order ID        CA-2014-115812
Order Date      6/9/14
Ship Date       6/14/14
Ship Mode       Standard Class
Customer ID     BH-11710
Customer Name   Brosina Hoffman
Segment         Consumer
Country         United States
City            Los Angeles
State           California
Postal Code     90032
Region          West
Product ID      TEC-PH-10002033
Category        Technology
Sub-Category    Phones
Product Name    Konftel 250 Conference†phone†- Charcoal black
Sales           911.42
Quantity        4
Discount        0.2
Profit          68.36
```

El ejemplo mostrado convierte el archivo `superstore.csv` en el archivo `superstore.fixed`.

Tu programa no deberá cargar el archivo completo a memoria, tú buscarás los registros con el ID proporcionado uno por uno leyendo directamente del archivo. 

## Integridad Académica
Al igual que todas las tareas en este curso, este programa deberá ser desarrollado de manera individual. El 100% del código que entregues deberá ser hecho por tí, no copies código de internet, ni utilices herramientas de inteligencia artificial para el desarrollo de tus tareas en este curso.

## Expectativas del Programa

* El programa debe compilar sin "warnings."
* El programa no deberá tener errores de tiempo de ejecución (no deberá "tronar").
* El programa no deberá tener fugas de memoria (memory leaks). _Aplica cuando haya uso de memoria dinámica_
* El programa deberá seguir buenas prácticas de programación
  * Indentación de código
  * Uso consistente de {} 
  * Nombres significativos de identificadores (variables y funciones)
  * Convenciones para los identificadores
    * Variables: camelCase, ejemplo: `rowId`
    * Funciones: CamelCase, ejemplo: `SplitRow`
    * La primera palabra del nombre de una función normalmente debe ser un verbo
    * Datos miembro de clases usan el _ comp prefijo, ejemplo: `_root`
  * *NO* usar variables globales
  * Siempre que pases instancias de clases o de structs asegúrate de pasarlos por referencia y `const` cuando sea apropiado.
  * El código debe ser limpio. Remueve el código que hayas comentado, y utiliza lenguaje profesional en tus variables y comentarios.
  * No debes usar `cin` o `cout` en funciones/métodos que no tienen el objetivo de hacer I/O de la consola.

### Documentación de tu Programa

Al principio de cada archivo con código fuente que hayas elaborado deberás colocar comentarios usando el formato que se muestra a continuación:
```
/*
    Title:      Lab 99 – spreadsheet.cpp
    Purpose:    implement the spreadsheet class methods
    Author:     John Wick
    Date:       January 6, 2020
*/
```

Tus funciones deberán estar comentadas también, usando el siguiente formato:
```
/**
* Add Element
* It adds an element to the set. There should not be repeated elements on the set.
* The set should always be sorted. Remember to consider all the possible cases when
* adding to the list:
* - The List is empty
* - The List is not empty and you want to insert before the _head
* @param element the number you want to insert to the set
* @returns true when the element was added to the set, false if the element was not added
* (the element was already on the set)
*/

bool AddElement(double element){
...
}
```
