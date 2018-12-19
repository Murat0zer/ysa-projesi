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

void normalize(QList<QPointF*> points, QPointF *mean, QPointF *standartDeviation) {
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

    myQGraphicsView->getWeightList().clear();
    myQGraphicsView->getAraKatmanWeights()->clear();
    myQGraphicsView->randomMultiLayerWeights();

    delay(500);
    bool needTraining = true;
    int cycle = 0;

    double c = ui->lineEditC->text().toDouble();
    double lambda = ui->lineEditLambda->text().toDouble();
    double maxErrForSigmoid = ui->lineEditErorForSigmoid->text().toDouble();


    Matrix z(3, 1);
    Matrix *V = this->myQGraphicsView->getV();
    Matrix *W = this->myQGraphicsView->getW();
    Matrix netJ(V->getRow(), z.getColumn());
    Matrix fNetJ(V->getRow(), z.getColumn());
    Matrix y(fNetJ.getRow() + 1, fNetJ.getColumn());
    Matrix netK(W->getRow(), y.getColumn());
    Matrix fNetZ(W->getRow(), y.getColumn());



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
        for(QString className : myQGraphicsView->getClassList()) {

            QList<QPointF> points = myQGraphicsView->getClassPoints().value(className);

            for(QPointF point : points) {

                z.Set(1, 1, point.rx());
                z.Set(2, 1, point.ry());
                z.Set(3, 1, 1);
                if(ui->checkBoxNormalize->isChecked()) {
                    z.Set(1, 1, (point.rx()-mean->rx())/standartDeviation->rx());
                    z.Set(2, 1, (point.ry()-mean->ry())/standartDeviation->ry());
                }
                netJ = Matrix::matrixMultiplication(*V, z);
                for(int i=0; i<netJ.getRow(); i++){
                    double fNet = (2.0/(1.0 + qExp(-netJ.Get(i+1, 1) * lambda))) - 1.0;
                    fNetJ.Set(i+1, 1, fNet);
                }

                y = new Matrix(fNetJ.getRow() + 1, fNetJ.getColumn());
                y.Set(y.getRow(), y.getColumn(), 1);
                netK = Matrix::matrixMultiplication(*W, y);

                for(int i=0; i< netK.getRow(); i++){
                    double fNet = (2.0/(1.0 + qExp(-netK.Get(i+1, 1) * lambda))) - 1.0;
                    fNetZ.Set(i+1, 1, fNet);
                }


                for(Matrix *weights : myQGraphicsView->getWeightList().values()) {
                    this->updateDiList(weightList->key(weights));
                    for(int j = 0; j < W->getRow(); j++)
                        error += qPow(diList.value(weightList->key(weights)) - fNetZ.Get(j, 1), 2);
                }

                error /= 2.0;
                Matrix fNetZDeriative(fNetZ.getRow(), fNetZ.getColumn());

                Matrix fNetJDeriative(fNetJ.getRow(), fNetJ.getColumn());


                for (int i =1; i<= fNetZ.getRow(); i++) {
                    double deriativeFNet =  0.5 * (1 + qPow(fNetZ.Get(i, 1), 2.0));
                    fNetZDeriative.Set(i, 1, deriativeFNet);
                }

                for (int i =1; i<= fNetJ.getRow(); i++) {
                    double deriativeFNet =  0.5 * (1 + qPow(fNetJ.Get(i, 1), 2.0));
                    fNetJDeriative.Set(i, 1, deriativeFNet);
                }

                int k = 0; int o = 1;
                Matrix *deltaW = new Matrix(W->getRow(), W->getColumn());
                for(Matrix *weights : myQGraphicsView->getWeightList().values()){
                    this->updateDiList(weightList->key(weights));
                    for(int j=0; j< W->getColumn(); j++) {
                        double var = 0.0;
                        for(Matrix *weights : myQGraphicsView->getWeightList().values()){
                            var = diList.value(weightList->key(weights)) - fNetJ.Get(o++, 1);
                        }

                        var *= fNetJDeriative.Get(j+1, 1) * y.Get(j, 1) * c;

                        deltaW->Set(k +1, j+1, var);
                    }
                    k++;
                }
                *W = *W + deltaW;

                k = 0; o = 1; // bu kisim kaldi duzelt.
                Matrix *deltaV = new Matrix(V->getRow(), V->getColumn());
                for(Matrix *weights : *myQGraphicsView->getAraKatmanWeights()){
                    this->updateDiList(weightList->key(weights));
                    for(int j=0; j< V->getColumn(); j++) {
                        double var = 0.0;
                        for(Matrix *weights : myQGraphicsView->getWeightList().values()){
                            var = diList.value(weightList->key(weights)) - fNetZ.Get(o++, 1);
                        }

                        var *= fNetZDeriative.Get(j+1, 1) * y.Get(j + 1, 1) * c;

                        deltaV->Set(k +1, j+1, var);
                    }
                    k++;
                }
                *V = *V + deltaV;


            }

            needTraining = error > maxErrForSigmoid ? true : false;
            needTraining = cycle > 1000 ? false : needTraining;
            cycle++;

            ui->labelCycleResultSigmoid->setText(QString::number(cycle));
            delay(delayInMs);
        }
    }
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


    }
