#pragma once
#include <string>
#include <vector>
#include <unordered_map>


struct stOperator{
  std::wstring op;
  int priority;
};
class stringHandler{
  public:
    stringHandler(std::wstring input);
    stringHandler(std::wstring input, wchar_t delimBegin, wchar_t delimEnd);
    stringHandler();

  private:
    std::wstring input_;
    std::vector<std::wstring> tokens_;
    const std::unordered_map<wchar_t, int>  operatorMap_{
      {L'+', 1},
      {L'-', 1},
      {L'*', 2},
      {L'/', 2},
      {L'&', 3}, // sqareroot
      {L'^', 3}, // exponent
      {L'~', 3}, // sin
      {L'@', 3}, // cos
      {L'§', 3}, // tan
      {L'!', 4}, // factorial
      {L'(', 5},
      {L')', 5}};

    const std::unordered_map<wchar_t, int>  operatorMapNoP_{
      {L'+', 1},
      {L'-', 1},
      {L'*', 2},
      {L'/', 2},
      {L'&', 3}, // sqareroot
      {L'^', 3}, // exponent
      {L'~', 3}, // sin
      {L'@', 3}, // cos
      {L'§', 3}, // tan
      {L'!', 4}}; // factorial

    const std::unordered_map<wchar_t, int>  operatorMapUnaryR_{
      // right associated operators
      {L'&', 3}, // sqareroot
      {L'~', 3}, // sin
      {L'@', 3}, // cos
      {L'§', 3}}; // tan
                  //
    const std::unordered_map<wchar_t, int>  operatorMapUnaryL_{
      // left associated operators
      {L'!', 4}}; // factorial

    const std::unordered_map<wchar_t, int>  operatorMapBinary_{
      // binary operators
      {L'+', 1},
      {L'-', 1},
      {L'*', 2},
      {L'/', 2},
      {L'^', 3}}; // exponent

    wchar_t delimBegin_{L'<'};
    wchar_t delimEnd_{L'>'};
    wchar_t prefixNeg_{L'#'};


  public:
    std::wstring preprocess(const std::wstring in);
    void tokenize();   
    std::wstring convertRPN();
    void evalRPN(const std::wstring rpn);
    std::wstring updateString(std::wstring& in, std::wstring::iterator it_start, std::wstring num1, std::wstring num2, wchar_t op);
    std::wstring updateString(const std::wstring& in, std::wstring::iterator it_start, std::wstring num1, wchar_t op);

};
