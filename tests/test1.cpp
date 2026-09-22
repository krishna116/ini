#include <iostream>
#include "Lexer.h"

using Token = Lexer::Token;

int test(){
  printf("=========================================test_good_str\n");
  std::string str = R"(;commmment line;
    [section1]
    key1 = value1;
    key2 = value2;
    key3 = "value3";
    [section2]
    key4 = "value4";
    key5 = "path = c:/file/file.h";
    key6 = "d:\\file\\file.cpp";
  )";

  Lexer lexer(str.data(), str.size());

  auto token = lexer.lex();
  while(token.type != Token::Type::Finished){
    printf("token = {%s}\n", token.text.c_str());
    token = lexer.lex();
  }

  if(lexer.hasError()){
    printf("%s\n", lexer.getLastError().c_str());
  }

  return lexer.hasError() ? 1 : 0;
}

int main(){
  return test();
}