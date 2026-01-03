#include <iostream>
#include <boost/numeric/mtl/matrix/dense2D.hpp>
#include <boost/numeric/mtl/operation/operators.hpp>
#include <boost/numeric/mtl/vector/dense_vector.hpp>

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

    // Matrix-vector multiplication
    dense_vector<double> y(A * x);

    // Output results
    std::cout << "A = \n" << A << "\n";
    std::cout << "x = " << x << "\n";
    std::cout << "y = A * x = " << y << "\n";

    return 0;
}