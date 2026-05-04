
---

# Proyecto Nro. 1: Hoja de Cálculo Simple (Matrices Dispersas)

[cite_start]Este proyecto implementa una aplicación de hoja de cálculo funcional utilizando **matrices dispersas** representadas mediante **listas enlazadas cruzadas** (cross-linked lists)[cite: 8, 11]. [cite_start]La estructura permite almacenar eficientemente solo las celdas que contienen información, optimizando el uso de memoria frente a matrices densas tradicionales[cite: 9, 10].

## 1. Instrucciones de Compilación y Ejecución

Para aquellos que no utilicen **Qt Creator**, el proyecto puede compilarse y ejecutarse mediante **CMake** desde la terminal. [cite_start]Se requiere un compilador compatible con **C++17** o superior y las librerías de **Qt 6** instaladas[cite: 54, 65].

### Pasos para compilar:
1.  **Clonar o descargar** el repositorio en una carpeta local.
2.  Abrir una terminal en la carpeta raíz del proyecto.
3.  Crear y acceder a una carpeta de construcción para mantener el código limpio:
    ```bash
    mkdir build
    cd build
    ```
4.  Configurar el proyecto con CMake:
    ```bash
    cmake ..
    ```
    *(Nota: Si Qt no está en la ruta del sistema, es posible que debas especificarla con `-DCMAKE_PREFIX_PATH=C:/Ruta/A/Qt/version/mingw_64`)*.
5.  Compilar el proyecto:
    ```bash
    make
    # o si usas MinGW en Windows:
    mingw32-make
    ```
6.  Ejecutar la aplicación:
    ```bash
    ./HojaDeCalculo
    ```

---

## 2. Descripción del Backend (`Spreadsheet.h`)

[cite_start]La lógica principal reside en la clase `Spreadsheet`, la cual gestiona una estructura dinámica de nodos enlazados[cite: 25, 102].

### Estructura de Datos: Lista Enlazada Cruzada
[cite_start]Cada celda con contenido se representa como un `CellNode` que almacena su fila, columna y valor (texto o fórmula)[cite: 26]. [cite_start]Lo fundamental es que cada nodo posee dos punteros de dirección[cite: 11, 26]:
* `nextInRow`: Apunta a la siguiente celda ocupada en la **misma fila**.
* `nextInCol`: Apunta a la siguiente celda ocupada en la **misma columna**.

[cite_start]La clase mantiene dos vectores de cabeceras (`rowHeaders` y `colHeaders`) que actúan como puntos de entrada para recorrer cualquier fila o columna activa[cite: 27].

### Métodos Principales:

* [cite_start]**`insertarCelda(row, col, valor)`**: Busca si la celda ya existe para actualizarla[cite: 34, 36]. Si es nueva, crea el nodo y recorre las listas correspondientes para insertarlo de forma ordenada (por columna en las filas y por fila en las columnas), asegurando que los punteros vecinos se actualicen correctamente.
* [cite_start]**`consultarCelda(row, col)`**: Recorre la fila indicada desde su cabecera hasta encontrar la columna buscada[cite: 35]. Si llega a un índice mayor o al final de la lista, retorna una cadena vacía.
* [cite_start]**`evaluarCelda(row, col)`**: Es el motor de cálculo[cite: 48]. [cite_start]Si el contenido empieza con `=`, el método llama a un evaluador de fórmulas que procesa la expresión, resuelve las referencias (ej. `A1`) y ejecuta operaciones aritméticas básicas[cite: 49].
* [cite_start]**`eliminarCelda(row, col)`**: Es la operación más crítica[cite: 37, 52]. [cite_start]Localiza el nodo y, lo más importante, identifica los nodos **anteriores** tanto en la fila como en la columna para "puentear" los punteros hacia el siguiente nodo, liberando luego la memoria[cite: 37].
* [cite_start]**`eliminarFila(row)` / `eliminarColumna(col)`**: Recorre toda la línea especificada y desconecta cada nodo encontrado de su lista perpendicular antes de eliminarlo[cite: 39, 40].
* [cite_start]**Operaciones de Rango (`suma`, `promedio`, `max`, `min`)**: Utilizan un doble bucle lógico que recorre las filas dentro del rango y, en cada una, avanza solo por las columnas que caen dentro de los límites[cite: 44, 45, 46]. Cada celda es evaluada matemáticamente antes de ser procesada.

---

¿Qué te parece esta base? [cite_start]Una vez que estés conforme con esta descripción de alto nivel, podemos añadir la sección de **Análisis de Complejidad** que pide la rúbrica para completar el documento[cite: 71, 80, 103].
