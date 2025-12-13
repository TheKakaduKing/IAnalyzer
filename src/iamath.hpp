#pragma once
#include <concepts>
#include <string>
#include <vector>
#include <deque>
#include <regex>
#include <unordered_map>
#include <iostream>

namespace iamath{
  class baseCalc;
  class calcInSingle;
  class calcInSeq;
}


template <typename T>
concept supported_input = std::same_as<T, std::string> || std::same_as<T, std::wstring>;

struct prepRule{
  std::wregex reg;
  std::wstring rep{L""};
  bool inEx{false};
};

struct stOpRpn{
  wchar_t op;
  int prio;
};

struct stRightAssocFunc{
  wchar_t func;
  int parentCount;  //parenth. counter for nested functions and parenth.
};


//*************************************************************************************************
//                      Base calculation class
//*************************************************************************************************
//  --derives into subclasses, base class only
//

class iamath::baseCalc{

  protected:
    baseCalc(){}

    //Delete copy constructor/assignment as there is no usecase for this class
    baseCalc(baseCalc&) = delete;
    baseCalc operator=(baseCalc&) = delete;

    //Delete move constructor/assignment as there is no usecase for this class
    baseCalc(baseCalc&&) = delete;
    baseCalc operator=(baseCalc&&) = delete;

  protected:
    std::wstring preprocess(const std::wstring& inputWstring) const;
    std::vector<std::wstring> tokenize(const std::wstring& inputWstring) const;   
    std::deque<std::wstring> convertRPN(const std::vector<std::wstring>& inputTokens) const;
    double factorial(int value) const;

  protected:

    static inline const std::unordered_map<wchar_t, int>  operatorMap_{
      {L'+', 1}, {L'-', 1}, {L'*', 2}, {L'/', 2},
      {L'&', 3}, // sqareroot
      {L'^', 3}, // exponent
      {L'~', 3}, // sin
      {L'@', 3}, // cos
      {L'§', 3}, // tan
      {L'!', 4}, // factorial
      {L'$', 4}, // ln
      {L'€', 4}, // log10
      {L'N', 4}, // negate (unary minus)
      {L'(', 5}, {L')', 5}};

    static inline const std::unordered_map<wchar_t, int>  operatorMapFunc_{
      // right associated operators
      {L'&', 3}, // sqareroot
      {L'~', 3}, // sin
      {L'@', 3}, // cos
      {L'§', 3}, // tan
      {L'$', 4}, // ln
      {L'€', 4}}; // log10
 
    static inline const std::unordered_map<wchar_t, int>  operatorMapBinary_{
      // binary functions
      {L'+', 1}, {L'-', 1}, {L'*', 2}, {L'/', 2}, {L'^', 3}};
};


//*************************************************************************************************
//                      Single calculation class
//*************************************************************************************************
//  --Calculates a single string or wstring input
//

class iamath::calcInSingle : public iamath::baseCalc{

  public:
    template <supported_input T>
      calcInSingle(T& input)
      {
        resultSingle_ = calculate(input);
      }

    //Delete copy constructor/assignment as there is no usecase for this class
    calcInSingle(calcInSingle&) = delete;
    calcInSingle operator=(calcInSingle&) = delete;

    //Delete move constructor/assignment as there is no usecase for this class
    calcInSingle(calcInSingle&&) = delete;
    calcInSingle operator=(calcInSingle&&) = delete;

  public:
    operator double() &&{
      return resultSingle_;
    }

  private:
    template<supported_input T>
    inline double calculate(const T& input) const;
    double evalRPN(std::deque<std::wstring>& queue) const;

  private:
    double resultSingle_{};
};


//*************************************************************************************************
//                      Sequence calculation class
//*************************************************************************************************
//  --Calculates a sequence of values, based on given input string or wstring, start, end, and increment

class iamath::calcInSeq : public iamath::baseCalc{

  public:
    template <supported_input T>
      calcInSeq(T& input, int start, int end, int inc):
        iamath::baseCalc{}
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

  private:
    template<supported_input T>
    inline std::vector<double> calculate(const T& input,  int start, int end, int inc) const;
    std::vector<double> evalRPN(std::deque<std::wstring> queue, int start, int end, float inc) const;

  private:
    std::vector<double> resultSeq_{};

};



//*************************************************************************************************
//                      Single calculation class member declaration
//*************************************************************************************************

// ***Member function***
// --Call the member functions to calculate a single result and return it
//
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

  std::wstring prepString = baseCalc::preprocess(tempStr);
  if (prepString == L"") {
    return 0;
  }
  std::vector<std::wstring> tokens = baseCalc::tokenize(prepString);
  std::deque<std::wstring> rpn = baseCalc::convertRPN(tokens);
  double result = calcInSingle::evalRPN(rpn);

  return result;
}



//*************************************************************************************************
//                      Sequence calculation class member declaration
//*************************************************************************************************

// ***Member function***
// --Call the member functions to calculate results and return result vector 
//
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

  std::wstring prepString = baseCalc::preprocess(tempStr);
  if (prepString == L"") {
    return std::vector<double>{};
  }
  std::vector<std::wstring> tokens = baseCalc::tokenize(prepString);
  std::deque<std::wstring> rpn = baseCalc::convertRPN(tokens);
  std::vector<double> result = calcInSeq::evalRPN(rpn, start, end, inc);

  return result;
}
