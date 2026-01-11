#include <memory>
#include <unordered_map>
#include <utility>
#include <type_traits>
#include <functional>
#include <variant>
#include <Matrices_req.h>

using namespace std;
using Tensor = mtl::dense2D;
using Vector = mtl::dense_vector;

class Variable
{
    unordered_map<shared_ptr<Variable>, double> derivatives;
    double value;
    Variable()
    void give();
    Variable(double v);
}

enum class Cont {Scalar, Vector, Matrix};

template<typename Var, typename = enable_if_t<is_same_v<Var, double>||is_same_v<Var, Variable>>> //Only numeric or Variable Container
class NumericCont
{
    public:
    variant<Var, Vector<Var>, Tensor<Var>> Val;
    pair<int, int> shape; //Only two-dim Tensors are supported
    bool trans;
    NumericCont(Var d);
    NumericCont(const Vector<Var>& a, bool t=false);
    NumericCont(const Tensor<Var>& A, bool t=false);
    NumericCont(int d1=1, int d2=1, bool t=false);
    pair<int, int> GetShape();
    Cont GetCont();
    NumericCont operator+(const NumericCont<Var>& other) const;
}