#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QList<QString> classList;
    int i = 1;
    while (true) {
         if(ui->classSelectComboBox->itemText(i) != "")
            classList.append(ui->classSelectComboBox->itemText(i++));
         else
             break;
    }
    myQGraphicsView = new MyQGraphicsView(nullptr, ui);
    ui->gridLayout->addWidget(myQGraphicsView);
    myQGraphicsView->setClassList(classList);

}

void delay(int ms)
{
    QTime dieTime= QTime::currentTime().addMSecs(ms);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MainWindow::on_pushButtonTrain_clicked()
{
    myQGraphicsView->clearLines();
    bool needTraining = true;
    Matrix *weights = (myQGraphicsView->getWeights());


    QMap<QString, int> diList; diList.insert("Class 1", 1); diList.insert("Class 2", -1);
    double netValue;
    double c = ui->lineEditC->text().toDouble();
    int delayInMs = ui->lineEditDelay->text().toInt();
    int cycle = 0;
    while (needTraining) {
        double error = 0;
        for(QString className : myQGraphicsView->getClassList()) {
            QList<QPointF> points = myQGraphicsView->getClassPoints().value(className);
            for(QPointF point : points) {
                Matrix xn(3, 1);
                xn.Set(1, 1, point.x());
                xn.Set(2, 1, point.y());
                xn.Set(3, 1, 1);
                Matrix delta(3, 1);
                Matrix netMatrix(1, 1);
                Matrix transpozeW(1, 3);
                transpozeW = Matrix::matrisTranspoze(*weights);
                netMatrix = Matrix::matrixMultiplication(transpozeW, xn);

                netValue = netMatrix.Get(1, 1);
                int sgnResult = netValue > 0.0 ? 1 : - 1;
                double t1 = diList.value(className) - sgnResult;
                double t2 = (c*(t1));

                if(std::abs(t2) > 0) {
                    delta.Set(1, 1, xn.Get(1,1) * t2);
                    delta.Set(2, 1, xn.Get(2,1) * t2);
                    delta.Set(3, 1, 1 * t2);

                    weights->Set(1, 1, weights->Get(1, 1) + delta.Get(1, 1));
                    weights->Set(2, 1, weights->Get(2, 1) + delta.Get(2, 1));
                    weights->Set(3, 1, weights->Get(3, 1) + delta.Get(3, 1));

                    error += std::abs(t1) / 2.00;
                }


            }

        }
        needTraining = error > 0 ? true : false;
        needTraining = cycle > 10000 ? false : needTraining;
        cycle++;

        myQGraphicsView->clearLines();
        myQGraphicsView->drawLine(*weights);
        ui->labelPointCords->setText(QString::number(cycle));
        delay(delayInMs);


    }

    if(cycle == 1) {
        Matrix *weightsx = (myQGraphicsView->getWeights());
        myQGraphicsView->drawLine(*weightsx);
    } else
        myQGraphicsView->drawLine(*weights);
}

void MainWindow::on_pushButtonClear_clicked()
{
    myQGraphicsView->clearPoints();
}

void MainWindow::on_classSelectComboBox_currentTextChanged(const QString &selectedClass)
{
    this->selectedClass = selectedClass;
    this->myQGraphicsView->setSelectedClass(selectedClass);
}

QString MainWindow::getSelectedClass()
{
    return this->selectedClass;
}

void MainWindow::setSelectedClass(const QString &value)
{
    this->selectedClass = value;
}

MainWindow::~MainWindow()
{
    delete ui;
}
