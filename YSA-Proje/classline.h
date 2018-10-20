#ifndef MYLINE_H
#define MYLINE_H

#include <QGraphicsEllipseItem>



class ClassLine
{
    public:
        ClassLine();
        ClassLine(QLineF *line1, QGraphicsLineItem *qGraphicsLineItem1,
               QLineF *line2, QGraphicsLineItem *qGraphicsLineItem2);

        QGraphicsLineItem *getQGraphicsLineItem1();
        void setQGraphicsLineItem1(QGraphicsLineItem *value);

        QGraphicsLineItem *getQGraphicsLineItem2();
        void setQGraphicsLineItem2(QGraphicsLineItem *value);

        QLineF *getLine1();
        void setLine1(QLineF *value);

        QLineF *getLine2();
        void setLine2(QLineF *value);

    public:
        bool operator == (const ClassLine &other);

    private:
        QGraphicsLineItem *qGraphicsLineItem1;
        QGraphicsLineItem *qGraphicsLineItem2;
        QLineF *line1;
        QLineF *line2;
};

#endif // MYLINE_H
