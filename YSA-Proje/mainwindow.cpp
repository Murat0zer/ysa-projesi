#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "statistics.cpp"
#include <QTime>

static double PIXEL_TO_CM = 37.795276;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QList<QString> classList;
    int i = 1; double a = 1.0;
    while (true) {
        if(ui->classSelectComboBox->itemText(i) != "") {
            classList.append(ui->classSelectComboBox->itemText(i));
            diList.insert(ui->classSelectComboBox->itemText(i++), a);
            a = -a;
        } else
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
    if(activationFunc == "Binary Function")
        ui->labelCycleResultBinary->setText("0");
    else
        ui->labelCycleResultSigmoid->setText("0");
    Matrix *weightsBeginning = (myQGraphicsView->getWeights());
    Matrix weights(3, 1);
    weights.Set(1,1, weightsBeginning->Get(1,1));
    weights.Set(2,1, weightsBeginning->Get(2,1));
    weights.Set(3,1, weightsBeginning->Get(3,1));
    myQGraphicsView->clearLines();
    myQGraphicsView->drawLine(weights);
    delay(500);

    bool needTraining = true;
    int cycle = 0;
    double netValue;
    double c = ui->lineEditC->text().toDouble();
    double lambda = ui->lineEditLambda->text().toDouble();
    double maxErrForSigmoid = ui->lineEditErorForSigmoid->text().toDouble();
    Matrix delta(3, 1);
    Matrix netMatrix(1, 1);
    Matrix transpozeW(1, 3);
    Matrix xn(3, 1);
    while (needTraining) {
        double error = 0;
        double outPut = 0;
        double t1 = 0;
        int delayInMs = ui->lineEditDelay->text().toInt();
        for(QString className : myQGraphicsView->getClassList()) {
            QList<QPointF> points = myQGraphicsView->getClassPoints().value(className);

            QList<double> xValues;
            QList<double> yValues;
            for (QPointF pointQ: points) {
                xValues.append(pointQ.x() );
                yValues.append(pointQ.y() );
            }
            double maxX = *std::max_element(xValues.begin(), xValues.end());
            double minX = *std::min_element(xValues.begin(), xValues.end());

            double maxY = *std::max_element(yValues.begin(), yValues.end());
            double minY = *std::min_element(yValues.begin(), yValues.end());

            for(QPointF point : points) {

                double a;
                double b;
                xn.Set(3, 1, 1);
                if(ui->checkBoxNormalize->isChecked()) {
//                    xn.Set(1, 1, (point.x() - meanX) / devinationX);
//                    xn.Set(2, 1, (point.y() - meanY) / devinationY);
                    xn.Set(1, 1, (point.x() - minX)/(maxX - minX));
                    xn.Set(2, 1, (point.y() - minY)/(maxY - minY));

                     a = xn.Get(1,1);
                     b = xn.Get(2,1);
                }
                else {
                    xn.Set(1, 1, point.x());
                    xn.Set(2, 1, point.y());
                }

                transpozeW = Matrix::matrisTranspoze(weights);
                netMatrix = Matrix::matrixMultiplication(transpozeW, xn);
                netValue = netMatrix.Get(1, 1);

                if(ui->checkBoxNormalize->isChecked()){
//                    xn.Set(1,1, (xn.Get(1,1) + meanX) * devinationX);
//                    xn.Set(2,1, (xn.Get(2,1) + meanY) * devinationY);
                    xn.Set(1, 1, (point.x() + minX)*(maxX - minX));
                    xn.Set(2, 1, (point.y() + minY)*(maxY - minY));
                    transpozeW = Matrix::matrisTranspoze(weights);
                    netMatrix = Matrix::matrixMultiplication(transpozeW, xn);
                }
                if(activationFunc == "Binary Function") {
                    outPut = netValue > 0.0 ? 1 : - 1;
                    t1 = c*(diList.value(className) - outPut);

                    if(std::abs(t1) > 0) {
                        delta = xn * t1;
                        weights = weights + delta;
                        error += std::abs(t1) / 2.00;
                    }
                } else {
                    outPut = (2.0/(1.0 + qExp(-netValue*lambda))) - 1.0;
                    double deriativeFNet =  0.5 * (1- qPow(outPut, 2.0));

                    double var = c * (diList.value(className)- outPut) * deriativeFNet;
                    delta = xn * var;
                    weights = weights + (delta );
                    double errorN = qPow(diList.value(className) - outPut, 2.0) / 2.0;
                    error += errorN  ;
                }
            }
        }
        if(activationFunc == "Binary Function") {
            needTraining = error > 0.0 ? true : false;
        } else
            needTraining = error > maxErrForSigmoid ? true : false;

        needTraining = cycle > 10000 ? false : needTraining;
        cycle++;

        myQGraphicsView->clearLines();
        myQGraphicsView->drawLine(weights);
        if(activationFunc == "Binary Function")
            ui->labelCycleResultBinary->setText(QString::number(cycle));
        else
            ui->labelCycleResultSigmoid->setText(QString::number(cycle));
        delay(delayInMs);
    }

    if(cycle == 1) {
        Matrix *weightsx = (myQGraphicsView->getWeights());
        myQGraphicsView->drawLine(*weightsx);
    } else
        myQGraphicsView->drawLine(weights);
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

void MainWindow::on_activationFuncSelectComboBox_currentTextChanged(const QString &arg1)
{
    if(arg1 == "Binary Function" || arg1 == "Sigmoid Function") {
        this->activationFunc = arg1;
        this->myQGraphicsView->setActivationFunc(arg1);
    }

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
