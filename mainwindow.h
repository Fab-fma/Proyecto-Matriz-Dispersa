#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "Spreadsheet.h" // <-- Tu archivo backend

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Estas funciones reaccionan a lo que hace el usuario
    void onCellChanged(int row, int col);
    void on_btnEliminarFila_clicked();
    void on_btnEliminarColumna_clicked();
    void on_btnEliminarRango_clicked();
    void on_btnCalcularTodo_clicked();

private:
    Ui::MainWindow *ui;
    Spreadsheet sheet; // <-- Tu estructura de datos real

    void colorearCelda(int row, int col, bool ocupada);
};
#endif // MAINWINDOW_H