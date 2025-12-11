#include "iamath.hpp"
#include <cmath>
#include <deque>
#include <stack>
#include <cctype>
#include <string>
#include <vector>
#include <iostream>
#include <regex>
#include <math.h>


// ***Preprocess the given string***
std::wstring iamath::baseCalc::preprocess(const std::wstring& input) const
{
  static constexpr std::regex_constants::syntax_option_type rx{std::regex_constants::extended}; // To use regex extended syntax (Posix ERE)


  //check parenthesis count
  int pCounter{0};
  for (wchar_t c : input) {
    if (c == L'(') {
      pCounter++;
    }else if (c == L')') {
      pCounter--;
    }
  }
  if (pCounter != 0) {
    return L"";
  }

  static const std::vector<prepRule> syntaxRun
  {
    //decimal error
    {std::wregex(L"[^[:digit:]]\\.|\\.[^[:digit:]]", rx)},
    //sin error
    {std::wregex(L"(sin[^(])|(cos[^(])|(tan[^(])|(log[^(])|(ln[^(])|(sqrt[^(])", rx)},
    // start of string
    {std::wregex(L"^[*/).!eE^]", rx)},
    // end of string 
    {std::wregex(L"[^[:digit:]n!)]$", rx)},
  };

  static const std::vector<prepRule> preRun
  {
    // preprocess sin to ~
    {std::wregex(L"(sin)", rx), L"~"},
    // preprocess cos to @
    {std::wregex(L"(cos)", rx), L"@"},
    // preprocess tan to § for
    {std::wregex(L"(tan)", rx), L"§"},
    // preprocess sqrt to &
    {std::wregex(L"(sqrt)", rx), L"&"},
    // preprocess ln to $
    {std::wregex(L"(ln)", rx), L"$"},
    // preprocess log to €
    {std::wregex(L"(log)", rx), L"€"},
  };

  static const std::vector<prepRule> loopRun
  {
    // insert * between digit or n or ! and opening parentheses
    {std::wregex(L"([[:digit:]n!\\)])(\\()", rx), L"$1*$2"},
    // insert * between digit or closing parentheses and n
    {std::wregex(L"([[:digit:]\\)])(n)", rx), L"$1*$2"},
    // cut + if -+ or +-
    {std::wregex(L"(\\+-|-\\+)", rx), L"-"},
    // replace ++ and --
    {std::wregex(L"(\\+\\+|--)", rx), L"+"},
    // cutoff + if after /, *,  ^, (, e, E
    {std::wregex(L"([/\\*^\\(eE])(\\+)", rx), L"$1"},
    // preprocess + at start of string
    {std::wregex(L"(^\\+)", rx), L""},
  };


  static const std::vector<prepRule> postRun
  {
    // delimit unary - for - follow by numbers or functions
    {std::wregex(L"(^|[^[:digit:]eEn\\)!])(-)", rx), L"$1N"},
  };


  std::wstring ws{input};
  std::wstring wsOld{};

  for (prepRule r : syntaxRun) {
    if(std::regex_search(ws, r.reg)){
      return L"";
    }
  
  }

  for (prepRule r : preRun) 
  {
    ws = std::regex_replace(ws, r.reg, r.rep);
  }

  while (wsOld != ws) 
  {
    wsOld = ws;

    for (prepRule r : loopRun) 
    {
      ws = std::regex_replace(ws, r.reg, r.rep);
    }
  }

  for (prepRule r : postRun) 
  {
    ws = std::regex_replace(ws, r.reg, r.rep);
  }

  return ws;
}


// ***Tokenize the preprocessed string***
//
std::vector<std::wstring> iamath::baseCalc::tokenize(const std::wstring& input) const
{
  std::vector<std::wstring> tokens{};
  std::wstring temp{};
  wchar_t var{L'n'};

  for (wchar_t c : input) 
  {
    if ((std::isdigit(c) || c == L'.' || c == L'e' || c == L'E')) 
    {  
      temp += c;
    }
    else if (!temp.empty() && (temp.back() == L'e' || temp.back() == L'E')) { // case for negative exponents
      temp += c;
    }
    else if (c == var) 
    {
      if (temp != L"") 
      {
        tokens.push_back(temp);
        temp = L"";
      }
      tokens.push_back(std::wstring{var});
    }
    else if (operatorMap_.find(c) != operatorMap_.end()) 
    {
      if (temp != L"") 
      {
        tokens.push_back(temp);
        temp = L"";
      }
      tokens.push_back(std::wstring{c});
    }
    else 
    {
      continue;
    }
  }

  if (temp != L"") {
    tokens.push_back(temp);
  }
  // for (std::wstring s : tokens) {
  //   std::wcout << s << std::endl;
  // }
  return tokens;
}



