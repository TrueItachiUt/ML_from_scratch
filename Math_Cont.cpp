#include <memory>
#include <unordered_map>
#include <utility>
#include <type_traits>
#include <functional>
#include <variant>
#include <cmath>
#include <stdexcept>
#include "Matrices_req.h"

using namespace std;

// Template aliases — no hardcoded double
template<typename T>
using Tensor = mtl::dense2D<T>;

template<typename T>
using Vector = mtl::dense_vector<T>;

class Variable
{
public:
    unordered_map<shared_ptr<Variable>, double> derivatives;
    double value;
    Variable() : value(0.0) {}
    void give(double v) { value = v; }
    Variable(double v) { this->give(v); }
    Variable operator+(const Variable& other) const {
            Variable res(this->value + other.value);
            
            // Case 1: Variables in 'this' (left operand)
            for (auto [Var, da] : derivatives) {
                double db = other.derivatives.contains(Var) ? other.derivatives.at(Var) : 0.0;
                res[Var] = da + db;
            }
            
            // Case 2: Variables only in 'other' (not in 'this')
            for (auto [Var, db] : other.derivatives) {
                if (!derivatives.contains(Var)) {
                    res[Var] = db;  // da/dx = 0
                }
            }
            return res;
        }

        // Subtraction: d(a-b)/dx = da/dx - db/dx
    Variable operator-(const Variable& other) const {
            Variable res(this->value - other.value);
            
            // Case 1: Variables in 'this'
            for (auto [Var, da] : derivatives) {
                double db = other.derivatives.contains(Var) ? other.derivatives.at(Var) : 0.0;
                res[Var] = da - db;
            }
            
            // Case 2: Variables only in 'other'
            for (auto [Var, db] : other.derivatives) {
                if (!derivatives.contains(Var)) {
                    res[Var] = -db;  // da/dx = 0
                }
            }
            return res;
        }

        // Multiplication: d(a*b)/dx = b*da/dx + a*db/dx
    Variable operator*(const Variable& other) const {
            double a = this->value;
            double b = other.value;
            Variable res(a * b);
            
            // Case 1: Variables in 'this'
            for (auto [Var, da] : derivatives) {
                double db = other.derivatives.contains(Var) ? other.derivatives.at(Var) : 0.0;
                res[Var] = b * da + a * db;
            }
            
            // Case 2: Variables only in 'other'
            for (auto [Var, db] : other.derivatives) {
                if (!derivatives.contains(Var)) {
                    res[Var] = a * db;  // da/dx = 0
                }
            }
            return res;
        }

        // Division: d(a/b)/dx = (b*da/dx - a*db/dx)/b²
    Variable operator/(const Variable& other) const {
            double a = this->value;
            double b = other.value+1e-9;
            double b_sq = b * b;
            Variable res(a / b);
            
            // Case 1: Variables in 'this'
            for (auto [Var, da] : derivatives) {
                double db = other.derivatives.contains(Var) ? other.derivatives.at(Var) : 0.0;
                res[Var] = (b * da - a * db) / b_sq;
            }
            
            // Case 2: Variables only in 'other'
            for (auto [Var, db] : other.derivatives) {
                if (!derivatives.contains(Var)) {
                    res[Var] = (-a * db) / b_sq;  // da/dx = 0
                }
            }
            return res;
        }
    Variable ApplyUnary(function<double(double)> f, function<double(double)>deriv)
    {
        Variable res(f(value));
        for (auto [Var, d]:derivatives)
        {
            res[Var]=d*deriv(this->value);
            //Chain rule: d(f(g(x))) = d_(f(g(x)))/d_(g(x))*d_(g(x))/d_x
        }
        return res;
    }
    
};


Variable sin(Variable v) {return v.ApplyUnary([](double v){return sin(v);}, [](double v){return cos(v);});}
Variable cos(Variable v) {return v.ApplyUnary([](double v){return cos(v);}, [](double v){return sin(v)*-1;});}
// Hyperbolic tangent: tanh(x)
Variable tanh(Variable v) {
    return v.ApplyUnary(
        [](double x) { return std::tanh(x); },
        [](double x) { return 1.0 - std::tanh(x)*std::tanh(x); } // sech²(x) = 1 - tanh²(x)
    );
}

// Rectified Linear Unit: ReLU(x) = max(0, x)
Variable relu(Variable v) {
    return v.ApplyUnary(
        [](double x) { return std::max(0.0, x); },
        [](double x) { return (x > 0) ? 1.0 : 0.0; } // Subgradient at 0: use 0 (common convention)
    );
}

