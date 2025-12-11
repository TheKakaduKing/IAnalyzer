#include "render.hpp"
#include "iamath.hpp"
#include <iostream>
#include <string>
#include <vector>


//plan: 
//create a window manager class, which creates an instance of itself once its called, and uses it everytime its called -> always only one instance per app lifetime
//create a static vector containig unique pointers to window objects
//create member functions to create or delete windows ny accessing and modifying abouve container
//create a window class, which holds ciritcal window information
//create derived classes, like panel left, sequence window etc.. 
//these derived classes are all accessible over the unique pointer container, and so in a loop access the show window functions of the devied classes,
//and check for show/dont show etc.
//IMPORTANT window base class needs a virtual destructor, otherwise only the window class destructor is called, but not the derived class destructor
// Main code
int main(){
  renderGLFWWindow(); 
  // std::string a = "-(2.0E1+3)1.1n(-9.0+2)/sqrt(5^2)*2!+-1.4*-sin(-1.0e-1)*cos(1)+tan(-3)";
  std::string a = "5+ln(25)";
  std::wstring b = L"4n+2";
  const char* s = a.c_str();


  std::wcout << s << std::endl;
  // double c = iamath::calcInSingle{a};
  // std::wcout << iamath::calcInSingle(a) << std::endl;
  // std::vector<double> d = iamath::calcInSeq{b, 0,100,1};
}



