#include "myqgraphicsview.h"

static double PIXEL_TO_CM = 37.795276;

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
        for (int j=0; j < 3; j++)
            weights->Set(j+1, 1, QRandomGenerator::global()->generateDouble() );

        this->getAraKatmanWeights()->append(weights);

        V->Set(i,1, weights->Get(1, 1) );
        V->Set(i,2, weights->Get(2, 1) );
        V->Set(i,3, weights->Get(3, 1) );
    }

    int classindex = 1;
    for(QString className : this->getClassList()){
        Matrix *weights = new Matrix(W->getColumn(), 1);
        for(int j=1; j<= W->getColumn(); j++) {
            weights->Set(j, 1, QRandomGenerator::global()->generateDouble() );
        }
        weightList->insert(className, new Matrix(weights));
        for(int j=1; j<= weights->getRow(); j++) {
            W->Set(classindex, j, weights->Get(j, 1));
        }
        classindex++;
    }
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

