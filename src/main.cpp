#include "render.hpp"
#include "stringHandler.hpp"
#include <iostream>


// Main code
int main(){
  renderGLFWWindow(); 
  // stringHandler str{"1+2*5*(s(5^25))"};
  stringHandler stf{};
  stringHandler str{stf.preprocess(L"-20.0(-9.0+2)/-s(5^(5-3))*3!+-1.4*-sin(-58+2)*cos(1)+tan(-3)")};
  // stringHandler str{"5*4/2"};
  str.tokenize();
  // str.convertRPN();

  str.evalRPN(str.convertRPN());
  // str.preprocess(L"-20.0(-9.0+2)/s(5^2)*2!+n-1.4*-sin-58*cos1+tan-3");

}



