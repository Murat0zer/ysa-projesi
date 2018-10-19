#ifndef MYQGRAPHICSVIEW_H
#define MYQGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QMouseEvent>
#include <QPointF>
#include <QRandomGenerator>
#include "ui_mainwindow.h"
#include "vertex.h"
#include "matrix.h"

static double PIXEL_TO_CM = 37.795276;

class MyQGraphicsView : public QGraphicsView
{
    Q_OBJECT
    public:
        explicit MyQGraphicsView(QWidget *parent = nullptr, Ui::MainWindow *ui = nullptr);
        ~MyQGraphicsView() override;
        QString getSelectedClass() const;
        void setSelectedClass(const QString &value);

        QList<QString> getClassList() const;
        void setClassList(const QList<QString> &value);

        QPointF getLastPoint() const;
        void setLastPoint(const QPointF &value);



        QMap<QString, QColor> getClassColors() const;
        void setClassColors(const QMap<QString, QColor> &value);

        QMap<QString, QList<QPointF> > getClassPoints() const;
        void setClassPoints(const QMap<QString, QList<QPointF> > &value);

        void drawLine(Matrix weights);

        Matrix *getWeights() const;
        void setWeights(Matrix *value);

        void clearLines();
        void clearPoints();


private:
        void generateColors();
        void drawOrigin();
        void drawRandomLine();


signals:

public slots:
        void mousePressEvent(QMouseEvent * e) override;

private:
            Ui::MainWindow *ui;
            QGraphicsScene * scene;
            QString selectedClass;
            QList<QString> classList;
            QPointF lastPoint;
            QGraphicsLineItem* itemToDraw;
            int sceneWidth;
            int sceneHeight;
            QMap<QString, QList<QPointF>> classPoints;
            QMap<QString, QColor> classColors;
            Matrix *weights;
            QGraphicsEllipseItem *ellipse;

            QGraphicsLineItem *qGraphicsLineItem1;
            QGraphicsLineItem *qGraphicsLineItem2;
            QLineF *line1;
            QLineF *line2;

};

#endif // MYQGRAPHICSVIEW_H
