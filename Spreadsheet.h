#ifndef SPREADSHEET_H
#define SPREADSHEET_H

#include <bits/stdc++.h>

using namespace std;

struct CellNode {
    int row;
    int col;
    string value;
    CellNode* nextInRow;
    CellNode* nextInCol;

    CellNode(int r, int c, const string& val)
        : row(r), col(c), value(val), nextInRow(nullptr), nextInCol(nullptr) {}
};

class Spreadsheet {
private:
    vector<CellNode*> rowHeaders;
    vector<CellNode*> colHeaders;

    void expandirCabeceras(int maxRow, int maxCol) {
        if (maxRow >= rowHeaders.size()) rowHeaders.resize(maxRow + 1, nullptr);
        if (maxCol >= colHeaders.size()) colHeaders.resize(maxCol + 1, nullptr);
    }

    CellNode* getPrevInRow(int row, int col) {
        CellNode* actual = rowHeaders[row];
        while (actual != nullptr && actual->nextInRow != nullptr && actual->nextInRow->col < col) {
            actual = actual->nextInRow;
        }
        return actual;
    }

    CellNode* getPrevInCol(int row, int col) {
        CellNode* actual = colHeaders[col];
        while (actual != nullptr && actual->nextInCol != nullptr && actual->nextInCol->row < row) {
            actual = actual->nextInCol;
        }
        return actual;
    }

    // Evalúa un operando (puede ser número o referencia como "A1")
    double parseOperand(const string& op) {
        if (op.empty()) return 0.0;
        if (isalpha(op[0])) {
            int r, c;
            parseCellRef(op, r, c);
            return evaluarCelda(r, c); // si A1 tambien tiene formula
        }
        try { return stod(op); } catch (...) { return 0.0; }
    }

    // Procesa expresiones matemáticas simples
    double evaluarFormula(const string& formula) {
        char op = 0;
        int opPos = -1;
        // find op
        for (int i = 0; i < formula.length(); ++i) {
            if (formula[i] == '+' || formula[i] == '-' || formula[i] == '*' || formula[i] == '/') {
                op = formula[i];
                opPos = i;
                break;
            }
        }

        if (opPos == -1) return parseOperand(formula); // Es solo un número o referencia directa

        string left = formula.substr(0, opPos);
        string right = formula.substr(opPos + 1);

        double valLeft = parseOperand(left);
        double valRight = parseOperand(right);

        switch(op) {
        case '+': return valLeft + valRight;
        case '-': return valLeft - valRight;
        case '*': return valLeft * valRight;
        case '/': return valRight != 0 ? valLeft / valRight : 0.0;
        }
        return 0.0;
    }

public:
    Spreadsheet() {
        rowHeaders.resize(100, nullptr);
        colHeaders.resize(100, nullptr);
    }

    // convertir string en cordenadas
    void parseCellRef(const string& ref, int& row, int& col) {
        string colStr = "", rowStr = "";
        for (char c : ref) {
            if (isalpha(c)) colStr += toupper(c);
            else if (isdigit(c)) rowStr += c;
        }
        col = 0;
        for (char c : colStr) col = col * 26 + (c - 'A' + 1);
        col -= 1;
        row = stoi(rowStr) - 1;
    }

    void insertarCelda(int row, int col, const string& valor) {
        expandirCabeceras(row, col);

        // if exists
        CellNode* actual = rowHeaders[row];
        while (actual != nullptr && actual->col <= col) {
            if (actual->col == col) {
                actual->value = valor;
                return;
            }
            actual = actual->nextInRow;
        }

        CellNode* nuevoNodo = new CellNode(row, col, valor);

        // row pointers
        if (rowHeaders[row] == nullptr || rowHeaders[row]->col > col) {
            nuevoNodo->nextInRow = rowHeaders[row];
            rowHeaders[row] = nuevoNodo;
        }
        else {
            CellNode* prevRow = getPrevInRow(row, col);
            nuevoNodo->nextInRow = prevRow->nextInRow;
            prevRow->nextInRow = nuevoNodo;
        }

        // column pointers
        if (colHeaders[col] == nullptr || colHeaders[col]->row > row) {
            nuevoNodo->nextInCol = colHeaders[col];
            colHeaders[col] = nuevoNodo;
        }
        else {
            CellNode* prevCol = getPrevInCol(row, col);
            nuevoNodo->nextInCol = prevCol->nextInCol;
            prevCol->nextInCol = nuevoNodo;
        }
    }

