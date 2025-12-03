#include "hdr/stringHandler.hpp"
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
  tokens_{},
  delimBegin_{delimBegin},
  delimEnd_{delimEnd}
{}


// ***Preprocess the given string***
//
std::wstring stringHandler::preprocess(std::wstring in){
  static constexpr std::regex_constants::syntax_option_type rx = std::regex_constants::extended;

  std::wstring ws{in};
  std::wstring wsOld{};

//  first run
  while(ws != wsOld){
  
    wsOld = ws;
    // insert * between digit or n or ! and opening parentheses
    static const std::wregex reg1(LR"(([[:digit:]]|n|!)(\())", rx);   
    ws = std::regex_replace(ws, reg1, L"$1*$2");
  
    // insert * between digit or closing parentheses and n
    static const std::wregex reg2(LR"(([[:digit:]]|\))(n))", rx);   
    ws = std::regex_replace(ws, reg2, L"$1*$2");
  
    // cut + if -+ or +-
    std::wregex reg3(LR"(\+-|-\+)", rx);   
    ws = std::regex_replace(ws, reg3, L"-");
  
    // replace ++ and --
    std::wregex reg4(LR"(\+\+|--)", rx);   
    ws = std::regex_replace(ws, reg4, L"+");
  
    // cutoff + if after /, *, s, ^
    std::wregex reg5(LR"((/|\*|s|\^)(\+)([[:digit:]]))", rx);   
    ws = std::regex_replace(ws, reg5, L"$1$3");
  
  }
  
  // second run
  wsOld = L"";
  while(ws != wsOld){
    wsOld = ws;
  
    // preprocess sin to ~
    std:: wregex reg50(LR"((sin))", rx);   
    ws = std:: regex_replace(ws, reg50,L"~");
  
    // preprocess cos to @
    std:: wregex reg51(LR"((cos))", rx);   
    ws = std:: regex_replace(ws, reg51,L"@");
  
    // preprocess tan to § for
    std:: wregex reg52(LR"((tan))", rx);   
    ws = std:: regex_replace(ws, reg52,L"§");
  
    // preprocess + at start of string
    std:: wregex reg53(LR"(^\+)", rx);   
    ws = std:: regex_replace(ws, reg53,L"");
  
    // delimit unary -
    std:: wregex reg54(LR"((^|\/|\*|s|\~|\@|\§|\^|\()(\-))", rx);   
    ws = std:: regex_replace(ws, reg54,L"$1#$2");
  
    // preprocess sqrt to &
    std:: wregex reg55(LR"((s))", rx);   
    ws = std:: regex_replace(ws, reg55,L"&");
  
    // preprocess - if in front of a function or Parentheses
    // e.g. -(, -sin, -sqrt
    std:: wregex reg56(LR"((\-)(s|\~|\@|\§|\())", rx);   
    ws = std:: regex_replace(ws, reg56,L"-1*$2");
  
  }
  //----------------------------
  // need to insert errors e.g. .n or 1. or /* etc.

  return ws;
}


// ***Tokenize the preprocessed string***
//
void stringHandler::tokenize(){
  std::wstring temp{delimBegin_};
  bool negNumber = false;

  for (wchar_t c : input_) {

    if ((std::isdigit(c) || c == L'.') || negNumber == true && (std::isdigit(c) || c == L'.' || c == L'-') ) {  // normal or negative number
                                                                                                                
      if (c == L'-') {
        temp += prefixNeg_;
      }
      else {
        temp += c;
      }
    }
    else if (c == L'#') {
      negNumber = true;
      continue;
    }

    else if (c == L'n') {  

      if (temp != std::wstring{delimBegin_}) {
        temp += delimEnd_;
        tokens_.push_back(temp);
        temp = delimBegin_;
        negNumber = false;
      }
      tokens_.push_back(std::wstring{delimBegin_ + L'n' + delimBegin_});
       
    }

    else if (operatorMap_.find(c) != operatorMap_.end()) {
      if (temp != std::wstring{delimBegin_}) {
        temp += delimEnd_;
        tokens_.push_back(temp);
        temp = delimBegin_;
        negNumber = false;
      }

      tokens_.push_back(std::wstring{c});
    }
    else {
      continue;
    }
  }

  if (temp != std::wstring{delimBegin_}) {
    temp += delimEnd_;
    tokens_.push_back(temp);
    negNumber = false;
  }
  for (std::wstring s : tokens_) {
    std::wcout << s << std::endl;
  }
}



