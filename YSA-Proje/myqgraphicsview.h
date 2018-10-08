#ifndef MYQGRAPHICSVIEW_H
#define MYQGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QMouseEvent>
#include "ui_mainwindow.h"

class MyQGraphicsView : public QGraphicsView
{
    Q_OBJECT
    public:
        explicit MyQGraphicsView(QWidget *parent = nullptr, Ui::MainWindow *ui = nullptr);

    QString getSelectedClass() const;
    void setSelectedClass(const QString &value);

    QList<QString> getClassList() const;
    void setClassList(const QList<QString> &value);

    QPointF getLastPoint() const;
    void setLastPoint(const QPointF &value);

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
};

#endif // MYQGRAPHICSVIEW_H
