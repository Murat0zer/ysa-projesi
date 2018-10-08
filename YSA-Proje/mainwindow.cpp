#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    myQGraphicsView = new MyQGraphicsView(nullptr, ui);
    ui->gridLayout->addWidget(myQGraphicsView);
    QList<QString> classList;
    for (int i=0; i<ui->classSelectComboBox->maxCount(); i++) {
         if(ui->classSelectComboBox->itemText(i) != "")
            classList.append(ui->classSelectComboBox->itemText(i));
         else
             break;
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_classSelectComboBox_currentTextChanged(const QString &selectedClass)
{
    this->selectedClass = selectedClass;
    this->myQGraphicsView->setSelectedClass(selectedClass);
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{

}

QString MainWindow::getSelectedClass()
{
    return this->selectedClass;
}

void MainWindow::setSelectedClass(const QString &value)
{
    this->selectedClass = value;
}
