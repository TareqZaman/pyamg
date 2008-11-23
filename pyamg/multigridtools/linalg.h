#ifndef LINALG_H
#define LINALG_H

#include <math.h>

// Overloaded routines for complex arithmetic
inline float conjugate(const float& x)
    { return x; }
inline double conjugate(const double& x)
    { return x; }
inline npy_cfloat_wrapper conjugate(const npy_cfloat_wrapper& x)
    { return npy_cfloat_wrapper(x.real, -x.imag); }
inline npy_cdouble_wrapper conjugate(const npy_cdouble_wrapper& x)
    { return npy_cdouble_wrapper(x.real, -x.imag); }

inline float real(const float& x)
    { return x; }
inline double real(const double& x)
    { return x; }
inline float real(const npy_cfloat_wrapper& x)
    { return x.real; }
inline double real(const npy_cdouble_wrapper& x)
    { return x.real; }

inline float imag(const float& x)
    { return 0.0; }
inline double imag(const double& x)
    { return 0.0; }
inline float imag(const npy_cfloat_wrapper& x)
    { return x.imag; }
inline double imag(const npy_cdouble_wrapper& x)
    { return x.imag; }

inline float mynorm(const float& x)
    { return fabs(x); }
inline double mynorm(const double& x)
    { return fabs(x); }
inline float mynorm(const npy_cfloat_wrapper& x)
    { return sqrt(x.real*x.real + x.imag*x.imag); }
inline double mynorm(const npy_cdouble_wrapper& x)
    { return sqrt(x.real*x.real + x.imag*x.imag); }

inline float mynormsq(const float& x)
    { return (x*x); }
inline double mynormsq(const double& x)
    { return (x*x); }
inline float mynormsq(const npy_cfloat_wrapper& x)
    { return (x.real*x.real + x.imag*x.imag); }
inline double mynormsq(const npy_cdouble_wrapper& x)
    { return (x.real*x.real + x.imag*x.imag); }


//Dense Algebra Routines

/* dot(x, y, n)
 * x,y are n-vectors
 * calculate conjuate(x).T y
*/
template<class I, class T>
T dot_prod(const T x[], const T y[], const I n)
{
    T sum = 0.0;
    for( I i = 0; i < n; i++)
    {   sum += conjugate(x[i])*y[i]; }
    return sum;
}

/* axpy(x, y, alpha, n)
 * x, y are n-vectors
 * alpha is a constant scalar
 * calculate x = x + alpha*y
*/
template<class I, class T>
void axpy(T x[], const T y[], const T alpha, const I n)
{
    for( I i = 0; i < n; i++)
    {   x[i] += alpha*y[i]; }
}

/*
 * Compute A*B ==> S
 *
 * Parameters:
 * A      -  Left operand in row major
 * B      -  Right operand in column major
 * S      -  A*B, in row-major
 * Atrans -  Whether to transpose A before multiply
 * Btrans -  Whether to transpose B before multiply
 * Strans -  Whether to transpose S after multiply, Outputted in row-major         
 *
 * Returns:
 *  S = A*B
 *
 * Notes:
 *    Not fully implemented, 
 *    - Atrans and Btrans not implemented
 *    - No error checking on inputs
 *
 */

template<class I, class T>
void gemm(const T Ax[], const I Arows, const I Acols, const char Atrans, 
          const T Bx[], const I Brows, const I Bcols, const char Btrans, 
          T Sx[], const I Srows, const I Scols, const char Strans)
{
    //Add checks for dimensions, but leaving them out speeds things up
    //Add functionality for transposes

    if(Strans == 'T')
    {
        I s_counter = 0; I a_counter =0; I b_counter =0; I a_start = 0;
        for(I i = 0; i < Arows; i++)
        {
            s_counter = i;
            b_counter = 0; 
            for(I j = 0; j < Bcols; j++)
            {
                Sx[s_counter] = 0.0;
                a_counter = a_start;
                for(I k = 0; k < Brows; k++)
                {
                    //S[i,j] += Ax[i,k]*B[k,j]
                    Sx[s_counter] += Ax[a_counter]*Bx[b_counter];
                    a_counter++; b_counter++;
                }
                s_counter+=Scols;
            }
            a_start += Acols;
        }
    }
    else if(Strans == 'F')
    {
        I s_counter = 0; I a_counter =0; I b_counter =0; I a_start = 0;
        for(I i = 0; i < Arows; i++)
        {
            b_counter = 0; 
            for(I j = 0; j < Bcols; j++)
            {
                Sx[s_counter] = 0.0;
                a_counter = a_start;
                for(I k = 0; k < Brows; k++)
                {
                    //S[i,j] += A[i,k]*B[k,j]
                    Sx[s_counter] += Ax[a_counter]*Bx[b_counter];
                    a_counter++; b_counter++;
                }
                s_counter++;
            }
            a_start += Acols;
        }
    }
}



// *gelss calculates the min norm solution, using the SVD, 
//   of a rectangular matrix A and possibly multiple RHS's
extern "C" void  dgelss_(int* M,      int* N,     int* NRHS, double* A,     int* LDA, 
                        double* B,    int* LDB,   double* S, double* RCOND, int* RANK, 
                        double* WORK, int* LWORK, int* INFO );

extern "C" void  sgelss_(int* M,      int* N,     int* NRHS, float* A,     int* LDA, 
                        float* B,    int* LDB,   float* S,   float* RCOND, int* RANK, 
                        float* WORK, int* LWORK, int* INFO );

/*
 * Compute pseudo_inverse(A)*B ==> B
 *
 * Parameters:
 * Ax      -  Matrix to invert             (column major)
 * Bx      -  RHS (possibly multiple)      (column major)
 * Sx      -  Vector of singular values         
 * x       -  Workspace
 *
 * Arows  -  rows(A)
 * Acols  -  cols(A)
 * Bcols  -  cols(B)
 * xdim   -  size of x in double words
 *
 * Returns:
 *   pinv(A)*B ==> B
 *
 * Notes:
 *    Not fully implemented, 
 *    - No error checking on inputs (presumably LAPACK does that)
 *
 */

void svd_solve(double * Ax, int Arows, int Acols, double * Bx, int Bcols, double * Sx, double * x, int xdim)
{
    //set up unused parameters
    double RCOND = -1.0;         // Uses machine epsilon instead of the condition 
                                 // number when calculating singular value drop-tol
    int RANK;
    int INFO;
    
    dgelss_(&(Arows), &(Acols),  &(Bcols),   Ax,    &(Arows),  
               Bx,    &(Acols),     Sx,    &(RCOND), &(RANK), 
               x,    &(xdim),   &(INFO) );

    if(INFO != 0)
    {   std::cerr << "svd_solve failed with dgelss giving flag: " << INFO << '\n'; }
}
void svd_solve(float * Ax, int Arows, int Acols, float * Bx, int Bcols, float * Sx, float * x, int xdim)
{
    //set up unused parameters
    float RCOND = -1.0;         // Uses machine epsilon instead of the condition 
                                // number when calculating singular value drop-tol
    int RANK;
    int INFO;
    
    sgelss_(&(Arows), &(Acols),  &(Bcols),   Ax,    &(Arows),  
               Bx,    &(Acols),     Sx,    &(RCOND), &(RANK), 
                x,    &(xdim),   &(INFO) );

    if(INFO != 0)
    {   std::cout << "svd_solve failed with sgelss giving flag: " << INFO << '\n'; }

}

#endif
