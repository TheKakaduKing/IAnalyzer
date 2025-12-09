#pragma once
#include "imgui/imgui_internal.h"
#include <concepts>
#include <string>
#include <vector>
#include <deque>
#include <regex>
#include <unordered_map>

namespace iamath{
  class calcInSingle;
  class calcInSeq;
}


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


class iamath::calcInSingle{

  public:
    template <supported_input T>
      calcInSingle(T& input)
      {
        resultSingle_ = calculate(input);
      }

  protected:
    calcInSingle(){}

  public:
    operator double() &&{
      return resultSingle_;
    }

    //Delete copy constructor/assignment as there is no usecase for this class
    calcInSingle(calcInSingle&) = delete;
    calcInSingle operator=(calcInSingle&) = delete;

    //Delete move constructor/assignment as there is no usecase for this class
    calcInSingle(calcInSingle&&) = delete;
    calcInSingle operator=(calcInSingle&&) = delete;

  protected:
    template<supported_input T>
    inline double calculate(const T& input) const;
    std::wstring preprocess(const std::wstring& inputWstring) const;
    std::vector<std::wstring> tokenize(const std::wstring& inputWstring) const;   
    std::deque<std::wstring> convertRPN(const std::vector<std::wstring>& inputTokens) const;
    double evalRPN(std::deque<std::wstring>& queue) const;
    double factorial(double value) const;

    private:
    template<supported_input T>
      inline std::vector<double> calculate(const T& input) const;

  protected:
    wchar_t var1_{L'n'};
    wchar_t prefixNeg_{L'#'};
    double resultSingle_{};
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


class iamath::calcInSeq : public iamath::calcInSingle{
  public:

    template <supported_input T>
      calcInSeq(T& input, int start, int end, int inc):
        iamath::calcInSingle{}
      {
        resultSeq_ = calculate(input, start, end, inc);
      }

    //Delete copy constructor/assignment as there is no usecase for this class
    calcInSeq(calcInSeq&) = delete;
    calcInSeq operator=(calcInSeq&) = delete;

    //Delete move constructor/assignment as there is no usecase for this class
    calcInSeq(calcInSeq&&) = delete;
    calcInSeq operator=(calcInSeq&&) = delete;

  public:
    operator std::vector<double>() &&{
      return resultSeq_;
    }

  protected:
    wchar_t var1_{L'n'};
    std::vector<double> resultSeq_{};
    std::vector<double> evalRPN(std::deque<std::wstring> queue, int start, int end, float inc) const;

  private:
    template<supported_input T>
    inline std::vector<double> calculate(const T& input,  int start, int end, int inc) const;

  protected:

};




template<supported_input T>
inline double iamath::calcInSingle::calculate(const T& input) const{

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
inline std::vector<double> iamath::calcInSeq::calculate(const T& input, int start, int end, int inc) const{

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
