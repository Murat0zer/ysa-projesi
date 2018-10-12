#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QList<QString> classList;
    for (int i=1; i<ui->classSelectComboBox->maxCount(); i++) {
         if(ui->classSelectComboBox->itemText(i) != "")
            classList.append(ui->classSelectComboBox->itemText(i));
         else
             break;
    }
    myQGraphicsView = new MyQGraphicsView(nullptr, ui);
    ui->gridLayout->addWidget(myQGraphicsView);
    myQGraphicsView->setClassList(classList);

}

void MainWindow::on_pushButtonTrain_clicked()
{
    bool needTraining = true;
    Matrix weights(3,1);
    weights.Set(1,1, 1);
    weights.Set(2,1, 3);
    weights.Set(3,1, 3);
    myQGraphicsView->drawLine(weights);


    Matrix delta(3,1);
    QMap<QString, double> diList; diList.insert("Class 1", 1); diList.insert("Class 2", -1);
    double net;
    double c = 0.1;
    int cycle = 0;
    while (needTraining) {
        double error = 0;
        for(QString className : myQGraphicsView->getClassList()) {
            QList<QPointF> points = myQGraphicsView->getClassPoints().value(className);
            for(QPointF point : points) {
                Matrix xn(3, 1);
                xn.Set(1, 1, point.x()); xn.Set(2, 1, point.y());
                xn.Set(3, 1, diList.value(className));

                Matrix result(1,1);
                Matrix transpozeW(1,3);
                transpozeW = Matrix::matrisTranspoze(weights);
                result = Matrix::matrixMultiplication(transpozeW, xn);

                net = result.Get(1,1);
                int sgnResult = net >= 0 ? 1 : -1;
                double t1 = diList.value(className) - sgnResult;
                if(std::abs(t1) > 0){
                    delta = xn * (c*(t1));
                    weights = weights + delta;
                    error += std::abs(diList.value(className) - sgnResult) / 2.0;
                }
            }
        }
        needTraining = error > 0 ? true : false;
        needTraining = cycle > 10000 ? false : needTraining ;
        cycle++;
    }
    ui->labelPointCords->setText(QString::number(cycle));
    myQGraphicsView->drawLine(weights);


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
