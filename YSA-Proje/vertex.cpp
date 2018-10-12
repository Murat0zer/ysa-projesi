#include "vertex.h"


Vertex::Vertex(){
    this->X = 0.00;
    this->Y = 0.00;
    this->Z = 0.00;
}

Vertex::Vertex(double x, double y, double z) {
    this->X = x;
    this->Y = y;
    this->Z = z;
}

Vertex::Vertex(Vertex * p)
{
    X = p->getX();
    Y = p->getY();
    Z = p->getZ();
}
Vertex  Vertex::Normalize()
{
    double length = std::sqrt(X*X + Y*Y + Z*Z);
    X /= length;
    Y /= length;
    Z /= length;
    return *this;
}

double  Vertex::Length()
{
    return std::sqrt(X*X + Y*Y + Z*Z);;
}

Vertex  Vertex::CrossProduct(Vertex p)
{
    return  Vertex(
                Y*p.Z - Z*p.Y,
                Z*p.X - X*p.Z,
                X*p.Y - Y*p.X);
}

Vertex  Vertex::operator + (Vertex p)
{
    return Vertex(X + p.X, Y + p.Y, Z + p.Z);
}

Vertex  Vertex::operator - (Vertex p)
{
    return Vertex(X - p.X, Y - p.Y, Z - p.Z);
}

double  Vertex::operator * (Vertex p)
{
    return X * p.X + Y * p.Y + Z * p.Z;
}

Vertex  Vertex::operator * (double f)
{
    return Vertex(f*X, f*Y, f*Z);
}

Vertex  Vertex::operator / (double f)
{
    return  Vertex(X / f, Y / f, Z / f);
}

Vertex  Vertex::operator=(Vertex p)
{
    this->X = p.getX();
    this->Y = p.getY();
    this->Z = p.getZ();
    return *this;
}

bool Vertex::operator <	(Vertex v1)
{
    return  (this->getX() - v1.getX()) > 0;
}

Vertex  operator * (double f, Vertex p)
{
    return Vertex(f * p.getX(), f * p.getY(), f * p.getZ());
}
//// getters-setters////
void Vertex::setX(double x)
{
    this->X = x;
}

void Vertex::setY(double y)
{
    this->Y = y;
}

void Vertex::setZ(double z)
{
    this->Z = z;
}

double  Vertex::getX()
{
    return X;
}

double  Vertex::getY()
{
    return this->Y;
}

double  Vertex::getZ()
{
    return this->Z;
}
