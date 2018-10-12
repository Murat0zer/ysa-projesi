#include "myqgraphicsview.h"
#include "myqgraphicsview.h"
#include "myqgraphicsview.h"
#include <QPointF>
#include <QRandomGenerator>


MyQGraphicsView::MyQGraphicsView(QWidget *parent, Ui::MainWindow *ui) :
    QGraphicsView(parent)
{
    this->ui = ui;
    scene = new QGraphicsScene();
    this->setSceneRect(0, 0, 900, 450);
    this->setScene(scene);
    this->clearScene();
    weights = new Matrix(3,1);
    weights->Set(1,1, QRandomGenerator::global()->generateDouble());
    weights->Set(2, 1, QRandomGenerator::global()->generateDouble());
    weights->Set(3, 1, 3);


    this->drawLine(*weights);
}

void MyQGraphicsView::drawLine(Matrix weights)
{
    this->clearScene();
    double newZ = weights.Get(3,1) / 34.0;
    double x2 = -newZ / weights.Get(2,1);
    double x1 = -newZ / weights.Get(1,1);
    QLineF *line = new QLineF(450,  225 - x2, x1 + 450  , 225);
    line->setLength(2000);
    scene->addLine(*line, QPen(Qt::black, 1, Qt::SolidLine));

    line->setLength(-2000);
    scene->addLine(*line, QPen(Qt::black, 1, Qt::SolidLine));

//    double c = 1;
//    double newZ = weights.Get(3,1) / 34.0;
//    QList<QGraphicsEllipseItem*> *points = new QList<QGraphicsEllipseItem*>;

//    for(double i = -450 ; i < 450;) {
//        ellipse = new QGraphicsEllipseItem;
//        double y = (weights.Get(1,1) * i + newZ)/ weights.Get(2,1);
//        ellipse->setRect(i + 450.0, 225 - y , 1, 1);
//        ellipse->setBrush(QBrush(QColor("black"), Qt::SolidPattern));
//        ellipse->setPen(QPen(QColor("black")));
//        this->scene->addEllipse(ellipse->rect(), ellipse->pen(), ellipse->brush());
//        points->append(ellipse);
//        i += c;
//    }

}

void MyQGraphicsView::clearScene()
{
    scene->clear();
    QGraphicsView::viewport()->update();
    this->sceneHeight = this->ui->gridWidget->height() - 24;
    this->sceneWidth = this->ui->gridWidget->width() - 24;

    scene->addLine(sceneWidth/2, 0, sceneHeight, sceneWidth/2, QPen(Qt::black, 1, Qt::SolidLine));
    scene->addLine(0, sceneHeight/2, sceneWidth, sceneHeight/2, QPen(Qt::black, 1, Qt::SolidLine));
    this->sceneWidth = this->ui->gridWidget->width() - 28;

    // mevcut noktalar geri ekleniyor.
    for(QString className : this->getClassList()) {
        QList<QPointF> points = classPoints.value(className);
        for(QPointF point : points) {
            QColor *classColor = new QColor(classColors.value(className));
            double rad = 2;
            scene->addEllipse(point.x() + sceneWidth/2 , sceneHeight/2 - point.y() , rad*2.0, rad*2.0,
                              QPen(*classColor), QBrush(*classColor, Qt::SolidPattern));
        }

    }
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

    QString xPos = QString::number(this->lastPoint.x() - this->sceneWidth/2);
    QString yPos = QString::number(this->sceneHeight/2 - this->lastPoint.y());
    ui->labelPointCords->setText(xPos + "," + yPos);

    QPointF myPoint(xPos.toDouble(), yPos.toDouble());
    QList<QPointF> pointList = this->classPoints.value(this->selectedClass);
    pointList.append(myPoint);
    this->classPoints.insert(this->selectedClass, pointList);
}

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

