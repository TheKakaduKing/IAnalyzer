#pragma once
#include <string>
#include <vector>
#include <unordered_map>


struct stOperator{
  std::string op;
  int priority;
};
class stringHandler{
  public:
    stringHandler(std::string input);
    stringHandler();

  private:
    std::string input_;
    std::vector<std::string> tokens_;
    const std::unordered_map<std::string, int> operatorMap_{
      {"+", 1},
      {"-", 1},
      {"*", 2},
      {"/", 2},
      {"&", 3}, // sqareroot
      {"^", 3}, // exponent
      {"~", 3}, // sin
      {"@", 3}, // cos
      {"{", 3}, // tan
      {"!", 4}, // factorial
      {"(", 5},
      {")", 5}};


  public:
    std::string preprocess(std::string in);
    void tokenize();   
    std::string convertRPN();

};
