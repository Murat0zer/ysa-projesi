#include "myqgraphicsview.h"

static double PIXEL_TO_CM = 37.795276;

MyQGraphicsView::MyQGraphicsView(QWidget *parent, Ui::MainWindow *ui) :
    QGraphicsView(parent)
{
    this->ui = ui;
    scene = new QGraphicsScene();
    this->setSceneRect(0, 0, 900, 450);
    this->setScene(scene);
    this->drawOrigin();
    this->drawRandomLine();
}



void MyQGraphicsView::drawLine(Matrix weights)
{
//    weights.Set(3,1, weights.Get(3,1));
//    weights.Set(2,1, weights.Get(2,1));
//    weights.Set(1,1, weights.Get(1,1));
    double newZ;
    newZ = weights.Get(3,1);
//    if(this->activationFunc == "Binary Function")
//         newZ = weights.Get(3,1);
//    else
//        newZ = weights.Get(3,1) * PIXEL_TO_CM;
    double x2 =  -newZ / weights.Get(2,1) * PIXEL_TO_CM;
    double x1 =  -newZ / weights.Get(1,1) * PIXEL_TO_CM;

    QLineF *line1 = new QLineF(450,  225 - x2, x1 + 450  , 225);
    QGraphicsLineItem *qGraphicsLineItem1 = new QGraphicsLineItem(*line1);

    QLineF *line2 = new QLineF(*line1);
    QGraphicsLineItem *qGraphicsLineItem2 = new QGraphicsLineItem(*line1);

    ClassLine myLine(line1, qGraphicsLineItem1, line2, qGraphicsLineItem2);

    allLines.append(myLine);

    myLine.getLine1()->setLength(this->sceneHeight * 20000000);
    myLine.getQGraphicsLineItem1()->setPen( QPen(Qt::black, 1, Qt::SolidLine));
    myLine.getQGraphicsLineItem1()->setLine(*line1);
    scene->addItem(myLine.getQGraphicsLineItem1());


    myLine.getLine2()->setLength(-this->sceneHeight * 2000000);
    myLine.getQGraphicsLineItem2()->setPen( QPen(Qt::black, 1, Qt::SolidLine));
    myLine.getQGraphicsLineItem2()->setLine(*line2);
    scene->addItem(myLine.getQGraphicsLineItem2());



}

void MyQGraphicsView::clearLines() {

    for(ClassLine classLine : allLines) {
        scene->removeItem(classLine.getQGraphicsLineItem1());
        scene->removeItem(classLine.getQGraphicsLineItem2());
        allLines.removeAll(classLine);
    }


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
    if(this->classColors.isEmpty())
        this->generateColors();

    if(this->ui->classSelectComboBox->currentIndex() != 0) {
        QColor *classColor = new QColor(classColors.value(this->selectedClass));
        double rad = 2;
        this->lastPoint = mapToScene(e->pos());
        scene->addEllipse(lastPoint.x(), lastPoint.y() , rad*2.0, rad*2.0,
                          QPen(*classColor), QBrush(*classColor, Qt::SolidPattern));
    }

    QString xPos = QString::number((this->lastPoint.x() - this->sceneWidth / 2) / PIXEL_TO_CM);
    QString yPos = QString::number((this->sceneHeight / 2 - this->lastPoint.y()) / PIXEL_TO_CM);
    ui->labelPointCords->setText(xPos + "," + yPos);

    QPointF myPoint(xPos.toDouble(), yPos.toDouble());
    QList<QPointF> pointList = this->classPoints.value(this->selectedClass);
    pointList.append(myPoint);
    this->classPoints.insert(this->selectedClass, pointList);
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

void MyQGraphicsView::generateColors()
{
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

void MyQGraphicsView::drawRandomLine()
{
    weights = new Matrix(3,1);
    weights->Set(1, 1, QRandomGenerator::global()->generateDouble() );
    weights->Set(2, 1, QRandomGenerator::global()->generateDouble() );
    weights->Set(3, 1, 3);

    this->drawLine(*weights);

}

MyQGraphicsView::~MyQGraphicsView()
{

    delete itemToDraw;
    delete ellipse;
}

// getters setters
Matrix *MyQGraphicsView::getWeights() const
{
    return weights;
}

void MyQGraphicsView::setWeights(Matrix *value)
{
    weights = value;
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
    classList = value;
}

QString MyQGraphicsView::getSelectedClass() const
{
    return selectedClass;
}

void MyQGraphicsView::setSelectedClass(const QString &value)
{
    selectedClass = value;
}

