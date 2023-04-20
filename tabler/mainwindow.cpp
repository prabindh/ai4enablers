/*
 * Part of ai4enablers project. 2023
 *
*/

#include "mainwindow.h"
#include "./ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_viewer(new Vision)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_pushButton_clicked()
{
    m_path = QFileDialog::getOpenFileName(nullptr, "Open PDF", "c:\\", "*.pdf");

    if (m_viewer->LoadDoc(m_path))
    {
        m_viewer->show();
    }
    else
    {
        QMessageBox msgError;
        msgError.setText("Could not load document!");
        msgError.setIcon(QMessageBox::Critical);
        msgError.setWindowTitle("Error");
        msgError.exec();
    }
}

