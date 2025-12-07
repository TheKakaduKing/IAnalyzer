#include "stringHandler.hpp"
#include <cmath>
#include <deque>
#include <stack>
#include <cctype>
#include <string>
#include <vector>
#include <iostream>
#include <regex>




// ***Preprocess the given string***
//
std::wstring stringHandler::preprocess(const std::wstring& input) const{

  std::wstring ws{input};
  std::wstring wsOld{};


    // preprocess sin to ~
    std:: wregex reg50(LR"((sin))", rx_);   
    ws = std:: regex_replace(ws, reg50,L"~");
  
    // preprocess cos to @
    std:: wregex reg51(LR"((cos))", rx_);   
    ws = std:: regex_replace(ws, reg51,L"@");
  
    // preprocess tan to § for
    std:: wregex reg52(LR"((tan))", rx_);   
    ws = std:: regex_replace(ws, reg52,L"§");
  
    // preprocess sqrt to &
    std:: wregex reg53(LR"((s))", rx_);   
    ws = std:: regex_replace(ws, reg53,L"&");

    // preprocess log to %
    std:: wregex reg56(LR"((log))", rx_);   
    ws = std:: regex_replace(ws, reg56,L"$");

//  first preprocess
  while(ws != wsOld){

    wsOld = ws;
    // insert * between digit or n or ! and opening parentheses
    static const std::wregex reg1(L"([[:digit:]" + std::wstring{var1_} + L"!])(\\()", rx_);   
    ws = std::regex_replace(ws, reg1, L"$1*$2");

    // insert * between digit or closing parentheses and n
    static const std::wregex reg2(L"([[:digit:]\\)])(" + std::wstring{var1_} + L")", rx_);   
    ws = std::regex_replace(ws, reg2, L"$1*$2");

    // cut + if -+ or +-
    std::wregex reg3(L"(\\+-|-\\+)", rx_);   
    ws = std::regex_replace(ws, reg3, L"-");

    // // replace ++ and --
    std::wregex reg4(L"(\\+\\+|--)", rx_);   
    ws = std::regex_replace(ws, reg4, L"+");

    // cutoff + if after /, *,  ^, (
    std::wregex reg5(L"([/\\*^\(])(\\+)([[:digit:]|" + std::wstring{var1_} + L"])", rx_);   
    ws = std::regex_replace(ws, reg5, L"$1$3");

  }
   
//  second preprocess
  

    // preprocess + at start of string
    std:: wregex reg54(L"(^\\+)", rx_);   
    ws = std:: regex_replace(ws, reg54,L"");

    // // delimit unary - for - follow by numbers or functions
    std:: wregex reg55(L"(^|[^[:digit:]eEn)!])(-)", rx_);   
    ws = std:: regex_replace(ws, reg55,L"$1" + std::wstring{prefixNeg_});
  
  
    std::wcout << ws << std::endl;
  
  //----------------------------
  // need to insert errors e.g. .n or 1. or /* etc.

  return ws;
}


// ***Tokenize the preprocessed string***
//
std::vector<std::wstring> stringHandler::tokenize(const std::wstring& input) const{
  std::vector<std::wstring> tokens{};
  std::wstring temp{};
  bool bNegNumber{false};

  for (wchar_t c : input) {

    if ((std::isdigit(c) || c == L'.') || bNegNumber == true && (std::isdigit(c) || c == L'.' || (operatorMapFunc_.find(c) != operatorMapFunc_.end()))) {  // normal or negative number
      temp += c;
    }
    else if (c == prefixNeg_ && !bNegNumber) {
      temp += prefixNeg_;
      bNegNumber = true;
      continue;
    }
    else if (c == prefixNeg_ && bNegNumber) {
      tokens.push_back(temp);
      temp = prefixNeg_;
    }

    else if (c == var1_) {  

      if (temp != L"") {
        tokens.push_back(temp);
        temp = L"";
        bNegNumber = false;
      }
      temp += var1_;
      tokens.push_back(temp);
      temp = L"";
       
    }

    else if (operatorMap_.find(c) != operatorMap_.end()) {
      if (temp != L"") {
        tokens.push_back(temp);
        temp = L"";
        bNegNumber = false;
      }

      tokens.push_back(std::wstring{c});
    }
    else {
      continue;
    }
  }

  if (temp != L"") {
    tokens.push_back(temp);
    bNegNumber = false;
  }
  // for (std::wstring s : tokens) {
  //   std::wcout << s << std::endl;
  // }
  return tokens;
}