// Sigmoid: σ(x) = 1/(1+e⁻ˣ)
Variable sigmoid(Variable v) {
    return v.ApplyUnary(
        [](double x) { 
            return 1.0 / (1.0 + exp(-x)); 
        },
        [](double x) {
            double s = 1.0 / (1.0 + exp(-x)); // Compute once for efficiency
            return s * (1.0 - s); // σ'(x) = σ(x)(1-σ(x))
        }
    );
}

// Exponential: exp(x)
Variable exp(Variable v) {
    return v.ApplyUnary(
        [](double x) { return exp(x); },
        [](double x) { return exp(x); } // d(exp)/dx = exp(x)
    );
}

// Natural logarithm: ln(x) [for x>0]
Variable log(Variable v) {
    return v.ApplyUnary(
        [](double x) { 
            if (x <= 0) throw domain_error("log(x) undefined for x≤0");
            return log(x); 
        },
        [](double x) { 
            if (x <= 0) throw domain_error("log derivative undefined for x≤0");
            return 1.0 / x; 
        }
    );
}









enum class Cont { Scalar, Vector, Matrix };

template<typename Var, typename = enable_if_t<is_same_v<Var, double> || is_same_v<Var, Variable>>>
class NumericCont
{
public:
    variant<Var, Vector<Var>, Tensor<Var>> Val;
    pair<int, int> shape;
    bool trans = false;

    NumericCont(Var d)
        : shape{1, 1}, Val(d) {}

    NumericCont(const Vector<Var>& a, bool t = false)
        : shape{a.length(), 1}, trans(t), Val(a) {}

    NumericCont(const Tensor<Var>& A, bool t = false)
        : shape{A.Num_Rows(), A.Num_Cols()}, trans(t), Val(A) {}

    NumericCont(int d1 = 1, int d2 = 1, bool t = false)
    {
        if (t) swap(d1, d2);
        if (d2 == 1) {
            if (d1 == 1) {
                shape = {1, 1};
                Val = Var{};
                return;
            }
            shape = {d1, 1};
            Val = Vector<Var>(d1);
            return;
        }
        shape = {d1, d2};
        Val = Tensor<Var>(d1, d2);
    }

    pair<int, int> GetShape() const
    {
        int d1 = shape.first, d2 = shape.second;
        if (trans) swap(d1, d2);
        return {d1, d2};
    }

    Cont GetCont() const
    {
        auto [r, c] = GetShape();
        if (r == 1 && c == 1) return Cont::Scalar;
        if (r == 1 || c == 1) return Cont::Vector;
        return Cont::Matrix;
    }

    NumericCont operator+(const NumericCont<Var>& other) const
    {
        auto s_shape = GetShape();
        auto o_shape = other.GetShape();

        if (GetCont() != Cont::Scalar && other.GetCont() != Cont::Scalar && s_shape != o_shape)
            throw invalid_argument("Dimensionalities do not match!");

        auto result_val = visit([](const auto& a, const auto& b) -> variant<Var, Vector<Var>, Tensor<Var>> {
            return a + b;
        }, Val, other.Val);

        pair<int, int> res_shape;
        if (GetCont() == Cont::Scalar) {
            res_shape = o_shape;
        } else if (other.GetCont() == Cont::Scalar) {
            res_shape = s_shape;
        } else {
            res_shape = s_shape;
        }

        NumericCont result;
        result.Val = result_val;   // no std::move
        result.shape = res_shape;
        result.trans = false;
        return result;
    }

    NumericCont operator*(const NumericCont<Var>& other) const
    {
        if (GetCont() == Cont::Matrix && other.GetCont() == Cont::Matrix) {
            auto [r1, c1] = GetShape();
            auto [r2, c2] = other.GetShape();
            if (c1 != r2)
                throw invalid_argument("Dimensionalities for matmul do not match!");
        }

        auto result_val = std::visit([](const auto& a, const auto& b) -> variant<Var, Vector<Var>, Tensor<Var>> {
            return a * b;
        }, Val, other.Val);

        pair<int, int> res_shape;
        if (GetCont() == Cont::Scalar) {
            res_shape = other.GetShape();
        } else if (other.GetCont() == Cont::Scalar) {
            res_shape = GetShape();
        } else if (GetCont() == Cont::Matrix && other.GetCont() == Cont::Matrix) {
            auto [r1, c1] = GetShape();
            res_shape = {r1, other.GetShape().second};
        } else {
            // Elementwise multiplication or vector dot (assumes MTL handles it)
            res_shape = GetShape();
        }

        NumericCont result;
        result.Val = result_val;   // no std::move
        result.shape = res_shape;
        result.trans = false;
        return result;
    }

    /*Need to consider trans, slicing*/
};

int main()
{
    return 0;
}