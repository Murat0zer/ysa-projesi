#include "statistics.h"
#include "qmath.h"
#include <QVector>

class Statistics {
    QVector<double> data;
    int size;

   public:
    Statistics(QVector<double> data) {
        this->data = data;
        size = data.length();
    }

    double getMean() {
        double sum = 0.0;
        for(double a : data)
            sum += a;
        return sum/size;
    }

    double getVariance() {
        double mean = getMean();
        double temp = 0;
        for(double a :data)
            temp += (a-mean)*(a-mean);
        return temp/(size-1);
    }

    double getStdDev() {
        return qSqrt(getVariance());
    }

    void updateSize()
    {
        size = data.length() ;
    }

    void setData(const QVector<double> &value)
    {
        data = value;
    }
};




