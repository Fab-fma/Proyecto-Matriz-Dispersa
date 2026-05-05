
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


## 2. Descripción del Backend y Logica Interna (`Spreadsheet.h`)

El núcleo de la hoja de cálculo es una clase manejada enteramente mediante memoria dinámica. Para evitar el desperdicio de memoria de una matriz densa, se implementa una **matriz dispersa basada en listas enlazadas cruzadas**. 

A continuación, se detalla la lógica de alto nivel y la implementación de cada operación clave. Esta estructura está diseñada para recorrer únicamente las celdas ocupadas, optimizando el tiempo de ejecución.

### A. Estructura Base y Nodos
Cada celda ocupada es un nodo. [cite_start]Se asume la existencia de dos vectores dinámicos: `rowHeaders` y `colHeaders`, que guardan los punteros al primer nodo de cada fila y columna activa respectivamente[cite: 111].

```cpp
struct CellNode {
    int row;
    int col;
    string value;
    CellNode* nextInRow;
    CellNode* nextInCol;

    CellNode(int r, int c, const string& val)
        : row(r), col(c), value(val), nextInRow(nullptr), nextInCol(nullptr) {}
};

// Dentro de la clase Spreadsheet:
vector<CellNode*> rowHeaders;
vector<CellNode*> colHeaders;
```

### B. Inserción y Modificación (`insertarCelda`)
**Lógica paso a paso:**
1. **Búsqueda de actualización:** Se recorre la fila desde la cabecera. [cite_start]Si se encuentra un nodo en la misma columna, simplemente se actualiza su valor y se termina la ejecución (no se crean nodos duplicados)[cite: 112, 113].
2. [cite_start]**Creación:** Si la celda no existía, se instancia un nuevo `CellNode`[cite: 114].
3. [cite_start]**Enlace Horizontal (Filas):** Se utilizan funciones de apoyo (`getPrevInRow`) para localizar el nodo anterior exacto, garantizando que los nodos se mantengan ordenados por columna de menor a mayor[cite: 115]. [cite_start]Se ajustan los punteros `nextInRow`[cite: 117].
4. [cite_start]**Enlace Vertical (Columnas):** Se repite la lógica con `getPrevInCol` para enlazar el nodo en su columna respectiva, manteniendo el orden por filas[cite: 118].

```cpp
void insertarCelda(int row, int col, const string& valor) {
    expandirCabeceras(row, col);

    // 1. Búsqueda y actualización
    CellNode* actual = rowHeaders[row];
    while (actual != nullptr && actual->col <= col) {
        if (actual->col == col) {
            actual->value = valor;
            return;
        }
        actual = actual->nextInRow;
    }

    // 2. Creación
    CellNode* nuevoNodo = new CellNode(row, col, valor);

    // 3. Enlace en la fila
    if (rowHeaders[row] == nullptr || rowHeaders[row]->col > col) {
        nuevoNodo->nextInRow = rowHeaders[row];
        rowHeaders[row] = nuevoNodo;
    } else {
        CellNode* prevRow = getPrevInRow(row, col);
        nuevoNodo->nextInRow = prevRow->nextInRow;
        prevRow->nextInRow = nuevoNodo;
    }

    // 4. Enlace en la columna
    if (colHeaders[col] == nullptr || colHeaders[col]->row > row) {
        nuevoNodo->nextInCol = colHeaders[col];
        colHeaders[col] = nuevoNodo;
    } else {
        CellNode* prevCol = getPrevInCol(row, col);
        nuevoNodo->nextInCol = prevCol->nextInCol;
        prevCol->nextInCol = nuevoNodo;
    }
}
```

### C. Consulta y Motor de Fórmulas (`consultarCelda` y `evaluarCelda`)
**Lógica paso a paso:**
1. [cite_start]**Extracción cruda:** `consultarCelda` verifica que la fila exista[cite: 119]. [cite_start]Luego itera a través de los nodos de esa fila usando `nextInRow`[cite: 120]. [cite_start]Si encuentra coincidencia exacta de columna, retorna el texto[cite: 121]. [cite_start]Si se pasa del índice, retorna vacío[cite: 122].
2. **Evaluación matemática:** `evaluarCelda` revisa si el texto extraído inicia con `=`. Si es así, llama al motor `evaluarFormula` que separa los operandos aritméticos.
3. **Recursividad:** Si una fórmula hace referencia a otra celda (ej. `=A1+5`), el parser resuelve la coordenada de `A1` y llama a `evaluarCelda(A1)` recursivamente para obtener su valor real.

