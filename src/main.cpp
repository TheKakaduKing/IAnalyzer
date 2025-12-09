#include "render.hpp"
#include "iamath.hpp"
#include <iostream>
#include <string>
#include <vector>


// Main code
int main(){
  renderGLFWWindow(); 
  // std::string a = "-(2.0E1+3)1.1n(-9.0+2)/sqrt(5^2)*2!+-1.4*-sin(-1.0e-1)*cos(1)+tan(-3)";
  std::string a = "5+ln(25)";
  std::wstring b = L"4n+2";


  // double c = iamath::calcInSingle{a};
  std::wcout << iamath::calcInSingle(a) << std::endl;
  // std::vector<double> d = iamath::calcInSeq{b, 0,100,1};
}



