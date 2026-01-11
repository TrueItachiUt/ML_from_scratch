#include <iostream>
#include <boost/numeric/mtl/matrix/dense2D.hpp>
#include <boost/numeric/mtl/matrices.hpp>
#include <boost/numeric/mtl/operation/operators.hpp>
#include <boost/numeric/mtl/vectors.hpp>
#include <boost/numeric/mtl/vector/dense_vector.hpp>

mtl::dense_vector<double> f()
{
    mtl::dense_vector<double> x(3);
    x[0] = 1.0; x[1] = 1.0; x[2] = 1.0;
    mtl::dense_vector<double> z(x+3);
    return  mtl::dense_vector<double>(x+z);
}
int main() {
    using namespace mtl;

    // Create a 3x3 dense matrix of doubles
    dense2D<double> A(3, 3);

    // Fill the matrix
    A = 1.0, 2.0, 3.0,
        4.0, 5.0, 6.0,
        7.0, 8.0, 9.0;

    // Create a vector
    dense_vector<double> x(3);
    x[0] = 1.0; x[1] = 1.0; x[2] = 1.0;
    dense_vector<double> z(x+3);
    dense_vector<double> w=f();
    std::cout << w << std::endl;

    // Matrix-vector multiplication
    dense_vector<double> y(A * x);
    // Output results
    /*
    std::cout << "A = \n" << A << "\n";
    std::cout << "x = " << x << "\n";
    std::cout << "y = A * x = " << y << "\n";
    std::cout << A.num_rows() << '\t' << A.num_cols() << std::endl;
    for (int i=0; i<A.num_rows(); i++)
    {
        for (int j=0; j<A.num_cols(); j++)
        {
            A[i][j]++; std::cout << A[i][j] << ' ';
        }
        std::cout << std::endl;
    }*/
    dense2D<double> B(A.num_rows(), A.num_cols());
    B = A*2.5;
    dense2D<double> C(A+B);
    return 0;
}