// ***Convert to reverse polish notation RPN***
//
std::wstring stringHandler::convertRPN(){
  std::wstring rpn{};
  stOperator newOp{};
  wchar_t c{};
  std::stack<stOperator> operatorStack{{stOperator{L"0",0}}};
  std::deque<std::wstring> symbolQueue{L" "};   // initialize with whitespace so we can use that as a limit for our evaluation (rpn.being is whitespace)


  for (std::wstring t : tokens_) {
    c = t[0];
    if (operatorMap_.find(c) == operatorMap_.end()) {
      symbolQueue.push_back(t);
    }
    else{

      newOp.op = t; newOp.priority = operatorMap_.at(c);

      while (!operatorStack.empty()) {

        if(newOp.op == L")") {
          while (operatorStack.top().op != L"(") {
            if (operatorStack.top().op == L"0") {
              std::cout << "Parentheses error!" << std::endl;
              return L"NULL";
            }
            else {
              symbolQueue.push_back(operatorStack.top().op);
              operatorStack.pop();
            }
          }
          operatorStack.pop();
          break;
        }
        else if(operatorStack.top().priority < newOp.priority) {
          operatorStack.push(newOp);
          break;
        }

        else if (operatorStack.top().priority >= newOp.priority) {
          if (operatorStack.top().op == L"(") {
            operatorStack.push(newOp);
            break; 
          }
          else {
            symbolQueue.push_back(operatorStack.top().op);
            operatorStack.pop();
          }
        }
        else {
          std::wcout << "stack top:" << operatorStack.top().op << std::endl;
          std::wcout << "current op:" << newOp.op << std::endl;
          return L"false operation. return NULL";
        }
      }
    }
  }

    while (operatorStack.top().op != L"0") {
      symbolQueue.push_back(operatorStack.top().op);
      operatorStack.pop();
    }

  for (std::wstring t : symbolQueue) {
    rpn += t;
  }


  return rpn;
}



// ***Evaluate RPN***
// e.g. <-20.0><-9.0><1>+*<-1>&/<5><2>^*<2>!*<1.4><-1>*<-58>~*<1>@*-<-3>§+
void stringHandler::evalRPN(std::wstring rpn){

  std::wcout << rpn << std::endl;

  std::wstring workStr{rpn};
  std::wstring::iterator it{workStr.begin()}; 

  std::stack<double> numStack{};
  std::stack<wchar_t> opStack{};

  std::wstring tempStr{};

  // extract all numbers and push them to the number stack from left to right
  while (it != workStr.end()) {
    if (*it == delimBegin_) {
      it++;
      while (*it != delimEnd_) {
        if (*it == L'#') {
          tempStr.push_back(L'-');
          it++;
        }
        else {
        tempStr.push_back(*it);
        it++;
        }
      }
      // std::wcout << tempStr << std::endl;
      numStack.push(std::stod(tempStr));
      tempStr = L"";
    }
    it++;
  } 

  it = workStr.end();

  // extract all operators and push them to the operator stack from right to left
  while (it != workStr.begin()) {
    if (operatorMapNoP_.find(*it) != operatorMapNoP_.end()) {
      opStack.push(*it);
    } 
    it--;
  } 

  for (int i = numStack.size(); i>0;i--) {
    std::wcout << numStack.top() << std::endl;
    numStack.pop();
  }
  for (int i = opStack.size(); i>0;i--) {
    std::wcout << opStack.top() << std::endl;
    opStack.pop();
  }
}


std::wstring stringHandler::updateString(std::wstring& workStr, std::wstring::iterator it_start, std::wstring num1, std::wstring num2, wchar_t op){

  const int len(num1.length() + num2.length() + 5);  // +4 because of delimiters <><> +1 to delete the operator in erase function
  std::wstring::iterator it_end{it_start + len};
  std::wstring resultStr{};
  double result{};


  // perform operation
  if (operatorMapBinary_.find(op) != operatorMapBinary_.end()) {

    double n1{std::stod(num1)},n2{std::stod(num2)};

    switch (op) {
      case L'+':{
                  result = n1 + n2;
                  break;
                } 
      case L'-':{
                  result = n1 - n2;
                  break;
                } 
      case L'*':{
                  result = n1 * n2;
                  break;
                } 
      case L'/':{
                  result = n1 / n2;
                  break;
                } 
      case L'^':{
                  result = std::pow(n1,n2);
                  break;
                } 
      default:{
                return L""; // error
              }

    }
  
  }
  else {
    return L"";   // error
  }


  resultStr += std::to_wstring(result);

  workStr.erase(it_start, it_end);

  workStr.insert(it_start, delimEnd_);
  for (int i = 0; i < resultStr.length(); i++) {      // ainsert characters of result string in reverse
    workStr.insert(it_start, *((resultStr.end() - i) -1));
  }
  workStr.insert(it_start, delimBegin_);

  std::wcout << workStr << std::endl;
  return L"alo";
}

std::wstring stringHandler::updateString(const std::wstring& in, std::wstring::iterator it_start, std::wstring num1, wchar_t op){
  return L"alo";
}