// ***Convert to reverse polish notation RPN***
//
 std::deque<std::wstring> iamath::baseCalc::convertRPN(const std::vector<std::wstring>& input) const
{

  stOpRpn newOp{};
  wchar_t op{};
  bool error{false};
  std::deque<std::wstring> errorReturn{};
  std::stack<stOpRpn> operatorStack{{stOpRpn{L'0',0}}};
  std::stack<stRightAssocFunc> functionStack{}; //function stack for nested right associative functions
  std::deque<std::wstring> symbolQueue{};  

  //regex expressions to catch e.g. unary minus on digits or functions
  std::wregex regOnDigit(L"[[:digit:]n]");

  for (std::wstring t : input) 
  {
    if (std::regex_search(t, regOnDigit)) 
    {
      symbolQueue.push_back(t);
    } 
    else if (operatorMapFunc_.find(t.back()) != operatorMapFunc_.end()) 
    {  //right associative function
      functionStack.push(stRightAssocFunc{t.back(), 0}); //dont negate function result
    }
    else if (t.front() == L')') 
    {
      while (!operatorStack.empty() && operatorStack.top().op != L'(') 
      {
        symbolQueue.push_back(std::wstring{operatorStack.top().op});
        operatorStack.pop();
      }

      if (!operatorStack.empty()) {operatorStack.pop();} else{error = true; return errorReturn;}; //pop right parenthesis

      if (!functionStack.empty() && functionStack.top().parentCount == 1) 
      {  //check for r.a. func on func stack. If so, add the func after the closing parenthesis was found
        symbolQueue.push_back(std::wstring{functionStack.top().func});
        functionStack.pop();
      }
      else if(!functionStack.empty())
      {
        functionStack.top().parentCount--;
      }
    }
    else if (operatorMap_.find(t.back()) != operatorMap_.end()) 
    {
      newOp.op = t.back();
      newOp.prio = operatorMap_.at(newOp.op);
      
      if (newOp.op == L'(' && !functionStack.empty()) 
      {
        functionStack.top().parentCount++;
      }

      if (!operatorStack.empty() && operatorStack.top().prio < newOp.prio) 
      {
        operatorStack.push(newOp);
      }
      else
      {
        while (!operatorStack.empty() && operatorStack.top().prio >= newOp.prio && operatorStack.top().op != L'(') 
        {
          symbolQueue.push_back(std::wstring{operatorStack.top().op});
          operatorStack.pop();
        }
        operatorStack.push(newOp);
      }
      

    }
  }


  while (!operatorStack.empty() && operatorStack.top().op != L'0') 
  {
      symbolQueue.push_back(std::wstring{operatorStack.top().op});
      operatorStack.pop();
    }

  // for (std::wstring w : symbolQueue) {
  //   std::wcout<<w<<" ";
  //
  // }
  //   std::wcout<<std::endl;

  if (error) {
    return errorReturn;
  }
  else {
  return symbolQueue;
  }
}



// ***Evaluate RPN***
double iamath::calcInSingle::evalRPN(std::deque<std::wstring>& queue) const
{
  bool error{false};
  double errorReturn{0};
  std::stack<double> operandStack{};
  double val1{}, val2{}, result{};
  wchar_t prefixNeg{L'#'}, var{L'n'};

  std::wregex regOnDigit(L"[[:digit:]]");
  std::wregex regOnOpFunc(L"[^[:digit:]]");

  while (!queue.empty()) 
  {
    if (std::regex_search(queue.front(), regOnDigit)) 
    {
      operandStack.push(std::stod(queue.front()));
      queue.pop_front();
    }
    else if (std::regex_search(queue.front(), regOnOpFunc)) 
    {
      if (operatorMapBinary_.find(queue.front().back()) != operatorMapBinary_.end()) {
        if (!(operandStack.size() >= 2)) {
          error = true; return errorReturn;
        }
        switch (queue.front().back()) 
        { // get the "last" character of the wstring(queue element) which converts it to int... nice one
          case L'+':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      val2 = operandStack.top();
                      operandStack.pop();
                      result = val2 + val1;
                      operandStack.push(result);
                      break;
                    }
          case L'-':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      val2 = operandStack.top();
                      operandStack.pop();
                      result = val2 - val1;
                      operandStack.push(result);
                      break;
                    }
          case L'*':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      val2 = operandStack.top();
                      operandStack.pop();
                      result = val2 * val1;
                      operandStack.push(result);
                      break;
                    }
          case L'/':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      val2 = operandStack.top();
                      operandStack.pop();
                      result = val2 / val1;
                      operandStack.push(result);
                      break;
                    }
          case L'^':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      val2 = operandStack.top();
                      operandStack.pop();
                      result = pow(val2, val1);
                      operandStack.push(result);
                      break;
                    }
          default:  {
                      std::wcout<< "Couldnt find operator "<<queue.front()<<std::endl;
                      return errorReturn;
                    }
        }
      
      }
      else{
        if (!(operandStack.size() >= 1)) {
          error = true; return errorReturn;
        }
        switch (queue.front().back()) 
        { // get the "last" character of the wstring(queue element) which converts it to int... nice one
          case L'N':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = 0.0 - val1;
                      operandStack.push(result);
                      break;
                    }
          case L'&':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = sqrt(val1);
                      operandStack.push(result);
                      break;
                    }
          case L'~':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = sin(val1);
                      operandStack.push(result);
                      break;
                    }
          case L'@':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = cos(val1);
                      operandStack.push(result);
                      break;
                    }
          case L'§':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = tan(val1);
                      operandStack.push(result);
                      break;
                    }
          case L'$':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = log(val1);
                      operandStack.push(result);
                      break;
                    }
          case L'€':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = log10(val1);
                      operandStack.push(result);
                      break;
                    }
          case L'!':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = factorial(val1);
                      operandStack.push(result);
                      break;
                    }
          default:  {
                      std::wcout<< "Couldnt find operator "<<queue.front()<<std::endl;
                      return errorReturn;
                    }
        }

      }
        queue.pop_front();
    }
  }
  if (operandStack.empty() || error) {
    error = true;
    return errorReturn;
  }
  else {
    result = operandStack.top();
  }

  return result;
}

