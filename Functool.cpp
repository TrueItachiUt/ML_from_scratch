#include <iostream>
#include <memory>
#include <utility>
#include <functional>
#include <cmath>
#include <string>
#include <map>
using namespace std;

const double eps = 1e-18;

class Variable
{
public:
    double val;
    Variable() {}
    void give(double n) { val = n; }
};

typedef function<double(Variable, double)> Simple_pow_func;

// Fix: pass by const reference or value to avoid modification; quefficient cannot be assigned to function
// Instead, capture it in lambda
Simple_pow_func wrap(double quefficient, Simple_pow_func f)
{
    return [quefficient, f](Variable x, double pow) { return quefficient * f(x, pow); };
}

map<string, Simple_pow_func> ElementaryFunctions =
{
    {"power", [](Variable x, double n_pow) { return pow(x.val, n_pow); }},
    {"sin", [](Variable x, double n_pow) { return sin(x.val); }},
    {"cos", [](Variable x, double n_pow) { return cos(x.val); }},
    {"tan", [](Variable x, double n_pow) { return tan(x.val); }},
    {"cot", [](Variable x, double n_pow) { return 1 / (eps + tan(x.val)); }},
    {"log", [](Variable x, double n_pow) { return log(x.val); }},
    {"log_deriv", [](Variable x, double n_pow) { return 1 / (x.val + eps); }},
    {"log10", [](Variable x, double n_pow) { return log10(x.val); }},
    {"log10_deriv", [](Variable x, double n_pow) { return 1 / ((x.val + eps) * log(10)); }},
    {"lin_activ", [](Variable x, double n_pow) { return x.val; }},
    {"lin_activ_der", [](Variable x, double n_pow) { return 1.0; }},
    {"exp", [](Variable x, double n_pow) { return exp(x.val); }},
    {"exp_deriv", [](Variable x, double n_pow) { return exp(x.val); }},
    {"sigmoid", [](Variable x, double n_pow) { return 1 / (1 + exp(-x.val)); }},
    {"sigmoid_deriv", [](Variable x, double n_pow) {
        double s = 1.0 / (1.0 + exp(-x.val));
        return s * (1.0 - s);
    }},
    {"relu", [](Variable x, double n_pow) { return max(0.0, x.val); }},
    {"relu_deriv", [](Variable x, double n_pow) { return (x.val > 0 ? 1.0 : 0.0); }},
    {"tanh", [](Variable x, double n_pow) { return tanh(x.val); }},
    {"tanh_deriv", [](Variable x, double n_pow) { return 1 - pow(tanh(x.val), 2); }}
};

class ElementaryFunction
{
    function<double(Variable)> f;
    function<double(Variable)> derivative_f;
    double pow;
public:
    double quefficient;

    ElementaryFunction() {
        f = ElementaryFunctions["lin_activ"];
        derivative_f = ElementaryFunctions["lin_activ_der"];
        pow = 1.0;
    }

    ElementaryFunction(string fname, double power = 1)
    {
        pow = 1;
        bool b = 0;
        for (const auto& kv : ElementaryFunctions) {
            if (kv.first == fname) {
                b = 1;
                break;
            }
        }
        if (b == 0) throw invalid_argument("Unknown elementary function");
        if (fname.find("der") != string::npos && fname != "log_deriv") {
            throw invalid_argument("Elementary function can not be derivative");
        }

        if (fname == "power") {
            f = bind(ElementaryFunctions["power"], placeholders::_1, power);
            Simple_pow_func temp_f = bind(ElementaryFunctions["power"], placeholders::_1, power - 1);
            derivative_f = bind(wrap(power, temp_f), placeholders::_1, 1.0);
            pow = power;
        }
        else if (fname == "sin") {
            f = bind(ElementaryFunctions["sin"], placeholders::_1, 1.0);
            derivative_f = bind(ElementaryFunctions["cos"], placeholders::_1, 1.0);
        }
        else if (fname == "cos") {
            f = bind(ElementaryFunctions["cos"], placeholders::_1, 1.0);
            derivative_f = bind(wrap(-1, ElementaryFunctions["sin"]), placeholders::_1, 1.0);
        }
        else if (fname == "tan") {
            f = bind(ElementaryFunctions["tan"], placeholders::_1, 1.0);
            derivative_f = [](Variable x) { return pow(cos(x.val), -2); };
        }
        else if (fname == "cot") {
            f = bind(ElementaryFunctions["cot"], placeholders::_1, 1.0);
            derivative_f = bind(wrap(-1, [](Variable x, double) { return pow(sin(x.val), -2); }), placeholders::_1, 1.0);
        }
        else if (fname == "lin_activ") {
            f = bind(ElementaryFunctions["lin_activ"], placeholders::_1, 1.0);
            derivative_f = bind(ElementaryFunctions["lin_activ_der"], placeholders::_1, 1.0);
        }
        else {
            f = bind(ElementaryFunctions[fname], placeholders::_1, 1.0);
            derivative_f = bind(ElementaryFunctions[fname + "_deriv"], placeholders::_1, 1.0);
        }
    }
};

int main()
{
    cout << "Hello world" << endl;
    return 0;
}