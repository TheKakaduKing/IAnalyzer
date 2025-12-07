#include "render.hpp"
#include "stringHandler.hpp"
#include <iostream>
#include <string>


// Main code
int main(){
  renderGLFWWindow(); 
  std::string a = "-20.0(-9.0+2)/s(5^2)*2!+-1.4*-sin(-58)*cos(1)+tan(-3)";
  std::wstring b = L"4n+2";

  // double b = stringHandler{a};
  std::wcout << stringHandler{a} << std::endl;
  std::vector<double> c = stringHandler{b, L'n', 0, 10, 1};
  std::wcout << c.size() << std::endl;
  for (double d : c) {
    std::wcout << d << std::endl;
  
  }
  // str.preprocess(L"-20.0(-9.0+2)/s(5^2)*2!+n-1.4*-sin-58*cos1+tan-3");

}



