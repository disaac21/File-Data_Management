[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/ohFLhj-L)
[![Open in Codespaces](https://classroom.github.com/assets/launch-codespace-7f7980b617ed060a017424585567c406b6ee15c891e84e1186181d67ecf80aa0.svg)](https://classroom.github.com/open-in-codespaces?assignment_repo_id=11195412)
# Trabajando con Archivos de Registros

## Objetivo
Leer del archivo de órdenes y buscar los registros con cierto ID, y reportar la suma de cierto campo.

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

## La meta de tu programa

Tu programa recibirá dos argumentos desde la línea de comando: `./basic-search ./data/superstore.csv CA-2015-149587`, los parámetros representan el archivo que se desea procesar y el ID que se quiere analizar. En este caso, el registro con ID CA-2015-149587 tiene tres registros en el archivo:
```
93,CA-2015-149587,1/31/2015,2/5/2015,Second Class,KB-16315,Karl Braun,Consumer,United States,Minneapolis,Minnesota,55407,Central,OFF-PA-10003177,Office Supplies,Paper,Xerox 1999,12.96,2,0,6.2208
94,CA-2015-149587,1/31/2015,2/5/2015,Second Class,KB-16315,Karl Braun,Consumer,United States,Minneapolis,Minnesota,55407,Central,FUR-FU-10003799,Furniture,Furnishings,"Seth Thomas 13 1/2"" Wall Clock",53.34,3,0,16.5354
95,CA-2015-149587,1/31/2015,2/5/2015,Second Class,KB-16315,Karl Braun,Consumer,United States,Minneapolis,Minnesota,55407,Central,OFF-BI-10002852,Office Supplies,Binders,Ibico Standard Transparent Covers,32.96,2,0,16.1504
```

Tu programa deberá analizar estos registros, calcular el total de la venta y luego mostrar en pantalla los campos: Order ID, Product Name, Sales, Quantity, Discount de los registros y el total de la venta de la siguiente manera:

```
prompt$ make
prompt$ ./build/basic-search ./data/superstore.csv CA-2015-149587
CA-2015-149587,Xerox 1999,12.96,2,0
CA-2015-149587,"Seth Thomas 13 1/2"" Wall Clock",53.34,3,0
CA-2015-149587,Ibico Standard Transparent Covers,32.96,2,0

Total Records: 3
Total Sale: 251.86
```

> NOTA
> Después de ejecutar `make` el programa ejecutable `basic-search` quedará guardado en el directorio `build`.

## ¿Cómo calcular el total?
Nota los últimos campos del registro: `sales`, `quantity`, y `discount`. Usando estos campos calcularás el total. En el caso de los tres registros mostrados arriba los valores para estos campos son:

|sales|quantity|discount|
|-----|--------|--------|
|12.96|2       |0       |
|53.34|3       |0       |
|32.96|2       |0       |

La fórmula para calcular el total de estos registros sería:
$Total = (12.96 * 2 * (1 - 0)) + (53.34 * 3 * (1 - 0)) + (35.96 * 2 * (1 - 0))$

En el caso de **estos** registros, el descuento es cero, es por esa razón que en los tres cálculos se mira $(1 - 0)$.

## Funcionamiento del programa
Tu programa no deberá cargar el archivo completo a memoria, tú buscarás los registros con el ID proporcionado uno por uno leyendo directamente del archivo. 

