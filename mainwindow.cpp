#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidgetItem>
#include <QString>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 1. Configurar tamaño inicial de la tabla visual (ej. 20x20)
    ui->tableWidget->setRowCount(20);
    ui->tableWidget->setColumnCount(20);
    QStringList headers;
    for (int i = 0; i < 26; ++i) {
        headers << QString(QChar('A' + i)); // Genera A, B, C... Z
    }
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    // 2. Conectar la tabla: cada vez que se edita una celda, llama a onCellChanged
    connect(ui->tableWidget, &QTableWidget::cellChanged, this, &MainWindow::onCellChanged);
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

// 3. CALCULAR SUMA (Y PROBAR FÓRMULAS)
void MainWindow::on_btnCalcularTodo_clicked() {
    QString texto = ui->lineEditRango->text().toUpper();
    if (!texto.contains(":")) return;

    QStringList partes = texto.split(":");
    int r1, c1, r2, c2;
    sheet.parseCellRef(partes[0].toStdString(), r1, c1);
    sheet.parseCellRef(partes[1].toStdString(), r2, c2);

    // Aquí es donde brillan tus fórmulas. Si A1 tiene 10 y B1 tiene "=A1+5",
    // la suma del rango A1:B1 dará 25.
    double suma = sheet.sumaRango(r1, c1, r2, c2);

    ui->labelResultado->setText("Suma del rango: " + QString::number(suma));
}

