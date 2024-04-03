[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/2kuG0uAN)
# Archivo de Registros

## Objetivo
La meta de esta tarea es aprender y practicar las operaciones de lectura y escritura a un archivo de registros.

## Descripción

La estructura del registro para representar un producto será la siguiente:

* `id` es un entero.
* `name` es el nombre del producto, representado por una cadena, es posible que el nombre del producto tenga espacios!.
* `category` es la categoría del producto. Puede tener los valores enteros del 0 al 9.
* `price` es el precio del producto, representado por un número real con dos posiciones decimales.
* `aisle` es el número de pasillo donde se guarda el producto, se representa con un número entero de 3 positiones.
* `bin` dentro de los pasillos hay diferentes "bin"s que indican el lugar exacto donde se guarda el producto, se representa con un número entero de 3 positiones.

Su programa deberá de recibir desde la línea de comando el nombre un archivo CSV que contiene al menos 10 registros con el formato descrito anteriormente. Además del nombre del archivo recibirá una bandera que le indicará a su programa qué es lo que quiere hacer. Las posibles banderas son `-json` y `-fixed`.

## Cómo se correrá su programa

* `./convert data.csv -jason` En este caso, lo que su programa hará es leer el archivo `data.csv` y producirá un archivo llamado `data.json` que contendrá los mismos datos que el archivo original pero en formato JSON.
* `./convert data.csc -fixed` En este caso, lo que su programa hará es leer el archivo `data.csv` y producira un archivo llamado `data.txt` que contendrá los mismos datos que el archivo origina pero en formato de registros con campos de longitud fija. ¿Qué longitud debería usar para los campos?


## Reto Adicional
Agregar banderas al programa para poder convertir los archivos JSON y los de registros de longitud fija a CSV.

## Ejemplos de datos

|id|name|category|price|aisle|bin|
|--|----|--------|-----|-----|---|
|133|Red Pen|0|0.99|121|044|
|4478|Razor|4|2.59|664|000|

El archivo CSV se vería así:
```
id,name,category,price,aisle,bin
133,Red Pen,0,0.99,121,044
4478,Razor,4,2.59,664,000

```

El archivo JSON se vería así:
```json
[
    {"id":133, "name": "Red Pen", "category":0, "price":0.99, "aisle":121, "bin":44},
    {"id":4478, "name": "Razor", "category":4, "price":2.59, "aisle":664, "bin":0}
]
```

El archivo de registros de longitud fija se vería así (podría verse un poco diferente dependiendo de... ¿?):
```
id    name      categoryprice  aislebin
133   Red Pen   0       0.99   121  044
4478  Razor     4       2.59   664  000
```

