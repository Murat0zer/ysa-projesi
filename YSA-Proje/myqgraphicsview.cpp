#include "myqgraphicsview.h"


MyQGraphicsView::MyQGraphicsView(QWidget *parent, Ui::MainWindow *ui) :
    QGraphicsView(parent)
{
    this->ui = ui;
    scene = new QGraphicsScene();
    weightList = new QMap<QString, Matrix*>;
    araKatmanWeights = new QList<Matrix*>;
    this->setSceneRect(0, 0, 900, 450);
    this->setScene(scene);
    this->drawOrigin();
    testPointGraphicItemList = new QList<QGraphicsEllipseItem*>;

}



void MyQGraphicsView::drawLine(QMap<QString, Matrix*> *weightList)
{
    for(Matrix *weight : weightList->values()) {
        double newZ;
        newZ = weight->Get(3,1);
        double x2 =  -newZ / weight->Get(2,1) ;
        double x1 =  -newZ / weight->Get(1,1) ;

        QLineF *line1 = new QLineF(450,  225 - x2, x1 + 450  , 225);
        QGraphicsLineItem *qGraphicsLineItem1 = new QGraphicsLineItem(*line1);

        QLineF *line2 = new QLineF(450,  225 - x2, x1 + 450  , 225);
        QGraphicsLineItem * qGraphicsLineItem2 = new QGraphicsLineItem(*line2);

        ClassLine myLine= * new ClassLine(line1, qGraphicsLineItem1, line2, qGraphicsLineItem2);

        allLines.append(myLine);

        myLine.getLine1()->setLength(this->sceneHeight * 2000);
        myLine.getQGraphicsLineItem1()->setPen(classColors.value(weightList->key(weight)));
        myLine.getQGraphicsLineItem1()->setLine(*line1);
        scene->addItem(myLine.getQGraphicsLineItem1());

        myLine.getLine2()->setLength(- this->sceneHeight * 2000);
        myLine.getQGraphicsLineItem2()->setPen(classColors.value(weightList->key(weight)));
        myLine.getQGraphicsLineItem2()->setLine(*line2);
        scene->addItem(myLine.getQGraphicsLineItem2());
        if(weightList->size() < 3)
            break;
    }
}

void MyQGraphicsView::randomMultiLayerWeights() {
    int araKatmanAdet = ui->lineEditAraKatman->text().toInt();

    V = new Matrix(araKatmanAdet,3);
    W = new Matrix(this->classList.size(), V->getRow() + 1);

    for (int i=1; i <= V->getRow(); i++) {
        Matrix *weights = new Matrix(3, 1);
        for (int j=0; j < 2; j++)
            weights->Set(j+1, 1, QRandomGenerator::global()->generateDouble() );

        weights->Set(3, 1, 3);

        this->getAraKatmanWeights()->append(weights);

        V->Set(i,1, weights->Get(1, 1) );
        V->Set(i,2, weights->Get(2, 1) );
        V->Set(i,3, weights->Get(3, 1) );

        delete weights;
    }

    int classindex = 1;
    for(QString className : this->getClassList()){
        Matrix *weights = new Matrix(W->getColumn(), 1);
        for(int j=1; j<= W->getColumn()-1; j++) {
            weights->Set(j, 1, QRandomGenerator::global()->generateDouble() );
        }

        weights->Set(3, 1, 3 );

        weightList->insert(className, new Matrix(weights));
        for(int j=1; j<= weights->getRow(); j++) {
            W->Set(classindex, j, weights->Get(j, 1));
        }
        classindex++;
        delete weights;
    }
}

