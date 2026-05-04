#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidgetItem>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. Configurar tamaño inicial de la tabla visual
    int totalFilas = 1000;
    int totalColumnas = 702; // 702 columnas equivale exactamente hasta la columna "ZZ"

    ui->tableWidget->setRowCount(totalFilas);
    ui->tableWidget->setColumnCount(totalColumnas);

    // Generar nombres de columnas al estilo Excel (A, B... Z, AA, AB... ZZ)
    QStringList headers;
    for (int i = 0; i < totalColumnas; ++i) {
        QString colName = "";
        int temp = i;
        while (temp >= 0) {
            colName.prepend(QChar('A' + (temp % 26)));
            temp = (temp / 26) - 1;
        }
        headers << colName;
    }
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    // 2. Conectar la tabla: cada vez que se edita una celda, llama a onCellChanged
    connect(ui->tableWidget, &QTableWidget::cellChanged, this, &MainWindow::onCellChanged);
    connect(ui->tableWidget, &QTableWidget::cellClicked, this, &MainWindow::onCellClicked);
    connect(ui->tableWidget, &QTableWidget::itemSelectionChanged, this, &MainWindow::onSelectionChanged);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::colorearCelda(int row, int col, bool ocupada) {
    QTableWidgetItem* item = ui->tableWidget->item(row, col);
    if (!item) return;

    ui->tableWidget->blockSignals(true); // Pausamos para no crear bucles

    // Cumplimos la rúbrica usando texto en negrita en vez de fondo de color
    QFont font = item->font();
    font.setBold(ocupada);
    item->setFont(font);

    // Si la celda ya no está ocupada, nos aseguramos de que quede totalmente en blanco
    if (!ocupada) {
        item->setText("");
    }

    ui->tableWidget->blockSignals(false);
}

// Se ejecuta cada vez que das "Enter" tras escribir en una celda
void MainWindow::onCellChanged(int row, int col) {
    QTableWidgetItem* item = ui->tableWidget->item(row, col);
    if (!item) return;

    std::string valor = item->text().toStdString();

    if (valor.empty()) {
        sheet.eliminarCelda(row, col);
        colorearCelda(row, col, false);
    } else {
        sheet.insertarCelda(row, col, valor);
        colorearCelda(row, col, true);
    }
}

// Se ejecuta al hacer clic en una celda
void MainWindow::onCellClicked(int row, int col) {
    std::string crudo = sheet.consultarCelda(row, col);

    if (crudo.empty()) {
        ui->labelValorCelda->setText("Celda vacía");
        return;
    }

    // Le pedimos al backend que evalúe la celda matemática
    double valorEvaluado = sheet.evaluarCelda(row, col);

    // Mostramos ambos valores
    QString texto = "celda: " + QString::fromStdString(crudo) +
                    "  |  valor: " + QString::number(valorEvaluado);

    ui->labelValorCelda->setText(texto);
}

// Se ejecuta al dar clic en el botón "Eliminar Fila"
void MainWindow::on_btnEliminarFila_clicked() {
    int fila = ui->spinBoxFila->value() - 1; // -1 porque la interfaz es 1-based y tu array 0-based
    sheet.eliminarFila(fila);

    // Limpiamos visualmente esa fila en la tabla usando nuestra función estándar
    for (int c = 0; c < ui->tableWidget->columnCount(); ++c) {
        colorearCelda(fila, c, false);
    }
}

void MainWindow::on_btnEliminarColumna_clicked() {
    QString texto = ui->lineEditColumna->text().toUpper(); // Ej: "B"
    if (texto.isEmpty()) return;

    // Convertir la letra a número (B -> 1, C -> 2)
    int col = 0;
    for (QChar c : texto) col = col * 26 + (c.unicode() - 'A' + 1);
    col -= 1;

    sheet.eliminarColumna(col); // Llama a tu backend

    // Limpia visualmente toda esa columna en la pantalla
    for (int r = 0; r < ui->tableWidget->rowCount(); ++r) {
        colorearCelda(r, col, false);
    }
}

// Auto-rellena el texto del rango cuando seleccionas con el ratón
void MainWindow::onSelectionChanged() {
    QList<QTableWidgetSelectionRange> rangos = ui->tableWidget->selectedRanges();
    if (rangos.isEmpty()) return;

    int r1 = rangos.first().topRow();
    int c1 = rangos.first().leftColumn();
    int r2 = rangos.first().bottomRow();
    int c2 = rangos.first().rightColumn();

    // Función lambda rápida para convertir número a letra (0 -> A, 26 -> AA)
    auto colToLetra = [](int c) {
        QString res = "";
        while (c >= 0) {
            res.prepend(QChar('A' + (c % 26)));
            c = (c / 26) - 1;
        }
        return res;
    };

    // Arma el texto final (ej. "A1:C3")
    QString texto = colToLetra(c1) + QString::number(r1 + 1) + ":" + colToLetra(c2) + QString::number(r2 + 1);

    // Lo escribe en tu cajita de texto
    ui->lineEditRango->setText(texto);
}

// 2. ELIMINAR RANGO
void MainWindow::on_btnEliminarRango_clicked() {
    QString texto = ui->lineEditRango->text().toUpper(); // Ej: "A1:C3"
    QStringList partes = texto.split(":");
    if (partes.size() != 2) return;

    int r1, c1, r2, c2;
    sheet.parseCellRef(partes[0].toStdString(), r1, c1);
    sheet.parseCellRef(partes[1].toStdString(), r2, c2);

    sheet.eliminarRango(r1, c1, r2, c2); // Llama a tu backend

    // Limpia visualmente ese rectángulo en la pantalla
    for (int r = r1; r <= r2; ++r) {
        for (int c = c1; c <= c2; ++c) {
            colorearCelda(r, c, false);
        }
    }
}

// 3. CALCULAR SUMA, PROMEDIO, MAX Y MIN
void MainWindow::on_btnCalcularTodo_clicked() {
    QString texto = ui->lineEditRango->text().toUpper();
    if (!texto.contains(":")) return;

    QStringList partes = texto.split(":");
    int r1, c1, r2, c2;
    sheet.parseCellRef(partes[0].toStdString(), r1, c1);
    sheet.parseCellRef(partes[1].toStdString(), r2, c2);

    // Llamamos a las 4 funciones de tu backend
    double suma = sheet.sumaRango(r1, c1, r2, c2);
    double prom = sheet.promedioRango(r1, c1, r2, c2);
    double max = sheet.maximoRango(r1, c1, r2, c2);
    double min = sheet.minimoRango(r1, c1, r2, c2);

    // Formateamos el texto con saltos de línea (\n)
    QString resultado = "Suma: " + QString::number(suma) + "\n" +
                        "Promedio: " + QString::number(prom) + "\n" +
                        "Máximo: " + QString::number(max) + "\n" +
                        "Mínimo: " + QString::number(min);

    ui->labelResultado->setText(resultado);
}

// Salta visualmente a la celda escrita
void MainWindow::on_btnIrA_clicked() {
    QString ref = ui->lineEditIrA->text().toUpper(); // Ej: "ZZ500"
    if (ref.isEmpty()) return;

    int r, c;
    // Usamos el traductor de tu backend que ya funciona perfecto
    sheet.parseCellRef(ref.toStdString(), r, c);

    // Verificamos que no te pida una celda fuera de los límites visuales
    if (r >= 0 && r < ui->tableWidget->rowCount() && c >= 0 && c < ui->tableWidget->columnCount()) {
        // Esta sola línea de Qt hace que la tabla haga scroll mágico hasta esa celda
        ui->tableWidget->setCurrentCell(r, c);
    }
}

