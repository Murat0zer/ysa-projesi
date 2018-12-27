#ifndef MATRIX_H
#define MATRIX_H


class Matrix
{
public:
    Matrix (int, int);

    void Set(int, int, double) const;

    double Get(int, int) const;

    Matrix  operator / (int n);

    Matrix  operator * (double n);

    Matrix  operator + (const Matrix & other);

    bool operator ==(const Matrix & other);

    bool operator!=(const Matrix & other);

    // copy const.
    Matrix (Matrix  &matrix);
    Matrix (Matrix *matrix);

    Matrix  operator = ( const Matrix  &other);
    Matrix  operator = ( const Matrix  *other);

    int getColumn() const;
    int getRow() const;

    static Matrix  matrixMultiplication(Matrix  matrixA, Matrix  matrixB);
    static Matrix  matrisTranspoze(Matrix  matris);
    static void  print(Matrix matrix);

private:
    int row, col;
    double **p;
};

#endif // MATRIX_H
