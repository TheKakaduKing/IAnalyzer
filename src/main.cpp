#include "hdr/render.hpp"
#include "hdr/stringHandler.hpp"
#include <iostream>

// Check if this expression is correct RPN: [-20.0[[n[!*[-9.0[[1[+*[n[*[-1[&/[5[[2[!^*[2[!*[1.4[[-1[*[-58[~*[1[@*-[-3[{+

// Main code
int main(){
  renderGLFWWindow(); 
  // stringHandler str{"1+2*5*(s(5^25))"};
  stringHandler stf{};
  stringHandler str{stf.preprocess("-20.0n!(-9.0++1)n/s-(5)^+2!*+2!---1.4*-sin-58*cos1+tan-3")};
  // stringHandler str{"5*4/2"};
  str.tokenize();
    std::cout << str.convertRPN() << std::endl;
  std::cout << str.preprocess("-20.0n!(-9.0++1)n/s-(5)^+2!*+2!---1.4*-sin-58*cos1+tan-3") << std::endl;

}



