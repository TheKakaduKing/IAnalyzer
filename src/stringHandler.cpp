#include "hdr/stringHandler.hpp"
#include <deque>
#include <stack>
#include <cctype>
#include <string>
#include <vector>
#include <iostream>
#include <regex>

stringHandler::stringHandler(std::string input): 
  input_{input}, 
  tokens_{}
{}

stringHandler::stringHandler(): 
  input_{}, 
  tokens_{}
{}

std::string stringHandler::preprocess(std::string in){
  static constexpr std::regex_constants::syntax_option_type rx = std::regex_constants::extended;

  std::string ws{in};
  std::string wsOld{};

//  first run
while(ws != wsOld){

  wsOld = ws;
  // insert * between digit or n or ! and opening parentheses
  static const std::regex reg1(R"(([[:digit:]]|n|!)(\())", rx);   
  ws = std::regex_replace(ws, reg1, "$1*$2");

  // insert * between digit or closing parentheses and n
  static const std::regex reg2(R"(([[:digit:]]|\))(n))", rx);   
  ws = std::regex_replace(ws, reg2, "$1*$2");

  // cut + if -+ or +-
  std::regex reg3(R"(\+-|-\+)", rx);   
  ws = std::regex_replace(ws, reg3, "-");

  // replace ++ and --
  std::regex reg4(R"(\+\+|--)", rx);   
  ws = std::regex_replace(ws, reg4, "+");

  // cutoff + if after /, *, s, ^
  std::regex reg5(R"((/|\*|s|\^)(\+)([[:digit:]]))", rx);   
  ws = std::regex_replace(ws, reg5, "$1$3");

}

// second run
wsOld = "";
while(ws != wsOld){
  wsOld = ws;

  // preprocess sin to ~
  std::regex reg50(R"((sin))", rx);   
  ws = std::regex_replace(ws, reg50, "~");

  // preprocess cos to @
  std::regex reg51(R"((cos))", rx);   
  ws = std::regex_replace(ws, reg51, "@");

  // preprocess tan to $ for
  std::regex reg52(R"((tan))", rx);   
  ws = std::regex_replace(ws, reg52, "{");

  // preprocess + at start of string
  std::regex reg53(R"(^\+)", rx);   
  ws = std::regex_replace(ws, reg53, "");

  // delimit unary -
  std::regex reg54(R"((^|\/|\*|s|\~|\@|\{|\^|\()(\-))", rx);   
  ws = std::regex_replace(ws, reg54, "$1#$2");

  // preprocess sqrt to &
  std::regex reg55(R"((s))", rx);   
  ws = std::regex_replace(ws, reg55, "&");

  // preprocess - if in front of a function or Parentheses
  // e.g. -(, -sin, -sqrt
  std::regex reg56(R"((\-)(s|\~|\@|\{|\())", rx);   
  ws = std::regex_replace(ws, reg56, "-1*$2");

}
//----------------------------
// need to insert errors e.g. .n or 1. or /* etc.

  return ws;
}

void stringHandler::tokenize(){
  std::string delim{"["};
  std::string temp{delim};
  bool negNumber = false;

  for (char c : input_) {

    if ((std::isdigit(c) || c == '.') || negNumber == true && (std::isdigit(c) || c == '.' || c == '-') ) {  // normal or negative number
      temp += c;
    }
    else if (c == '#') {
      negNumber = true;
      continue;
    }

    else if (c == 'n') {  

      if (temp != delim) {
        temp += delim;
        tokens_.push_back(temp);
        temp = delim;
        negNumber = false;
      }
      tokens_.push_back(delim + "n" + delim);
       
    }

    else if (operatorMap_.find(std::string{c}) != operatorMap_.end()) {
      if (temp != delim) {
        temp += delim;
        tokens_.push_back(temp);
        temp = delim;
        negNumber = false;
      }

      tokens_.push_back(std::string{c});
    }
    else {
      continue;
    }
  }

  if (temp != delim) {
    temp += delim;
    tokens_.push_back(temp);
    negNumber = false;
  }
  for (std::string s : tokens_) {
    std::cout << s << std::endl;
  }
}


std::string stringHandler::convertRPN(){
  std::string rpn{};
  stOperator newOp{};
  std::stack<stOperator> operatorStack{{stOperator{"0",0}}};
  std::deque<std::string> symbolQueue{};


  for (std::string t : tokens_) {
    if (operatorMap_.find(t) == operatorMap_.end()) {
      symbolQueue.push_back(t);
    }
    else{

      newOp.op = t; newOp.priority = operatorMap_.at(t);

      while (!operatorStack.empty()) {

        if(newOp.op == ")") {
          while (operatorStack.top().op != "(") {
            if (operatorStack.top().op == "0") {
              std::cout << "Parentheses error!" << std::endl;
              return "NULL";
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
          if (operatorStack.top().op =="(") {
            operatorStack.push(newOp);
            break; 
          }
          else {
            symbolQueue.push_back(operatorStack.top().op);
            operatorStack.pop();
          }
        }
        else {
          std::cout << "stack top:" << operatorStack.top().op << std::endl;
          std::cout << "current op:" << newOp.op << std::endl;
          return "false operation. return NULL";
        }
      }
    }
  }

    while (operatorStack.top().op != "0") {
      symbolQueue.push_back(operatorStack.top().op);
      operatorStack.pop();
    }

  for (std::string t : symbolQueue) {
    rpn += t;
  }


  return rpn;
}