// ***Convert to reverse polish notation RPN***
//
 std::deque<std::wstring> stringHandler::convertRPN(const std::vector<std::wstring>& input) const{

  stOpRpn newOp{};
  wchar_t op{};
  std::stack<stOpRpn> operatorStack{{stOpRpn{L'0',0}}};
  std::stack<stRightAssocFunc> functionStack{}; //function stack for nested right associative functions
  std::deque<std::wstring> symbolQueue{};  

  //regex expressions to catch e.g. unary minus on digits or functions
  std::wregex regOnDigit(L"([[:digit:]]|" + std::wstring{var1_} + L")");
  std::wregex regOnFunction(LR"(([^[:digit:]|\(\)]))");

  bool unaryOnFunction{false};


  for (std::wstring t : input) {

    if (std::regex_search(t, regOnDigit)) {
      if (t.front() == L'#') {
        t.erase(t.begin());
        symbolQueue.push_back(t);
        symbolQueue.push_back(L"N");
        }
      else{
        symbolQueue.push_back(t);
      }
    } 
    else if (operatorMapFunc_.find(t.back()) != operatorMapFunc_.end()) {  //right associative function
      if (t.front() == L'#') {
        functionStack.push(stRightAssocFunc{t.back(), true, 0});  //negate function result
      }
      else{
        functionStack.push(stRightAssocFunc{t.back(), false, 0}); //dont negate function result
      }
    }
    else if (t.front() == L')') {

      while (operatorStack.top().op != L'(' && operatorStack.size() != 0) {
        symbolQueue.push_back(std::wstring{operatorStack.top().op});
        operatorStack.pop();
      }

      operatorStack.pop();  //pop right parenthesis

      if (!functionStack.empty() && functionStack.top().parentCount == 1) {  //check for r.a. func on func stack. If so, add the func after the closing parenthesis was found
        symbolQueue.push_back(std::wstring{functionStack.top().func});
        if (functionStack.top().negateFunc) {
          symbolQueue.push_back(L"N");
        }
        functionStack.pop();
      }
      else if(!functionStack.empty()){
        functionStack.top().parentCount--;
      }
    }
    else if (operatorMap_.find(t.back()) != operatorMap_.end()) {
      newOp.op = t.back();
      newOp.prio = operatorMap_.at(newOp.op);
      
      if (newOp.op == L'(' && !functionStack.empty()) {
        functionStack.top().parentCount++;
      }

      if (operatorStack.top().prio < newOp.prio) {
        operatorStack.push(newOp);
      }
      else{
        while (operatorStack.top().prio >= newOp.prio && operatorStack.top().op != L'(') {
          symbolQueue.push_back(std::wstring{operatorStack.top().op});
          operatorStack.pop();
        }
        operatorStack.push(newOp);
      }
      

    }
  }


  while (operatorStack.top().op != L'0') {
      symbolQueue.push_back(std::wstring{operatorStack.top().op});
      operatorStack.pop();
    }

  // for (std::wstring w : symbolQueue) {
  //   std::wcout<<w;
  //
  // }
  //   std::wcout<<std::endl;

  return symbolQueue;
}



