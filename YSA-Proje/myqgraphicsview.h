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
#include "classline.h"
#include <QtMath>


class MyQGraphicsView : public QGraphicsView
{
    Q_OBJECT
    public:
        explicit MyQGraphicsView(QWidget *parent = nullptr, Ui::MainWindow *ui = nullptr);
        ~MyQGraphicsView() override;

        void drawRandomLine();

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

        void drawLine(QMap<QString, Matrix*> *weightList);

        void clearLines();
        void clearPoints();


        QMap<QString, ClassLine> getMyLines();
        void setMyLines(QMap<QString, ClassLine> &value);


        QString getActivationFunc() const;
        void setActivationFunc(const QString &value);

        QMap<QString, Matrix *> getWeightList() const;
        void setWeightList(const QMap<QString, Matrix *> &value);

        QList<Matrix *> *getAraKatmanWeights() const;
        void setAraKatmanWeights(QList<Matrix *> *value);

        Matrix *getV() const;
        void setV(Matrix *value);

        Matrix *getW() const;
        void setW(Matrix *value);

        void randomMultiLayerWeights();

        void normalize(QList<QPointF *> points, QPointF *mean, QPointF *standartDeviation);

        void testNetwork(Matrix W, Matrix V, QPointF *mean, QPointF *standartDeviation);
private:
        void generateColors();
        void drawOrigin();



signals:

public slots:
        void mousePressEvent(QMouseEvent * e) override;

private slots:
        void on_commandLinkButton_clicked();

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
            QGraphicsEllipseItem *ellipse;
            QMap<QString, ClassLine> disctrimantFuns;
            QList<ClassLine> allLines;
            QString activationFunc;

            QMap<QString, Matrix*> *weightList;
            QList<Matrix*> *araKatmanWeights;

            Matrix *V;
            Matrix *W;

            QList<QGraphicsEllipseItem*> *testPointGraphicItemList;


};

#endif // MYQGRAPHICSVIEW_H