void MyQGraphicsView::testNetwork(Matrix W, Matrix V, QPointF *mean, QPointF *standartDeviation)
{
    for(QGraphicsEllipseItem *item : *testPointGraphicItemList) {
        scene->removeItem(item);
        testPointGraphicItemList->removeOne(item);
        delete item;
    }

    int incSize = 4;
    QList<QPointF*> *testPoints = new QList<QPointF*>;
    for(int i=0; i< 900; i+=incSize)
        for(int j=0; j< 450; j+=incSize) {
            QPointF *testPoint = new QPointF(i- 450, j- 225 );
            testPoints->append(testPoint);
        }

    //this->normalize(*testPoints, mean, standartDeviation);

    int i = 0; int j = 0;
    for(QPointF *point : *testPoints) {
        Matrix * normalizedInput = new Matrix(3, 1);
        normalizedInput->Set(1, 1, (point->rx() - mean->rx())/ standartDeviation->rx());
        normalizedInput->Set(2, 1, (point->ry() - mean->ry())/ standartDeviation->ry());
        //        normalizedInput->Set(2, 1, (point->ry()));
        //        normalizedInput->Set(1, 1, (point->rx()));
        normalizedInput->Set(3, 1, -1.0);

        Matrix netJ(V.getRow(), normalizedInput->getColumn());
        Matrix fNetJ(V.getRow(), normalizedInput->getColumn());
        Matrix y(fNetJ.getRow() + 1, fNetJ.getColumn());
        Matrix netK(W.getRow(), y.getColumn());
        Matrix fNetZ(W.getRow(), y.getColumn());

        netJ = Matrix::matrixMultiplication(V, *normalizedInput);
        for(int i=0; i<netJ.getRow(); i++){
            //double fNet = (2.0/(1.0 + qExp(-netJ.Get(i+1, 1) * 1))) - 1.0;
            double fNet = tanh(netJ.Get(i+1, 1));
            fNetJ.Set(i+1, 1, fNet);
        }

        y = new Matrix(fNetJ.getRow() + 1, fNetJ.getColumn());
        for(int l = 0; l< fNetJ.getRow(); l++)
            for(int n = 0; n<fNetJ.getColumn(); n++)
                y.Set(l+1, n+1, fNetJ.Get(l+1,n+1));
        y.Set(y.getRow(), y.getColumn(), -1.0);
        netK = Matrix::matrixMultiplication(W, y);

        for(int k=0; k< netK.getRow(); k++){
            //double fNet = (2.0/(1.0 + qExp(-netK.Get(k+1, 1) * 1))) - 1.0;
            double fNet = tanh(netK.Get(k+1, 1));
            fNetZ.Set(k+1, 1, fNet);
        }


        double max = 0;
        int classIndex = 0;
        for(int k =0; k < fNetZ.getRow(); k++) {
            if((fNetZ.Get(k + 1, 1)) > (max)) {
                max = fNetZ.Get(k + 1, 1);
                classIndex = k;
            }
        }

        this->setSelectedClass("Class " + QString::number(classIndex +  1));

        QColor classColor = QColor(classColors.value(this->selectedClass));
        QGraphicsEllipseItem *qGrapEllipseItem = new QGraphicsEllipseItem(i, j , 0.1, 0.1);
        QPen qPen = QPen(classColor);
        QBrush qBrush = QBrush(classColor, Qt::SolidPattern);
        qGrapEllipseItem->setPen(qPen);
        qGrapEllipseItem->setBrush(qBrush);
        scene->addItem(qGrapEllipseItem);
        testPointGraphicItemList->append(qGrapEllipseItem);

        j+=incSize;
        if(! (j < this->sceneHeight) ) {
            i+=incSize;
            j = 0;
        }
        delete normalizedInput;
    }
    for(QPointF *point : *testPoints)
        delete  point;

}