// ***Evaluate RPN***
double stringHandler::evalRPN(std::deque<std::wstring>& queue) const{
  std::stack<double> operandStack{};
  double val1{}, val2{}, result{};

  std::wregex regOnDigit(LR"([[:digit:]|n])");
  std::wregex regOnOpFunc(LR"([^[:digit:]|n])");

  while (!queue.empty()) {
    if (std::regex_search(queue.front(), regOnDigit)) {
      operandStack.push(std::stod(queue.front()));
      queue.pop_front();
    }
    else if (std::regex_search(queue.front(), regOnOpFunc)) {

      switch (queue.front().back()) { // get the "last" character of the wstring(queue element) which converts it to int... nice one
        case L'N':{
                    val1 = operandStack.top();
                    operandStack.pop();
                    result = 0.0 - val1;
                    operandStack.push(result);
                    break;
                  }
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
        case L'&':{
                    val1 = operandStack.top();
                    operandStack.pop();
                    result = std::sqrt(val1);
                    operandStack.push(result);
                    break;
                  }
        case L'^':{
                    val1 = operandStack.top();
                    operandStack.pop();
                    val2 = operandStack.top();
                    operandStack.pop();
                    result = std::pow(val2, val1);
                    operandStack.push(result);
                    break;
                  }
        case L'~':{
                    val1 = operandStack.top();
                    operandStack.pop();
                    result = std::sin(val1);
                    operandStack.push(result);
                    break;
                  }
        case L'@':{
                    val1 = operandStack.top();
                    operandStack.pop();
                    result = std::cos(val1);
                    operandStack.push(result);
                    break;
                  }
        case L'§':{
                    val1 = operandStack.top();
                    operandStack.pop();
                    result = std::tan(val1);
                    operandStack.push(result);
                    break;
                  }
        case L'$':{
                    val1 = operandStack.top();
                    operandStack.pop();
                    result = std::log(val1);
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
                  }
      }
        queue.pop_front();
    }
  }
  result = operandStack.top();

  return result;
}

std::vector<double> stringHandler::evalRPN(std::deque<std::wstring> queue, int start, int end, float inc) const{
  std::stack<double> operandStack{};
  std::vector<double> resultVec{};
  std::deque<std::wstring> orgQueue{queue};
  
  double val1{}, val2{}, result{};

  std::wregex regOnDigit(LR"([[:digit:]|n])");
  std::wregex regOnOpFunc(LR"([^[:digit:]|n])");

  for (int n = start; n <=end; n+= inc) {
    queue = orgQueue;
  
    while (!queue.empty()) {
      //check if queue front is variable
      if (queue.front() == std::wstring{var1_}) {
        queue.pop_front();
        queue.push_front(std::to_wstring(n));
        std::wcout << "front popped" << std::endl;
      
      }
      if (std::regex_search(queue.front(), regOnDigit)) {
        operandStack.push(std::stod(queue.front()));
        queue.pop_front();
      }
      else if (std::regex_search(queue.front(), regOnOpFunc)) {

        switch (queue.front().back()) { // get the "last" character of the wstring(queue element) which converts it to int... nice one
          case L'N':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = 0.0 - val1;
                      operandStack.push(result);
                      break;
                    }
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
          case L'&':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = std::sqrt(val1);
                      operandStack.push(result);
                      break;
                    }
          case L'^':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      val2 = operandStack.top();
                      operandStack.pop();
                      result = std::pow(val2, val1);
                      operandStack.push(result);
                      break;
                    }
          case L'~':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = std::sin(val1);
                      operandStack.push(result);
                      break;
                    }
          case L'@':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = std::cos(val1);
                      operandStack.push(result);
                      break;
                    }
          case L'§':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = std::tan(val1);
                      operandStack.push(result);
                      break;
                    }
          case L'$':{
                      val1 = operandStack.top();
                      operandStack.pop();
                      result = std::log(val1);
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
                    }
        }
          queue.pop_front();
      }
    }
    resultVec.push_back(operandStack.top());
    std::wcout << "operand stack size: " << operandStack.size() << std::endl;
    operandStack.pop();
  }


  return resultVec;
}


double stringHandler::factorial(double value) const{
  double result{1.0};

  while (value >= 1) {
    result *= value;
    value--;
  }
  return result;
}










