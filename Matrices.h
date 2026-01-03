#include <iostream>
#include <vector>
#include <format>
using namespace std;
class Matrix
{
    vector<int> dimens();
    vector<double> flattened_matrix();
    Matrix() {dimens.add(0); }
    Matrix(vector<int> d, vector<double> values)
    {
        int n_elems = 1;
        for (const int& t:d) {n_elems*=t;}
        if (n_elems!=values.size())
        {
            throw invalid_argument(format("Dimensionalities do not match = there are {} values in array
                 and length of flattened matrix of defined dimensionality is {}", values.size(), n_elems));
        }
        dimens = move(d);
        flattened_matrix = move(values);
    }
    double operator() (vector<int> indices)
    {
        if (indices.size()!=dimens.size())
        {
            throw invalid_argument("Dimensionalities do not match");
        }
        for (int i=0; i<indices.size(); i++)
        {
            if (indices[i]>=dimens[i]) {
                throw out_of_range(format("trying to access element {} on 
                {}-th dimension, while there are only {}", indices[i], i+1, dimens[i]));}
        }
        int ind;
        switch indices.size()
        {
            case 1: ind=indices[0]; break;
            case 2: ind=(indices[0])*dimens[1]+indices[1]; break;
            case 3: ind = indices[0] * (dimens[1] * dimens[2]) + indices[1] * dimens[2] + indices[2];; break;
            //Vector of t n*m matrices, there are n*m*indices[0]-1 elems before and everything else is just like in case 2
            default: cout << "Unable to determine elem" << endl; ind = 0; break;
        }
        return flattened_matrix[ind];
    }
}