```cpp
string consultarCelda(int row, int col) {
    if (row >= rowHeaders.size() || col >= colHeaders.size()) return "";
    CellNode* actual = rowHeaders[row];
    while (actual != nullptr && actual->col <= col) {
        if (actual->col == col) return actual->value;
        actual = actual->nextInRow;
    }
    return "";
}

double evaluarCelda(int row, int col) {
    string val = consultarCelda(row, col);
    if (val.empty()) return 0.0;
    if (val[0] == '=') return evaluarFormula(val.substr(1));
    try { return stod(val); } catch (...) { return 0.0; }
}
```

### D. Eliminación Simple (`eliminarCelda`)
**Lógica paso a paso:**
1. [cite_start]**Validación:** Si la celda no existe en la estructura, la función termina sin provocar fallos de memoria (caso borde controlado)[cite: 123, 124].
2. [cite_start]**Desconexión Horizontal:** Se ubica el nodo anterior en la fila y se hace que su puntero `nextInRow` "salte" (puentee) al nodo a eliminar, apuntando directamente al siguiente[cite: 125, 126]. Si es el primer nodo de la fila, se actualiza la cabecera.
3. [cite_start]**Desconexión Vertical:** Se aplica el mismo concepto de "puenteo" en la lista de la columna usando `nextInCol`[cite: 127, 128].
4. [cite_start]**Liberación:** Se elimina el nodo de la memoria de forma segura (`delete`)[cite: 129].

```cpp
void eliminarCelda(int row, int col) {
    if (row >= rowHeaders.size() || col >= colHeaders.size()) return;
    CellNode* nodoAEliminar = nullptr;

    // Extraer de la fila
    if (rowHeaders[row] != nullptr && rowHeaders[row]->col == col) {
        nodoAEliminar = rowHeaders[row];
        rowHeaders[row] = nodoAEliminar->nextInRow;
    } else {
        CellNode* prevRow = getPrevInRow(row, col);
        if (prevRow != nullptr && prevRow->nextInRow != nullptr && prevRow->nextInRow->col == col) {
            nodoAEliminar = prevRow->nextInRow;
            prevRow->nextInRow = nodoAEliminar->nextInRow;
        }
    }
    
    if (nodoAEliminar == nullptr) return; // No existe

    // Extraer de la columna
    if (colHeaders[col] != nullptr && colHeaders[col]->row == row) {
        colHeaders[col] = colHeaders[col]->nextInCol;
    } else {
        CellNode* prevCol = getPrevInCol(row, col);
        if (prevCol != nullptr && prevCol->nextInCol != nullptr) {
            prevCol->nextInCol = prevCol->nextInCol->nextInCol;
        }
    }

    delete nodoAEliminar;
}
```

### E. Eliminaciones Masivas (`eliminarFila`, `eliminarColumna`, `eliminarRango`)
**Lógica paso a paso:**
Estas funciones actúan como envoltorios de conveniencia apoyados en la robustez de `eliminarCelda`.
1. [cite_start]Para borrar una fila, se lee la cabecera y se itera sobre todos los nodos existentes en esa lista[cite: 130, 131].
2. [cite_start]Se guarda temporalmente el puntero al siguiente nodo[cite: 131].
3. [cite_start]Se invoca `eliminarCelda(row, col)` aislando toda la lógica compleja en una sola función[cite: 132, 133].

```cpp
void eliminarFila(int row) {
    if (row >= rowHeaders.size() || rowHeaders[row] == nullptr) return;
    CellNode* actual = rowHeaders[row];
    while (actual != nullptr) {
        int col = actual->col;
        CellNode* siguiente = actual->nextInRow; // Resguardar antes de borrar
        eliminarCelda(row, col);
        actual = siguiente;
    }
}

void eliminarColumna(int col) {
    if (col >= colHeaders.size() || colHeaders[col] == nullptr) return;
    CellNode* actual = colHeaders[col];
    while (actual != nullptr) {
        int row = actual->row;
        CellNode* siguiente = actual->nextInCol; // Resguardar antes de borrar
        eliminarCelda(row, col);
        actual = siguiente;
    }
}

void eliminarRango(int r1, int c1, int r2, int c2) {
    for (int r = r1; r <= r2; ++r) {
        if (r >= rowHeaders.size()) continue;
        
        // Recolectar columnas a borrar primero para no romper los punteros de la iteración
        vector<int> colsToDelete;
        CellNode* actual = rowHeaders[r];
        
        while (actual != nullptr && actual->col <= c2) {
            if (actual->col >= c1) colsToDelete.push_back(actual->col);
            actual = actual->nextInRow;
        }
        
        // Ejecutar eliminación segura
        for (int c : colsToDelete) eliminarCelda(r, c);
    }
}
```

