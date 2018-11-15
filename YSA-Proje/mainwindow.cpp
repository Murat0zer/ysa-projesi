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
    weightList = new QMap<QString, Matrix*>;
    myQGraphicsView = new MyQGraphicsView(nullptr, ui);
    this->updateClassList();
    ui->gridLayout->addWidget(myQGraphicsView);


}

void MainWindow::updateClassList() {
    int i = 1; double a = 1.0;
    this->classList.clear();
    while (true) {
        if(this->ui->classSelectComboBox->itemText(i) != "") {
            this->classList.append(ui->classSelectComboBox->itemText(i));
            diList.insert(ui->classSelectComboBox->itemText(i++), a);
            a = -a;
        } else
            break;
    }
    myQGraphicsView->setClassList(this->getClassNames());
}

void delay(int ms)
{
    QTime dieTime= QTime::currentTime().addMSecs(ms);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void MainWindow::computeMaxMinForNormalize(QList<QPointF> points) {

    QList<double> xValues;
    QList<double> yValues;
    for (QPointF pointQ: points) {
        xValues.append(pointQ.rx() );
        yValues.append(pointQ.ry());
    }
    this->maxX = *std::max_element(xValues.begin(), xValues.end());
    this->minX = *std::min_element(xValues.begin(), xValues.end());
    this->maxY = *std::max_element(yValues.begin(), yValues.end());
    this->minY = *std::min_element(yValues.begin(), yValues.end());
}

void MainWindow::getWeightList() {

    this->weightList->clear();
    if(this->getClassNames().size() > 2 ) {
        for(QString className : this->getClassNames() ){
            Matrix *weightsBeginning = (myQGraphicsView->getWeightList().value(className));
            Matrix weights(3, 1);
            weights.Set(1, 1, weightsBeginning->Get(1, 1));
            weights.Set(2, 1, weightsBeginning->Get(2, 1));
            weights.Set(3, 1, weightsBeginning->Get(3, 1));
            this->weightList->insert(className, new Matrix(weights));
        }
    } else {
        Matrix *weightsBeginning = (myQGraphicsView->getWeightList().value("Class 1"));
        Matrix weights(3, 1);
        weights.Set(1,1, weightsBeginning->Get(1,1));
        weights.Set(2,1, weightsBeginning->Get(2,1));
        weights.Set(3,1, weightsBeginning->Get(3,1));
        this->weightList->insert("Class 2", new Matrix(weights));
        this->weightList->insert("Class 1", new Matrix(weights));

    }
}


void MainWindow::updateDiList(QString className) {
    this->diList.clear();
    int i = 1;
    for(QString classN : this->classList)
        diList.insert("Class " +QString::number(i++), -1);
    diList.remove(className);
    diList.insert(className, 1);
}

void MainWindow::on_pushButtonTrain_clicked()
{
    if(activationFunc == "Binary Function")
        ui->labelCycleResultBinary->setText("0");
    else
        ui->labelCycleResultSigmoid->setText("0");


    myQGraphicsView->clearLines();
    this->getWeightList();
    myQGraphicsView->drawLine(this->weightList);
    delay(500);

    bool needTraining = true;
    int cycle = 0;
    double netValue;
    double c = ui->lineEditC->text().toDouble();
    double lambda = ui->lineEditLambda->text().toDouble();
    double maxErrForSigmoid = ui->lineEditErorForSigmoid->text().toDouble();
    Matrix netMatrix(1, 1);
    Matrix transpozeW(1, 3);
    Matrix xn(3, 1);
    while (needTraining) {
        double error = 0;
        int delayInMs = ui->lineEditDelay->text().toInt();
        Matrix weights(3, 1);
        for(QString className : myQGraphicsView->getClassList()) {

            QList<QPointF> points = myQGraphicsView->getClassPoints().value(className);
            Matrix weights(3, 1);
            weights.Set(1,1, (this->weightList->value(className)->Get(1,1)));
            weights.Set(2,1, (this->weightList->value(className)->Get(2,1)));
            weights.Set(3,1, (this->weightList->value(className)->Get(3,1)));

            computeMaxMinForNormalize(points);
            this->updateDiList(className);

            for(QPointF point : points) {
                for(Matrix *weights : weightList->values()) {                    
                    xn.Set(1, 1, point.rx());
                    xn.Set(2, 1, point.ry());
                    xn.Set(3, 1, 1);
                    if(ui->checkBoxNormalize->isChecked()) {
                        xn.Set(1, 1, (point.rx() - minX)/(maxX - minX));
                        xn.Set(2, 1, (point.ry() - minY)/(maxY - minY));
                    }

                    transpozeW = Matrix::matrisTranspoze(*weights);
                    netMatrix = Matrix::matrixMultiplication(transpozeW, xn);
                    netValue = netMatrix.Get(1, 1);

                    xn.Set(1, 1, point.rx());
                    xn.Set(2, 1, point.ry());

                    if(activationFunc == "Binary Function") {
                        double fNet = netValue > 0.0 ? 1 : - 1;
                        double var = c*(diList.value(weightList->key(weights)) - fNet);

                        if(std::abs(var) > 0) {
                            Matrix delta(3, 1); delta = xn * var;
                            *weights = *weights + delta;

                            error += std::abs(var) / 2.00;
                        }
                    } else {
                        double fNet = (2.0/(1.0 + qExp(-netValue*lambda))) - 1.0;
                        double deriativeFNet =  0.5 * (1 + qPow(fNet, 2.0));
                        double var = c * (diList.value(weightList->key(weights))- fNet) * deriativeFNet;

                        Matrix delta(3, 1); delta = xn * var;

                        *weights = *weights + delta;

                        double errorN = qPow(diList.value(weightList->key(weights)) - fNet, 2.0) / 2.0;
                        error += errorN;
                    }
                }
            }
        }
        if(activationFunc == "Binary Function") {
            needTraining = error > 0.0 ? true : false;
        } else
            needTraining = error > maxErrForSigmoid ? true : false;

        needTraining = cycle > 1000 ? false : needTraining;
        cycle++;

        myQGraphicsView->clearLines();
        myQGraphicsView->drawLine(weightList);
        if(activationFunc == "Binary Function")
            ui->labelCycleResultBinary->setText(QString::number(cycle));
        else
            ui->labelCycleResultSigmoid->setText(QString::number(cycle));
        delay(delayInMs);
    }


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

QList<QString> MainWindow::getClassNames()
{
    return this->classList;
}

void MainWindow::on_commandLinkButton_clicked()
{
    ui->classSelectComboBox->addItem("Class " + QString::number(this->classList.size() + 1));
    diList.insert("Class " +QString::number(this->classList.size() + 1), -1);
    this->classList.append("Class " + QString::number(this->classList.size() + 1));
    this->updateClassList();


}
