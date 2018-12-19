#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QRect>
#include "myqgraphicsview.h"
#include <QtMath>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    QList<QString> getClassNames();
    QString getSelectedClass();
    void setSelectedClass(const QString &value);

    void computeMaxMinForNormalize(QList<QPointF> points);
    void getWeightList();
    void updateClassList();
    void updateDiList(QString className);
    void multiLayerTrain();

private slots:
    void on_classSelectComboBox_currentTextChanged(const QString &arg1);
    void on_pushButtonTrain_clicked();
    void on_pushButtonClear_clicked();

    void on_activationFuncSelectComboBox_currentTextChanged(const QString &arg1);

    void on_commandLinkButton_clicked();

private:

    Ui::MainWindow *ui;
    QString selectedClass;
    QList<QString> classList;
    MyQGraphicsView * myQGraphicsView;
    QString activationFunc;

    QMap<QString, double> diList;
    QMap<QString, Matrix*> *weightList;
    double maxX, minX, maxY, minY;


};

#endif // MAINWINDOW_H