### F. Operaciones de Agregación (`suma`, `promedio`, `maximo`, `minimo`)
**Lógica paso a paso:**
Todos los métodos de agregación comparten el mismo motor de iteración eficiente:
1. **Iteración acotada:** Se utiliza un bucle externo desde la fila de inicio hasta la fila de fin.
2. **Salto inteligente:** En cada fila, el puntero inicia en la cabecera y avanza rápidamente hasta alcanzar la columna de inicio (`col_inicio`), evitando recorrer la matriz desde cero.
3. **Filtro Numérico/Fórmula:** Mientras el nodo esté dentro del límite (`col_fin`), se verifica si su texto indica un número o una fórmula (ej. `=`). Si es válido, se llama a `evaluarCelda()` para obtener su valor matemático real, resolviendo referencias dinámicas si existen.
4. **Cálculo Específico:**
   * **Promedio:** Suma los valores y mantiene un contador. Protege contra la división por cero.
   * **Máximo/Mínimo:** Inician con `-infinito` e `infinito` respectivamente, y actualizan el valor si encuentran uno que rompa el récord actual.

```cpp
```cpp
double sumaRango(int r1, int c1, int r2, int c2) {
    double suma = 0.0;
    for (int r = r1; r <= r2; ++r) {
        if (r >= rowHeaders.size()) continue;
        
        CellNode* actual = rowHeaders[r];
        
        // Saltar celdas anteriores al rango
        while (actual != nullptr && actual->col < c1) actual = actual->nextInRow;
        
        // Sumar celdas dentro del rango
        while (actual != nullptr && actual->col <= c2) {
            string val = actual->value;
            if (!val.empty() && (isdigit(val[0]) || val[0] == '-' || val[0] == '=')) {
                suma += evaluarCelda(actual->row, actual->col);
            }
            actual = actual->nextInRow;
        }
    }
    return suma;
}

double promedioRango(int r1, int c1, int r2, int c2) {
    double suma = 0.0;
    int count = 0;
    for (int r = r1; r <= r2; ++r) {
        if (r >= rowHeaders.size()) continue;
        CellNode* actual = rowHeaders[r];
        while (actual != nullptr && actual->col < c1) actual = actual->nextInRow;
        while (actual != nullptr && actual->col <= c2) {
            string val = actual->value;
            if (!val.empty() && (isdigit(val[0]) || val[0] == '-' || val[0] == '=')) {
                suma += evaluarCelda(actual->row, actual->col);
                count++;
            }
            actual = actual->nextInRow;
        }
    }
    return count > 0 ? suma / count : 0.0;
}

double maximoRango(int r1, int c1, int r2, int c2) {
    double maximo = -numeric_limits<double>::infinity();
    bool encontrado = false;
    for (int r = r1; r <= r2; ++r) {
        if (r >= rowHeaders.size()) continue;
        CellNode* actual = rowHeaders[r];
        while (actual != nullptr && actual->col < c1) actual = actual->nextInRow;
        while (actual != nullptr && actual->col <= c2) {
            string val = actual->value;
            if (!val.empty() && (isdigit(val[0]) || val[0] == '-' || val[0] == '=')) {
                double v = evaluarCelda(actual->row, actual->col);
                if (v > maximo) maximo = v;
                encontrado = true;
            }
            actual = actual->nextInRow;
        }
    }
    return encontrado ? maximo : 0.0;
}

