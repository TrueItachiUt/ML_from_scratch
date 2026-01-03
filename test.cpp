#include <iostream>
#include <vector>
#include <format>
using namespace std;
int main()
{
    vector<int> a(2); a.push_back(1); a.push_back(-2);
    cout  <<  format("a is {}", a) << endl;
    return 0;
}