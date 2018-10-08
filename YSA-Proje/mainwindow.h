#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGridLayout>
#include <QRect>
#include "myqgraphicsview.h"

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
private slots:
    void on_classSelectComboBox_currentTextChanged(const QString &arg1);

public slots:
    void mousePressEvent(QMouseEvent * e) override;

private:
    Ui::MainWindow *ui;
    QString selectedClass;
    QList<QString> classList;
    MyQGraphicsView * myQGraphicsView;

};

#endif // MAINWINDOW_H
