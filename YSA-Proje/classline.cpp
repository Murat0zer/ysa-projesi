#include "classline.h"

ClassLine::ClassLine()
{

}

ClassLine::ClassLine(QLineF *line1, QGraphicsLineItem *qGraphicsLineItem1, QLineF *line2, QGraphicsLineItem *qGraphicsLineItem2)
{
    this->line1 = line1;
    this->qGraphicsLineItem1 = qGraphicsLineItem1;
    this->line2 = line2;
    this->qGraphicsLineItem2 = qGraphicsLineItem2;
}

QGraphicsLineItem *ClassLine::getQGraphicsLineItem1()
{
    return qGraphicsLineItem1;
}

void ClassLine::setQGraphicsLineItem1(QGraphicsLineItem *value)
{
    qGraphicsLineItem1 = value;
}

QGraphicsLineItem *ClassLine::getQGraphicsLineItem2()
{
    return qGraphicsLineItem2;
}

void ClassLine::setQGraphicsLineItem2(QGraphicsLineItem *value)
{
    qGraphicsLineItem2 = value;
}

QLineF *ClassLine::getLine1()
{
    return line1;
}

void ClassLine::setLine1(QLineF *value)
{
    line1 = value;
}

QLineF *ClassLine::getLine2()
{
    return line2;
}

void ClassLine::setLine2(QLineF *value)
{
    line2 = value;
}

bool ClassLine::operator ==(const ClassLine &other)
{
    return this->line1 == other.line1
           && this->line2 == other.line2
           && this->qGraphicsLineItem1 == other.qGraphicsLineItem1
           && this->qGraphicsLineItem2 == other.qGraphicsLineItem2;
}