    string consultarCelda(int row, int col) {
        if (row >= rowHeaders.size() || col >= colHeaders.size()) return "";
        CellNode* actual = rowHeaders[row];
        while (actual != nullptr && actual->col <= col) {
            if (actual->col == col) return actual->value;
            actual = actual->nextInRow;
        }
        return "";
    }

    // interpreta formulas
    double evaluarCelda(int row, int col) {
        string val = consultarCelda(row, col);
        if (val.empty()) return 0.0;
        if (val[0] == '=') return evaluarFormula(val.substr(1));
        try { return stod(val); } catch (...) { return 0.0; }
    }

    void eliminarCelda(int row, int col) {
        if (row >= rowHeaders.size() || col >= colHeaders.size()) return;
        CellNode* nodoAEliminar = nullptr;

        // Extraer de la fila
        if (rowHeaders[row] != nullptr && rowHeaders[row]->col == col) {
            nodoAEliminar = rowHeaders[row];
            rowHeaders[row] = nodoAEliminar->nextInRow;
        }
        else {
            CellNode* prevRow = getPrevInRow(row, col);
            if (prevRow != nullptr && prevRow->nextInRow != nullptr && prevRow->nextInRow->col == col) {
                nodoAEliminar = prevRow->nextInRow;
                prevRow->nextInRow = nodoAEliminar->nextInRow;
            }
        }

        if (nodoAEliminar == nullptr) return;

        // Extraer de la columna
        if (colHeaders[col] != nullptr && colHeaders[col]->row == row) {
            colHeaders[col] = colHeaders[col]->nextInCol;
        }
        else {
            CellNode* prevCol = getPrevInCol(row, col);
            if (prevCol != nullptr && prevCol->nextInCol != nullptr) {
                prevCol->nextInCol = prevCol->nextInCol->nextInCol;
            }
        }

        delete nodoAEliminar;
    }

    void eliminarFila(int row) {
        if (row >= rowHeaders.size() || rowHeaders[row] == nullptr) return;
        CellNode* actual = rowHeaders[row];
        while (actual != nullptr) {
            int col = actual->col;
            CellNode* siguiente = actual->nextInRow;
            eliminarCelda(row, col);
            actual = siguiente;
        }
    }

    void eliminarColumna(int col) {
        if (col >= colHeaders.size() || colHeaders[col] == nullptr) return;
        CellNode* actual = colHeaders[col];
        while (actual != nullptr) {
            int row = actual->row;
            CellNode* siguiente = actual->nextInCol;
            eliminarCelda(row, col);
            actual = siguiente;
        }
    }

    void eliminarRango(int r1, int c1, int r2, int c2) {
        for (int r = r1; r <= r2; ++r) {
            if (r >= rowHeaders.size()) continue;
            vector<int> colsToDelete;
            CellNode* actual = rowHeaders[r];
            while (actual != nullptr && actual->col <= c2) {
                if (actual->col >= c1) colsToDelete.push_back(actual->col);
                actual = actual->nextInRow;
            }
            for (int c : colsToDelete) eliminarCelda(r, c);
        }
    }


    double sumaRango(int r1, int c1, int r2, int c2) {
        double suma = 0.0;
        for (int r = r1; r <= r2; ++r) {
            if (r >= rowHeaders.size()) continue;
            CellNode* actual = rowHeaders[r];
            while (actual != nullptr && actual->col < c1) actual = actual->nextInRow;
            while (actual != nullptr && actual->col <= c2) {
                // fórmula o número
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

};

#endif // SPREADSHEET_H
