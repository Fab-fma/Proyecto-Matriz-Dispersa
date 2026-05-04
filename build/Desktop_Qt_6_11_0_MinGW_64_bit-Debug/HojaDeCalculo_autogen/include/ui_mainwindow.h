/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *btnEliminarFila;
    QLineEdit *lineEditRango;
    QPushButton *btnCalcularTodo;
    QLabel *labelResultado;
    QTableWidget *tableWidget;
    QSpinBox *spinBoxFila;
    QLineEdit *lineEditColumna;
    QPushButton *btnEliminarColumna;
    QPushButton *btnEliminarRango;
    QLabel *labelValorCelda;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        btnEliminarFila = new QPushButton(centralwidget);
        btnEliminarFila->setObjectName("btnEliminarFila");
        btnEliminarFila->setGeometry(QRect(400, 450, 80, 24));
        lineEditRango = new QLineEdit(centralwidget);
        lineEditRango->setObjectName("lineEditRango");
        lineEditRango->setGeometry(QRect(40, 450, 113, 24));
        btnCalcularTodo = new QPushButton(centralwidget);
        btnCalcularTodo->setObjectName("btnCalcularTodo");
        btnCalcularTodo->setGeometry(QRect(170, 450, 91, 24));
        labelResultado = new QLabel(centralwidget);
        labelResultado->setObjectName("labelResultado");
        labelResultado->setGeometry(QRect(40, 490, 101, 61));
        tableWidget = new QTableWidget(centralwidget);
        tableWidget->setObjectName("tableWidget");
        tableWidget->setGeometry(QRect(20, 10, 761, 421));
        spinBoxFila = new QSpinBox(centralwidget);
        spinBoxFila->setObjectName("spinBoxFila");
        spinBoxFila->setGeometry(QRect(321, 450, 71, 31));
        lineEditColumna = new QLineEdit(centralwidget);
        lineEditColumna->setObjectName("lineEditColumna");
        lineEditColumna->setGeometry(QRect(330, 500, 113, 24));
        btnEliminarColumna = new QPushButton(centralwidget);
        btnEliminarColumna->setObjectName("btnEliminarColumna");
        btnEliminarColumna->setGeometry(QRect(449, 500, 101, 24));
        btnEliminarRango = new QPushButton(centralwidget);
        btnEliminarRango->setObjectName("btnEliminarRango");
        btnEliminarRango->setGeometry(QRect(170, 490, 91, 24));
        labelValorCelda = new QLabel(centralwidget);
        labelValorCelda->setObjectName("labelValorCelda");
        labelValorCelda->setGeometry(QRect(530, 450, 131, 20));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 21));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        btnEliminarFila->setText(QCoreApplication::translate("MainWindow", "Eliminar Fila", nullptr));
        btnCalcularTodo->setText(QCoreApplication::translate("MainWindow", "Calcular Todo", nullptr));
        labelResultado->setText(QCoreApplication::translate("MainWindow", "Rango:", nullptr));
        btnEliminarColumna->setText(QCoreApplication::translate("MainWindow", "Eliminar Columna", nullptr));
        btnEliminarRango->setText(QCoreApplication::translate("MainWindow", "Eliminar Rango", nullptr));
        labelValorCelda->setText(QCoreApplication::translate("MainWindow", "_", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
