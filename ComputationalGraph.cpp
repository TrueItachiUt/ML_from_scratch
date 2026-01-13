#include <memory>
#include <utility>
#include <functional>
#include <Math_Cont.h>
using namespace std;
using Tensor = mtl::dense2D;
using Vector = mtl::dense_vector;

class OpNode;
class VarNode
{
    NumericCont value;
    NumericCont grad; //Grad of loss with respect to this
    weak_ptr<OpNode> parent;
    bool trainable;
    VarNode(NumericCont v, const OpNode& par, bool tr=false) {value=v; parent=par; trainable=tr;}
    void giveGrad(NumericCont v) {grad=v;}
    void applyGrad() {value=value-grad;}

}

class OpNode
{
    shared_ptr<VarNode> right, left;
    bool ac;
    function<NumericCont(NumericCont, NumericCont)> forward;
    NumericCont der_right, der_left;
    bool unary;
    OpNode(VarNode l, VarNode r) {left=l; right=r; unary=false; ac=false;}
    OpNode(VarNode s) {left=s; unary=false; ac=false;}
    void giveFunc(function<NumericCont(NumericCont, NumericCont)> f) {
        if (unary) 
            throw invalid_argument("Node is unary, you're trying to give binary func");
        forward=f; ac=true;}
    void giveFunc(function<NumericCont(NumericCont)> f){
        if (!unary) throw invalid_argument("Node is binary, you're trying to give unary func");
        forward=f; ac=true;
    }
    virtual void ObtainDer()
    {
        if (left==nullptr||!unary&&right==nullptr||!ac) throw logic_error("Node is not completely initialized!");
        NumericCont<Variable> inp_l, inp_r; 
        inp_l(left.value.Shape); inp_r(right.value.Shape);
    }
}
/*
In OpNode we need to define derivative for every input (remember that input may be unary )
*/