double minimoRango(int r1, int c1, int r2, int c2) {
    double minimo = numeric_limits<double>::infinity();
    bool encontrado = false;
    for (int r = r1; r <= r2; ++r) {
        if (r >= rowHeaders.size()) continue;
        CellNode* actual = rowHeaders[r];
        while (actual != nullptr && actual->col < c1) actual = actual->nextInRow;
        while (actual != nullptr && actual->col <= c2) {
            string val = actual->value;
            if (!val.empty() && (isdigit(val[0]) || val[0] == '-' || val[0] == '=')) {
                double v = evaluarCelda(actual->row, actual->col);
                if (v < minimo) minimo = v;
                encontrado = true;
            }
            actual = actual->nextInRow;
        }
    }
    return encontrado ? minimo : 0.0;
}
```
## 3. Análisis de Complejidad

### 3.1. Estructura de datos y justificación
La implementación de `Spreadsheet.h` usa una estructura de matriz dispersa basada en listas enlazadas cruzadas.

- Cada nodo `CellNode` contiene:
  - `int row`
  - `int col`
  - `string value`
  - `CellNode* nextInRow`
  - `CellNode* nextInCol`
- Hay dos vectores de cabeceras:
  - `rowHeaders[r]` apunta al primer nodo de la fila `r`
  - `colHeaders[c]` apunta al primer nodo de la columna `c`

Esta organización es equivalente a una lista doblemente enlazada en el sentido de que cada celda se enlaza en dos dimensiones: por fila y por columna. Aunque no existe un puntero `prev` explícito, la estructura mantiene enlaces bidireccionales lógicos porque cada nodo participa en dos listas independientes.

### 3.2. Complejidad por función

- `expandirCabeceras(int maxRow, int maxCol)`
  - Complejidad: `O(maxRow + maxCol)` en el peor caso.
  - Justificación: redimensiona los vectores de cabeceras hasta el índice requerido.

- `getPrevInRow(int row, int col)`
  - Complejidad: `O(k_row)`.
  - Justificación: recorre los nodos de la fila `row` hasta encontrar el nodo anterior a la columna `col`.

- `getPrevInCol(int row, int col)`
  - Complejidad: `O(k_col)`.
  - Justificación: recorre los nodos de la columna `col` hasta encontrar el nodo anterior a la fila `row`.

- `parseCellRef(const string& ref, int& row, int& col)`
  - Complejidad: `O(|ref|)`.
  - Justificación: convierte una referencia tipo `A1` a índices numéricos.

- `parseOperand(const string& op)`
  - Complejidad: `O(|op| + costRef)`.
  - Justificación: analiza un operando, que puede ser un número o una referencia a otra celda. Si es referencia, llama a `evaluarCelda`.

- `evaluarFormula(const string& formula)`
  - Complejidad: `O(|formula| + costOperands)`.
  - Justificación: busca el operador y evalúa operandos numéricos o referencias.

- `insertarCelda(int row, int col, const string& valor)`
  - Complejidad: `O(k_row + k_col)`.
  - Justificación: busca la posición en la fila, crea el nodo y enlaza en fila y columna. Con `k = max(k_row, k_col)`, cumple `O(k)`.

- `consultarCelda(int row, int col)`
  - Complejidad: `O(k_row)`.
  - Justificación: recorre la lista de la fila `row` hasta hallar la columna deseada o superarla.

- `evaluarCelda(int row, int col)`
  - Complejidad: `O(k_row + costFormula)`.
  - Justificación: obtiene el valor de la celda y, si es fórmula, evalúa la expresión.

- `eliminarCelda(int row, int col)`
  - Complejidad: `O(k_row + k_col)`.
  - Justificación: localiza y elimina el nodo en la fila y actualiza el enlace correspondiente en la columna.

- `eliminarFila(int row)`
  - Complejidad: `O(t * k)` donde `t` es el número de celdas en la fila.
  - Justificación: recorre todos los nodos de la fila y elimina cada celda con `eliminarCelda`, lo que vuelve a recorrer las columnas relacionadas.

- `eliminarColumna(int col)`
  - Complejidad: `O(t * k)` donde `t` es el número de celdas en la columna.
  - Justificación: recorre la columna y elimina cada celda con `eliminarCelda`, volviendo a recorrer las filas relacionadas.

- `eliminarRango(int r1, int c1, int r2, int c2)`
  - Complejidad: `O(r * k + d * k)` donde `r` es el número de filas recorridas y `d` el número de celdas eliminadas.
  - Justificación: para cada fila se recopilan columnas dentro del rango y luego se eliminan celdas individualmente.

- `sumaRango(int r1, int c1, int r2, int c2)`
  - Complejidad: `O(k_total)`.
  - Justificación: recorre solo las celdas no vacías relevantes dentro del rango.

- `promedioRango(int r1, int c1, int r2, int c2)`
  - Complejidad: `O(k_total)`.
  - Justificación: igual que `sumaRango`, sumando valores y contando celdas válidas.

- `maximoRango(int r1, int c1, int r2, int c2)`
  - Complejidad: `O(k_total)`.
  - Justificación: recorre las celdas del rango y actualiza el máximo encontrado.

- `minimoRango(int r1, int c1, int r2, int c2)`
  - Complejidad: `O(k_total)`.
  - Justificación: recorre las celdas del rango y actualiza el mínimo encontrado.

### 3.3. Justificación clara de la elección

Usar listas enlazadas cruzadas es una elección adecuada para matrices dispersas porque:

1. Solo almacena celdas no vacías, evitando la sobrecarga de memoria de una matriz densa.
2. Permite recorrer eficientemente por filas y por columnas sin visitar celdas vacías.
3. La operación sobre una celda individual (inserción, búsqueda o eliminación) se limita a la fila y columna activas, lo que da un costo `O(k)`.
4. Para funciones de agregación y rango, la estructura permite saltar directamente a las celdas presentes dentro de los límites dados.