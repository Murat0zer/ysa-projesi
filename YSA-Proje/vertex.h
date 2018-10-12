#ifndef VERTEX_H
#define VERTEX_H

#include <math.h>
#include "math.h"


class Vertex
{
public:
    Vertex();
    Vertex(double x, double y, double z);
    // copy const
//	Vertex(const  Vertex & p);
    Vertex(Vertex * p);

    Vertex  Normalize();
    double  Length();
    Vertex  CrossProduct(Vertex p);
    Vertex  operator + (Vertex p);
    Vertex  operator - (Vertex p);
    double   operator * (Vertex p);
    Vertex  operator * (double f);
    Vertex  operator / (double f);
    Vertex  operator = (Vertex p);
    bool	operator < (Vertex v1);

    //setters and getters
    void   setX(double x);
    void   setY(double y);
    void   setZ(double z);
    double  getX();
    double  getY();
    double  getZ();
private:
    double X;
    double Y;
    double Z;

};
Vertex operator * (double f, Vertex p);



#endif // VERTEX_H
