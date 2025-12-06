#include "stringHandler.hpp"
#include <cmath>
#include <deque>
#include <stack>
#include <cctype>
#include <string>
#include <vector>
#include <iostream>
#include <regex>

stringHandler::stringHandler(): 
  input_{}, 
  tokens_{}
{}

stringHandler::stringHandler(std::wstring input): 
  input_{input}, 
  tokens_{}
{}

stringHandler::stringHandler(std::wstring input, wchar_t delimBegin, wchar_t delimEnd): 
  input_{input}, 
  tokens_{}
{}


// ***Preprocess the given string***
//
std::wstring stringHandler::preprocess(std::wstring in){

  std::wstring ws{in};
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

//  first preprocess
  while(ws != wsOld){

    wsOld = ws;
    // insert * between digit or n or ! and opening parentheses
    static const std::wregex reg1(LR"(([[:digit:]]|n|!)(\())", rx_);   
    ws = std::regex_replace(ws, reg1, L"$1*$2");
  
    // insert * between digit or closing parentheses and n
    static const std::wregex reg2(LR"(([[:digit:]]|\))(n))", rx_);   
    ws = std::regex_replace(ws, reg2, L"$1*$2");
  
    // cut + if -+ or +-
    std::wregex reg3(LR"(\+-|-\+)", rx_);   
    ws = std::regex_replace(ws, reg3, L"-");
  
    // replace ++ and --
    std::wregex reg4(LR"(\+\+|--)", rx_);   
    ws = std::regex_replace(ws, reg4, L"+");
  
    // cutoff + if after /, *, s, ^, (
    std::wregex reg5(LR"((/|\*|&|\^|\()(\+)([[:digit:]]))", rx_);   
    ws = std::regex_replace(ws, reg5, L"$1$3");
  
  }
   
//  second preprocess
  

    // preprocess + at start of string
    std:: wregex reg54(LR"(^\+)", rx_);   
    ws = std:: regex_replace(ws, reg54,L"");
  
    // delimit unary - for - follow by numbers or functions
    std:: wregex reg55(LR"((^|[^[:digit:]eEn)!])(-))", rx_);   
    ws = std:: regex_replace(ws, reg55,L"$1" + std::wstring{prefixNeg_});
  
  
    std::wcout << ws << std::endl;
  
  //----------------------------
  // need to insert errors e.g. .n or 1. or /* etc.

  return ws;
}


// ***Tokenize the preprocessed string***
//
void stringHandler::tokenize(){
  std::wstring temp{};
  bool bNegNumber{false};

  for (wchar_t c : input_) {

    if ((std::isdigit(c) || c == L'.') || bNegNumber == true && (std::isdigit(c) || c == L'.' || (operatorMapFunc_.find(c) != operatorMapFunc_.end()))) {  // normal or negative number
      temp += c;
    }
    else if (c == prefixNeg_ && !bNegNumber) {
      temp += prefixNeg_;
      bNegNumber = true;
      continue;
    }
    else if (c == prefixNeg_ && bNegNumber) {
      tokens_.push_back(temp);
      temp = prefixNeg_;
    }

    else if (c == var1_) {  

      if (temp != L"") {
        tokens_.push_back(temp);
        temp = L"";
        bNegNumber = false;
      }
      temp += var1_;
      tokens_.push_back(temp);
      temp = L"";
       
    }

    else if (operatorMap_.find(c) != operatorMap_.end()) {
      if (temp != L"") {
        tokens_.push_back(temp);
        temp = L"";
        bNegNumber = false;
      }

      tokens_.push_back(std::wstring{c});
    }
    else {
      continue;
    }
  }

  if (temp != L"") {
    tokens_.push_back(temp);
    bNegNumber = false;
  }
  for (std::wstring s : tokens_) {
    std::wcout << s << std::endl;
  }
}



// ***Convert to reverse polish notation RPN***
//
 std::deque<std::wstring> stringHandler::convertRPN(){

  stOpRpn newOp{};
  wchar_t op{};
  std::stack<stOpRpn> operatorStack{{stOpRpn{L'0',0}}};
  std::stack<stRightAssocFunc> functionStack{}; //function stack for nested right associative functions
  std::deque<std::wstring> symbolQueue{};  

  //regex expressions to catch e.g. unary minus on digits or functions
  std::wregex regOnDigit(L"([[:digit:]]|" + std::wstring{var1_} + L")");
  std::wregex regOnFunction(LR"(([^[:digit:]|\(\)]))");

  bool unaryOnFunction{false};


  for (std::wstring t : tokens_) {

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
        std::wcout << "parent size -- "<<std::endl;
        std::wcout << "stack top parent count: " << functionStack.top().parentCount<<std::endl;
      }
    }
    else if (operatorMap_.find(t.back()) != operatorMap_.end()) {
      newOp.op = t.back();
      newOp.prio = operatorMap_.at(newOp.op);
      
      if (newOp.op == L'(' && !functionStack.empty()) {
        functionStack.top().parentCount++;
        std::wcout << "parent size ++ "<<std::endl;
        std::wcout << "stack top parent count: " << functionStack.top().parentCount<<std::endl;
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

  for (std::wstring w : symbolQueue) {
    std::wcout<<w;
  
  }
    std::wcout<<std::endl;

  return symbolQueue;
}



// ***Evaluate RPN***
int stringHandler::evalRPN(std::deque<std::wstring> queue){
  std::stack<double> operandStack{};
  std::wcout << queue.front()<< std::endl;
  double val1{}, val2{}, result{};

  std::wregex regOnDigit(LR"([[:digit:]|n])");
  std::wregex regOnOpFunc(LR"([^[:digit:]|n])");

  while (!queue.empty()) {
    std::wcout<< "New cycle..."<<std::endl;
    if (std::regex_search(queue.front(), regOnDigit)) {
      operandStack.push(std::stod(queue.front()));
      std::wcout<< "Operand pushed: "<<operandStack.top()<<std::endl;
      queue.pop_front();
    }
    else if (std::regex_search(queue.front(), regOnOpFunc)) {
      std::wcout<< "Operator found: "<<queue.front().back()<<std::endl;

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
        std::wcout<< "Result pushed: "<<result<<std::endl;
        queue.pop_front();
    }
  }

  return 0;
}



double stringHandler::factorial(double value){
  double result{1.0};

  while (value >= 1) {
    result *= value;
    value--;
  }
  return result;
}










