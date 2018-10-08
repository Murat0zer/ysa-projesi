#include "myqgraphicsview.h"
#include <QPointF>
#include "utils.h"

MyQGraphicsView::MyQGraphicsView(QWidget *parent, Ui::MainWindow *ui) :
    QGraphicsView(parent)
{
    this->ui = ui;
    scene = new QGraphicsScene();
    this->setSceneRect(0, 0, 900, 450);
    this->setScene(scene);
    this->sceneHeight = this->ui->gridWidget->height() - 24;
    this->sceneWidth = this->ui->gridWidget->width() - 24;
    scene->addLine(sceneWidth/2, 0, sceneHeight, sceneWidth/2, QPen(Qt::black, 1, Qt::SolidLine));
    scene->addLine(0, sceneHeight/2, sceneWidth, sceneHeight/2, QPen(Qt::black, 1, Qt::SolidLine));
    this->sceneWidth = this->ui->gridWidget->width() - 28;
}

void MyQGraphicsView::mousePressEvent(QMouseEvent * e)
{
    if(this->selectedClass== "Class 1") {
        double rad = 2;
        this->lastPoint = mapToScene(e->pos());
        scene->addEllipse(lastPoint.x(), lastPoint.y() , rad*2.0, rad*2.0,
            QPen(QColor("red")), QBrush(QColor("red"), Qt::SolidPattern));
    }

    if(this->selectedClass== "Class 2") {
        double rad = 2;
        this->lastPoint = mapToScene(e->pos());
        scene->addEllipse(this->lastPoint.x(), this->lastPoint.y(), rad*2.0, rad*2.0,
            QPen(QColor("blue")), QBrush(QColor("blue"), Qt::SolidPattern));
    }

    QString xPos = QString::number(this->lastPoint.x() - this->sceneWidth/2 - 1);
    QString yPos = QString::number(this->sceneHeight/2 - this->lastPoint.y() - 1);
    ui->labelPointCords->setText(xPos + "," + yPos);

    QPointF myPoint(xPos.toDouble(), yPos.toDouble());
    QList<QPointF> pointList = this->classPoints.value(this->selectedClass);
    pointList.append(myPoint);
    this->classPoints.insert(this->selectedClass, pointList);
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
