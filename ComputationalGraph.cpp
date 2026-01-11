#include <memory>
#include <utility>
#include <functional>
#include <Math_Cont.h>
using namespace std;
using Tensor = mtl::dense2D;
using Vector = mtl::dense_vector;


class VarNode
{
    unique_ptr<NumericCont> value;
    unique_ptr<NumericCont> grad; //Grad of loss with respect to this
    bool trainable;
}

class OpNode
{
    shared_ptr<VarNode> right, left;
    function<NumericCont(NumericCont, NumericCont)> forward;
    function<NumericCont()> der_right, der_left;
}
/*
In OpNode we need to define derivative for every input (remember that input may be unary )
*/