std::vector<double> iamath::calcInSeq::evalRPN(std::deque<std::wstring> queue, int start, int end, float inc) const{
  bool error{false};
  std::vector<double> errorReturn{};
  std::stack<double> operandStack{};
  std::vector<double> resultVec{};
  std::deque<std::wstring> orgQueue{queue};
  wchar_t prefixNeg{L'#'}, var{L'n'};
  
  double val1{}, val2{}, result{};

  std::wregex regOnDigit(L"[[:digit:]n]");
  std::wregex regOnOpFunc(L"[^[:digit:]n]");

  for (int n = start; n <=end; n+= inc) 
  {
    queue = orgQueue;
  
    while (!queue.empty()) {
      //check if queue front is variable
      if (queue.front() == std::wstring{var}) 
      {
        queue.pop_front();
        queue.push_front(std::to_wstring(n));
      }
      if (std::regex_search(queue.front(), regOnDigit)) 
      {
        operandStack.push(std::stod(queue.front()));
        queue.pop_front();
      }
      else if (std::regex_search(queue.front(), regOnOpFunc)) 
    {
      if (operatorMapBinary_.find(queue.front().back()) != operatorMapBinary_.end()) {
        if (!(operandStack.size() >= 2)) {
          error = true; return errorReturn;
        }
        switch (queue.front().back()) 
        { // get the "last" character of the wstring(queue element) which converts it to int... nice one
          case L'+':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      val2 = operandStack.top();
                      operandStack.pop();
                      result = val2 + val1;
                      operandStack.push(result);
                      break;
                    }
          case L'-':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      val2 = operandStack.top();
                      operandStack.pop();
                      result = val2 - val1;
                      operandStack.push(result);
                      break;
                    }
          case L'*':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      val2 = operandStack.top();
                      operandStack.pop();
                      result = val2 * val1;
                      operandStack.push(result);
                      break;
                    }
          case L'/':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      val2 = operandStack.top();
                      operandStack.pop();
                      result = val2 / val1;
                      operandStack.push(result);
                      break;
                    }
          case L'^':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      val2 = operandStack.top();
                      operandStack.pop();
                      result = pow(val2, val1);
                      operandStack.push(result);
                      break;
                    }
          default:  {
                      std::wcout<< "Couldnt find operator "<<queue.front()<<std::endl;
                      return errorReturn;
                    }
        }
      
      }
      else{
        if (!(operandStack.size() >= 1)) {
          error = true; return errorReturn;
        }
        switch (queue.front().back()) 
        { // get the "last" character of the wstring(queue element) which converts it to int... nice one
          case L'N':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = 0.0 - val1;
                      operandStack.push(result);
                      break;
                    }
          case L'&':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = sqrt(val1);
                      operandStack.push(result);
                      break;
                    }
          case L'~':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = sin(val1);
                      operandStack.push(result);
                      break;
                    }
          case L'@':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = cos(val1);
                      operandStack.push(result);
                      break;
                    }
          case L'§':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = tan(val1);
                      operandStack.push(result);
                      break;
                    }
          case L'$':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = log(val1);
                      operandStack.push(result);
                      break;
                    }
          case L'€':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = log10(val1);
                      operandStack.push(result);
                      break;
                    }
          case L'!':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = factorial(val1);
                      operandStack.push(result);
                      break;
                    }
          default:  {
                      std::wcout<< "Couldnt find operator "<<queue.front()<<std::endl;
                      return errorReturn;
                    }
        }

      }
        queue.pop_front();
    }
    }
    resultVec.push_back(operandStack.top());
    operandStack.pop();
  }


  return resultVec;
}


double iamath::baseCalc::factorial(int value) const
{
  if (value<=0) 
  {
    return 0.0;
  }

  double result{1.0};
  while (value >= 1) 
  {
    result *= value;
    value--;
  }
  return result;
}
