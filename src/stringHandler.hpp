#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <regex>


struct stOpRpn{
  wchar_t op;
  int prio;
};

struct stEval{
  double val;
  wchar_t op;
};

struct stRightAssocFunc{
  wchar_t func;
  bool negateFunc;
  int parentCount;  //parenth. counter for nested functions and parenth.
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
      {L'N', 4}, // factorial
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

    const std::unordered_map<wchar_t, int>  operatorMapUnary_{
      {L'&', 3}, // sqareroot
      {L'~', 3}, // sin
      {L'@', 3}, // cos
      {L'§', 3}, // tan
      {L'!', 4}};// factorial

    const std::unordered_map<wchar_t, int>  operatorMapFunc_{
      // right associated operators
      {L'&', 3}, // sqareroot
      {L'~', 3}, // sin
      {L'@', 3}, // cos
      {L'§', 3}}; // tan
                  //

    const std::unordered_map<wchar_t, int>  operatorMapBinary_{
      // binary operators
      {L'+', 1},
      {L'-', 1},
      {L'*', 2},
      {L'/', 2},
      {L'^', 3}}; // exponent

    wchar_t prefixNeg_{L'#'};
    wchar_t var1_{L'n'};
    static constexpr std::regex_constants::syntax_option_type rx_ = std::regex_constants::extended;


  public:
    std::wstring preprocess(const std::wstring in);
    void tokenize();   
    std::deque<std::wstring> convertRPN();
    int evalRPN(std::deque<std::wstring> queue);
    double factorial(double value);

};
