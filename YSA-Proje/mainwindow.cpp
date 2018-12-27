#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "statistics.cpp"
#include <QTime>
#include <QDebug>

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
    myQGraphicsView->randomMultiLayerWeights();
    error = 0.0;
    myQGraphicsView->randomMultiLayerWeights();

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
            delete weightsBeginning;

        }
    } else {
        Matrix *weightsBeginning = (myQGraphicsView->getWeightList().value("Class 1"));
        Matrix weights(3, 1);
        weights.Set(1,1, weightsBeginning->Get(1,1));
        weights.Set(2,1, weightsBeginning->Get(2,1));
        weights.Set(3,1, weightsBeginning->Get(3,1));
        this->weightList->insert("Class 2", new Matrix(weights));
        this->weightList->insert("Class 1", new Matrix(weights));

        delete weightsBeginning;
    }
}


void MainWindow::updateDiList(QString className) {
    this->diList.clear();
    int i = 1;
    for(QString classN : this->classList)
        diList.insert("Class " +QString::number(i++), -1.0);
    diList.remove(className);
    diList.insert(className, 1.0);
}

void MainWindow::normalize(QList<QPointF*> points, QPointF *mean, QPointF *standartDeviation) {
    double meanX = 0;
    double meanY = 0;
    double sumX = 0;
    double sumY = 0;
    for(QPointF *point : points){
        sumX += point->rx();
        sumY += point->ry();
    }
    meanX = sumX / points.size();
    meanY = sumY / points.size();
    QPointF meanPoint;
    meanPoint.setX(meanX); meanPoint.setY(meanY);
    QPointF variance(0,0);

    for(QPointF *point : points){
        variance.setX(variance.rx() + pow(point->rx() - meanPoint.rx(), 2));
        variance.setY(variance.ry() + pow(point->ry() - meanPoint.ry(), 2));
    }
    standartDeviation->setX(sqrt(variance.rx()));
    standartDeviation->setY(sqrt(variance.ry()));
    mean->setX(meanX);
    mean->setY(meanY);

}


