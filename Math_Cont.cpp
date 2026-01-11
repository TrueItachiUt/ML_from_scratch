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
    Variable() {}
    void give(double v) {value=v;}
    Variable(double v) {this->give(v);}


}

enum class Cont {Scalar, Vector, Matrix};

template<typename Var, typename = enable_if_t<is_same_v<Var, double>||is_same_v<Var, Variable>>> //Only numeric or Variable Container
class NumericCont
{
    public:
    variant<Var, Vector<Var>, Tensor<Var>> Val;
    pair<int, int> shape; //Only two-dim Tensors are supported
    bool trans;
    NumericCont(Var d)
    {
        shape = {1,1}; Val = d; trans=false;
    }



    NumericCont(const Vector<Var>& a, bool t=false)
    {
        shape = {a.length(), 1}; trans=t; Val=a;
    }




    NumericCont(const Tensor<Var>& A, bool t=false)
    {
        shape = {A.Num_Rows(), A.Num_Cols()}; trans = t; Val = A;
    }



    NumericCont(int d1=1, int d2=1, bool t=false)
    {
        if (t) swap(d1, d2);
        if (d2==1)
        {
            if (d1==1) {shape = {1,1}; Val = Variable(); return;}
            Val = Vector<Variable>(d1);
            return;
        }
        Val = Tensor<Variable>(d1, d2);
    }


    pair<int, int> GetShape()
    {
        int d1 = shape.first; int d2 = shape.second;
        if (trans) swap(d1, d2);
        return {d1, d2};
    }

    Cont GetCont()
    {
        if (shape.first==1 && shape.second==1) return Cont::Scalar;
        if (shape.first==1 ^ shape.second==1) return Cont::Vector;
        if (!(shape.first==1 || shape.second==1)) return Cont::Matrix;
    }

    NumericCont Operator+(const NumericCont<Var>& other)
    {
        if (this->GetCont==other.GetCont){
            switch this->GetCont
            {
                case Cont::Scalar: return this->value+other.value; break;
                case Cont::Vector: return Vector<Var>(this.value+other.value); break;
                case Cont::Tensor: return Tensor<Var>(this.value+other.value); break;
            }
        };
        if (this->GetCont==Cont::Scalar)
        {
            if (other.GetCont==Vector) return Vector<Var>()
        }

    }

}