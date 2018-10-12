#include "matrix.h"

Matrix::Matrix( Matrix & Matrix)
{
    row = Matrix.row;
    col = Matrix.col;
    p = new double*[row];
    for (int i = 0; i < row; i++)
    {
        p[i] = new double[col];
    }
    for (int i = 1; i <= Matrix.row; i++)
        for (int j = 1; j <= Matrix.col; j++)
            this->Set(i, j, Matrix.Get(i, j));
}

Matrix Matrix::operator=(const Matrix & other)
{
    row = other.row;
    col = other.col;
    p = new double*[row];
    for (int i = 0; i < row; i++)
    {
        p[i] = new double[col];
    }
    for (int i = 1; i <= other.row; i++)
        for (int j = 1; j <= other.col; j++)
            this->Set(i, j, other.Get(i, j));

    return *this;
}

Matrix::Matrix(int x, int y)
{
    row = x;
    col = y;
    p = new double*[x];
    for (int i = 0; i < x; i++)
    {
        p[i] = new double[y];
    }
}
void Matrix::Set(int a, int b, double d) const
{

    p[a-1][b-1] = d;
}

double Matrix::Get(int a, int b) const
{
    return p[a-1][b-1];
}

int Matrix::getRow() const
{
    return this->row;
}

Matrix Matrix::matrixMultiplication(Matrix matrixA, Matrix matrixB)
{
    Matrix productMatrix(matrixA.getRow(), matrixB.getColumn());
    double sum = 0; int j = 1;
    for (int i = 1; i <= matrixA.getRow(); i++)
    {
        for (j = 1; j <= matrixB.getColumn(); j++)
        {
            sum = 0;
            for (int k = 1; k <= matrixA.getColumn(); k++)
            {
                double a = matrixA.Get(i,  k);
                double b = matrixB.Get(k, j);
                double carpim = a * b;
                sum +=  carpim ;
            }
            productMatrix.Set(i, j, sum);
        }
    }
    return productMatrix;
}

Matrix Matrix::matrisTranspoze(Matrix matris)
{
    Matrix transpozeMatris(matris.getColumn(), matris.getRow());
    for(int i=1; i<=matris.getColumn(); i++)
        for (int j = 1; j <= matris.getRow(); j++)
        {
            double a = matris.Get(j, i);
            transpozeMatris.Set(i, j, matris.Get(j, i));
        }

    return transpozeMatris;

}
int Matrix::getColumn() const
{
    return this->col;
}

Matrix Matrix::operator/(int n)
{
    Matrix newMatrix(row, col);
    for (int i = 1; i <= row; i++)
        for (int j = 1; j <= col; j++)
            newMatrix.Set(i, j, this->Get(i, j) / n);
    return newMatrix;
}

Matrix Matrix::operator *(double n)
{
    Matrix newMatrix(row, col);
    for (int i = 1; i <= row; i++)
        for (int j = 1; j <= col; j++)
            newMatrix.Set(i, j, this->Get(i, j) * n);
    return newMatrix;
}

Matrix Matrix::operator +(const Matrix &other)
{
    int row = this->getRow();
    int col = this->getColumn();
    Matrix sumMatrix(row, col);
    for (int i = 1; i <= row; i++)
        for (int j = 1; j <= col; j++)
            sumMatrix.Set(i,j, this->Get(i,j) + other.Get(i,j));

    return sumMatrix;
}

bool Matrix::operator==(const Matrix & other)
{
    int row = this->getRow();
    int col = this->getColumn();
    for (int i = 1; i <= row; i++)
        for (int j = 1; j <= col; j++)
            if (this->Get(i, j) != other.Get(i, j))
                return false;

    return true;
}
bool Matrix::operator!=(const Matrix & other)
{
    return !(*this == other);

}