void MyQGraphicsView::normalize(QList<QPointF *> points, QPointF *mean, QPointF *standartDeviation)
{
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

void MyQGraphicsView::clearLines() {

    for(ClassLine classLine : allLines) {
        scene->removeItem(classLine.getQGraphicsLineItem1());
        scene->removeItem(classLine.getQGraphicsLineItem2());
    }
    allLines.clear();
}

void MyQGraphicsView::clearPoints()
{
    clearLines();
    scene->clear();
    QGraphicsView::viewport()->update();

    // mevcut noktalar siliniyor.
    for(QString className : this->getClassList())
        classPoints.remove(className);
    this->drawOrigin();
    this->drawRandomLine();

}

void MyQGraphicsView::mousePressEvent(QMouseEvent * e)
{
    if(this->ui->classSelectComboBox->currentIndex() != 0) {
        QColor *classColor = new QColor(classColors.value(this->selectedClass));
        double rad = 2;
        this->lastPoint = mapToScene(e->pos());
        scene->addEllipse(lastPoint.x(), lastPoint.y() , rad*2.0, rad*2.0,
                          QPen(*classColor), QBrush(*classColor, Qt::SolidPattern));
    }

    QString xPos = QString::number((this->lastPoint.x() - this->sceneWidth / 2) );
    QString yPos = QString::number((this->sceneHeight / 2 - this->lastPoint.y()) );
    ui->labelPointCords->setText(xPos + "," + yPos);

    QPointF myPoint(xPos.toDouble(), yPos.toDouble());
    QList<QPointF> pointList = this->classPoints.value(this->selectedClass);
    pointList.append(myPoint);
    this->classPoints.insert(this->selectedClass, pointList);
}

Matrix *MyQGraphicsView::getW() const
{
    return W;
}

void MyQGraphicsView::setW(Matrix *value)
{
    W = value;
}

Matrix *MyQGraphicsView::getV() const
{
    return V;
}

void MyQGraphicsView::setV(Matrix *value)
{
    V = value;
}

QList<Matrix *> *MyQGraphicsView::getAraKatmanWeights() const
{
    return araKatmanWeights;
}

void MyQGraphicsView::setAraKatmanWeights(QList<Matrix *> *value)
{
    araKatmanWeights = value;
}


void MyQGraphicsView::drawRandomLine()
{
    this->clearLines();
    this->weightList->clear();
    if(this->getClassList().size() > 2){
        for(QString className : this->getClassList()){
            Matrix *weights = new Matrix(3,1);
            weights->Set(1, 1, QRandomGenerator::global()->generateDouble() );
            weights->Set(2, 1, QRandomGenerator::global()->generateDouble() );
            weights->Set(3, 1, 3);
            weightList->insert(className, new Matrix(weights));
        }
    } else {
        Matrix weights(3,1);
        weights.Set(1, 1, QRandomGenerator::global()->generateDouble() );
        weights.Set(2, 1, QRandomGenerator::global()->generateDouble() );
        weights.Set(3, 1, 3);
        this->weightList->insert("Class 1", new Matrix(weights));
        this->weightList->insert("Class 2", new Matrix(weights));

    }
    this->drawLine(weightList);
}

void MyQGraphicsView::generateColors()
{
    this->classColors.clear();
    QColor colours[10] = {QColor("red"), QColor("blue"), QColor("green"), QColor("cyan"),
                          QColor("magenta"), QColor("darkRed"), QColor("darkCyan"),
                          QColor("darkMagenta"), QColor("darkGreen"), QColor("yellow")};
    int i = 0;
    for(QString className : this->getClassList()){
        this->classColors.insert(className, colours[i++]);
    }

}

void MyQGraphicsView::drawOrigin()
{
    this->sceneHeight = this->ui->gridWidget->height() - 24;
    this->sceneWidth = this->ui->gridWidget->width() - 24;

    scene->addLine(sceneWidth/2, 0, sceneHeight, sceneWidth/2, QPen(Qt::black, 1, Qt::SolidLine));
    scene->addLine(0, sceneHeight/2, sceneWidth, sceneHeight/2, QPen(Qt::black, 1, Qt::SolidLine));
    this->sceneWidth = this->ui->gridWidget->width() - 28;

}

QMap<QString, Matrix *> MyQGraphicsView::getWeightList() const
{
    return *weightList;
}

void MyQGraphicsView::setWeightList(const QMap<QString, Matrix *> &value)
{
    *weightList = value;
}

QString MyQGraphicsView::getActivationFunc() const
{
    return activationFunc;
}

void MyQGraphicsView::setActivationFunc(const QString &value)
{
    activationFunc = value;
}

QMap<QString, ClassLine> MyQGraphicsView::getMyLines()
{
    return disctrimantFuns;
}

void MyQGraphicsView::setMyLines(QMap<QString, ClassLine> &value)
{
    disctrimantFuns = value;
}


MyQGraphicsView::~MyQGraphicsView()
{

    delete itemToDraw;
    delete ellipse;
}

QMap<QString, QList<QPointF> > MyQGraphicsView::getClassPoints() const
{
    return classPoints;
}

void MyQGraphicsView::setClassPoints(const QMap<QString, QList<QPointF> > &value)
{
    classPoints = value;
}

QMap<QString, QColor> MyQGraphicsView::getClassColors() const
{
    return classColors;
}

void MyQGraphicsView::setClassColors(const QMap<QString, QColor> &value)
{
    classColors = value;
}


QPointF MyQGraphicsView::getLastPoint() const
{
    return lastPoint;
}

void MyQGraphicsView::setLastPoint(const QPointF &value)
{
    lastPoint = value;
}

QList<QString> MyQGraphicsView::getClassList() const
{
    return classList;
}

void MyQGraphicsView::setClassList(const QList<QString> &value)
{
    this->classList.clear();
    for(QString val : value)
        this->classList.append(val);
    this->generateColors();
    this->drawRandomLine();
}

QString MyQGraphicsView::getSelectedClass() const
{
    return selectedClass;
}

void MyQGraphicsView::setSelectedClass(const QString &value)
{
    selectedClass = value;
}

