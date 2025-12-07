#pragma once
#include <concepts>
#include <string>
#include <vector>
#include <unordered_map>
#include <regex>
#include <iostream>

template <typename T>
concept supported_input = std::same_as<T, std::string> || std::same_as<T, std::wstring>;

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
    template <supported_input T>
    explicit stringHandler(T& input);
    template <supported_input T>
    explicit stringHandler(T& input, wchar_t variable, int start, int end, int inc);

    //Delete copy constructor/assignment as there is no usecase for this class
    stringHandler(stringHandler&) = delete;
    stringHandler operator=(stringHandler&) = delete;

    //Delete move constructor/assignment as there is no usecase for this class
    stringHandler(stringHandler&&) = delete;
    double operator=(stringHandler&&) = delete;


    operator double() &&{
      return resultSingle_;
    }
    operator std::vector<double>() &&{
      return resultMulti_;
    }

  private:
    template<supported_input T>
    inline double calculate(const T& input) const;
    template<supported_input T>
    inline std::vector<double> calculate(const T& input,  int start, int end, int inc) const;
    std::wstring preprocess(const std::wstring& inputWstring) const;
    std::vector<std::wstring> tokenize(const std::wstring& inputWstring) const;   
    std::deque<std::wstring> convertRPN(const std::vector<std::wstring>& inputTokens) const;
    double evalRPN(std::deque<std::wstring>& queue) const;
    std::vector<double> evalRPN(std::deque<std::wstring> queue, int start, int end, float inc) const;
    double factorial(double value) const;

  private:
    wchar_t var1_{L'n'};
    wchar_t prefixNeg_{L'#'};
    double resultSingle_{};
    std::vector<double> resultMulti_{};
    static constexpr std::regex_constants::syntax_option_type rx_ = std::regex_constants::extended; // To use regex extended syntax (Posix ERE)

    static inline const std::unordered_map<wchar_t, int>  operatorMap_{
      {L'+', 1}, {L'-', 1}, {L'*', 2}, {L'/', 2},
      {L'&', 3}, // sqareroot
      {L'^', 3}, // exponent
      {L'~', 3}, // sin
      {L'@', 3}, // cos
      {L'§', 3}, // tan
      {L'!', 4}, // factorial
      {L'$', 4}, // log
      {L'N', 4}, // factorial
      {L'(', 5}, {L')', 5}};

    static inline const std::unordered_map<wchar_t, int>  operatorMapFunc_{
      // right associated operators
      {L'&', 3}, // sqareroot
      {L'~', 3}, // sin
      {L'@', 3}, // cos
      {L'§', 3}, // tan
      {L'$', 4}}; // log
};


template <supported_input T>
stringHandler::stringHandler(T& input)
{
  resultSingle_ = calculate(input);
}

template <supported_input T>
stringHandler::stringHandler(T& input, wchar_t variable, int start, int end, int inc):
  var1_{variable}
{
  resultMulti_ = calculate(input, start, end, inc);
}

template<supported_input T>
inline double stringHandler::calculate(const T& input) const{

  std::wstring tempStr{};
  //Must be evaluated at compile time, therefore constexpr
  if constexpr(std::same_as<T, std::string>) {
    for (char c : input) {
      tempStr += wchar_t{c};
    }
  }
  else {
    tempStr = input;
  }

  std::wstring prepString = preprocess(tempStr);
  std::vector<std::wstring> tokens = tokenize(prepString);
  std::deque<std::wstring> rpn = convertRPN(tokens);
  double result = evalRPN(rpn);

  return result;
}

template<supported_input T>
inline std::vector<double> stringHandler::calculate(const T& input, int start, int end, int inc) const{

  std::wstring tempStr{};
  //Must be evaluated at compile time, therefore constexpr
  if constexpr(std::same_as<T, std::string>) {
    for (char c : input) {
      tempStr += wchar_t{c};
    }
  }
  else {
    tempStr = input;
  }

  std::wstring prepString = preprocess(tempStr);
  std::vector<std::wstring> tokens = tokenize(prepString);
  std::deque<std::wstring> rpn = convertRPN(tokens);
  std::vector<double> result = evalRPN(rpn, start, end, inc);

  return result;
}