void MainWindow::multiLayerTrain() {
    myQGraphicsView->clearLines();

    delay(500);
    bool needTraining = true;
    int cycle = 0;

    double c = ui->lineEditC->text().toDouble();
    double c2 = ui->lineEditC2->text().toDouble();
    double lambda = ui->lineEditLambda->text().toDouble();
    double maxErrForSigmoid = ui->lineEditErorForSigmoid->text().toDouble();

    Matrix z(3, 1);
    Matrix *V = new Matrix(this->myQGraphicsView->getV());
    Matrix *W = new Matrix(this->myQGraphicsView->getW());
    Matrix netJ(V->getRow(), z.getColumn());
    Matrix fNetJ(V->getRow(), z.getColumn());
    Matrix y(fNetJ.getRow() + 1, fNetJ.getColumn());
    Matrix netK(W->getRow(), y.getColumn());
    Matrix fNetK(W->getRow(), y.getColumn());

    Matrix deltaWT_1(W->getRow(), W->getColumn());
    Matrix deltaVT_1(V->getRow(), V->getColumn());


    QPointF *mean = new QPointF; QPointF *standartDeviation = new QPointF;
    QList<QPointF*> allPoints;
    int patternCount = 0;
    if(ui->checkBoxNormalize->isChecked()) {


        for(QList<QPointF> classPointList : myQGraphicsView->getClassPoints().values())
            for(QPointF point : classPointList)
                allPoints.append(new QPointF(point));
        normalize(allPoints, mean, standartDeviation);
        patternCount = allPoints.size();
    }

    for(QPointF *point : allPoints)
        delete  point;
    int delayInMs = 0.0;

    while (needTraining) {
        this->error = 0.0;
        for(QString className : myQGraphicsView->getClassList()) {

            QList<QPointF> points = myQGraphicsView->getClassPoints().value(className);
            this->updateDiList((className));

            for(QPointF point : points) {

                z.Set(1, 1, point.rx());
                z.Set(2, 1, point.ry());
                z.Set(3, 1, -1);
                if(ui->checkBoxNormalize->isChecked()) {
                    z.Set(1, 1, (point.rx() - mean->rx()) / standartDeviation->rx());
                    z.Set(2, 1, (point.ry() - mean->ry()) / standartDeviation->ry());
                }
                netJ = Matrix::matrixMultiplication(*V, z);
                for(int i=0; i<netJ.getRow(); i++){
                    //double fNet = (2.0/(1.0 + qExp(-netJ.Get(i+1, 1) * lambda))) - 1.0;
                    double fNet = tanh(netJ.Get(i+1, 1));
                    fNetJ.Set(i+1, 1, fNet);
                }
                for(int l = 0; l< fNetJ.getRow(); l++)
                    for(int n = 0; n<fNetJ.getColumn(); n++)
                        y.Set(l+1, n+1, fNetJ.Get(l+1,n+1));
                y.Set(y.getRow(), y.getColumn(), -1);

                netK = Matrix::matrixMultiplication(*W, y);

                for(int i=0; i< netK.getRow(); i++){
                    //double fNet = (2.0 / (1.0 + qExp(-netK.Get(i+1, 1) * lambda))) - 1.0;
                    double fNet = tanh(netK.Get(i+1, 1));
                    fNetK.Set(i + 1, 1, fNet);
                }

                Matrix fNetKDeriative(fNetK.getRow(), fNetK.getColumn());

                Matrix fNetJDeriative(fNetJ.getRow(), fNetJ.getColumn());


                for (int i =1; i<= fNetK.getRow(); i++) {
                    //double deriativeFNet =  0.5 * (1 - qPow(fNetK.Get(i, 1), 2.0));
                    double deriativeFNet =  1 - qPow(tanh(fNetK.Get(i, 1)), 2);
                    fNetKDeriative.Set(i, 1, deriativeFNet);
                }

                for (int i =1; i<= fNetJ.getRow(); i++) {
                    // double deriativeFNet =  0.5 * (1 - qPow(fNetJ.Get(i, 1), 2.0));
                    double deriativeFNet =  1 - qPow(tanh(fNetJ.Get(i, 1)), 2);
                    fNetJDeriative.Set(i, 1, deriativeFNet);
                }
                //=================================================================================================
                int k = 0;
                Matrix *deltaW = new Matrix(W->getRow(), W->getColumn());
                for(QString clsName : myQGraphicsView->getWeightList().keys()){
                    double var = diList.value(clsName) - fNetK.Get(k+1, 1);
                    for(int j=0; j< W->getColumn(); j++) {

                        double var2 = var * fNetKDeriative.Get(k+1, 1) * y.Get(j + 1, 1) * c2;

                        deltaW->Set(k +1, j + 1, var2);
                    }
                    k++;
                }

                *W = *W + deltaW + deltaWT_1;
                deltaWT_1 = deltaW;
                delete deltaW;
                //=====================================================================================================================

                Matrix *deltaV = new Matrix(V->getRow(), V->getColumn());
                for(int j=0; j < V->getRow(); j++){
                    for(int i=0; i< V->getColumn(); i++) {
                        double var = 0.0;
                        k = 0;
                        for(QString clsName : myQGraphicsView->getWeightList().keys()){
                            var += (diList.value(clsName) - fNetK.Get(k + 1, 1))
                                    * fNetKDeriative.Get(k+1, 1) * W->Get(k + 1, j + 1);
                            k++;
                        }

                        double var2 = c * fNetJDeriative.Get(j+1, 1) * z.Get(i + 1, 1) *  var;

                        deltaV->Set(j + 1, i + 1, var2);
                    }
                }
                *V = *V + deltaV + deltaVT_1;
                deltaVT_1 = deltaV;
                delete deltaV;
            }
        }

        double errorN = 0;
        for(QString clsNameForUpdate : myQGraphicsView->getWeightList().keys()){
            this->updateDiList(clsNameForUpdate);
            for(QPointF point : myQGraphicsView->getClassPoints().value(clsNameForUpdate)){
                int k = 0;
                for(QString clsName : myQGraphicsView->getWeightList().keys()){
                    errorN = qPow(diList.value(clsName) - fNetK.Get(k+1, 1), 2);
                    k++;
                }
            }
        }
        error += sqrt(errorN)/ (this->classList.size() * patternCount);

        needTraining = error > maxErrForSigmoid ? true : false;
        needTraining = cycle > 10000 ? false : needTraining;
        cycle++;

        ui->label_error->setText(QString::number(error));
    }
    ui->labelCycleResultSigmoid->setText(QString::number(cycle));

    this->myQGraphicsView->setW(W);
    this->myQGraphicsView->setV(V);
    myQGraphicsView->testNetwork(*W, *V, mean, standartDeviation);

    delete mean;
    delete standartDeviation;

}

void MainWindow::on_pushButtonTrain_clicked()
{
    if(activationFunc == "Binary Function")
        ui->labelCycleResultBinary->setText("0");
    else
        ui->labelCycleResultSigmoid->setText("0");

    if(ui->checkBoxMultiLayer->isChecked()){
        multiLayerTrain();
        return;
    }
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

    QPointF *mean = new QPointF; QPointF *standartDeviation = new QPointF;

    if(ui->checkBoxNormalize->isChecked()) {
        QList<QPointF*> allPoints;
        for(QList<QPointF> classPointList : myQGraphicsView->getClassPoints().values())
            for(QPointF point : classPointList)
                allPoints.append(new QPointF(point));
        normalize(allPoints, mean, standartDeviation);
    }
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

            this->updateDiList(className);

            for(QPointF point : points) {
                for(Matrix *weights : weightList->values()) {
                    xn.Set(1, 1, point.rx());
                    xn.Set(2, 1, point.ry());
                    xn.Set(3, 1, 1);
                    if(ui->checkBoxNormalize->isChecked()) {
                        xn.Set(1, 1, (point.rx()-mean->rx())/standartDeviation->rx());
                        xn.Set(2, 1, (point.ry()-mean->ry())/standartDeviation->ry());
                    }
                    transpozeW = Matrix::matrisTranspoze(*weights);
                    netMatrix = Matrix::matrixMultiplication(transpozeW, xn);
                    netValue = netMatrix.Get(1, 1);


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
    this->myQGraphicsView->randomMultiLayerWeights();


}

double MainWindow::getError() const
{
    return error;
}

void MainWindow::setError(double value)
{
    error = value;
}
