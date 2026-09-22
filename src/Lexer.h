#pragma once

#include <string>
#include <cstddef>

class Lexer {
public:
  struct Token{
    enum class Type{
      Finished,
      BraceLeft,
      BraceRight,
      Identifier,
      String
    };
    Type type;
    std::string text;
    size_t xcol;
    size_t yrow;
    Token():type{Type::Finished}, text{}{};
    Token(Type tp, std::string str, size_t x, size_t y):type{tp}, text{str}, xcol(x), yrow(y){};
    std::string getPosText() const { return "yrow = " + std::to_string(yrow) + ", xcol = " + std::to_string(xcol); }
  };

  Lexer(const char *data, size_t size) : YYCURSOR(data), YYLIMIT(data + size), y(1),x(1){};
  
  Token lex();

  bool hasError(){ return !lastError.empty(); }

  std::string getLastError() const { return lastError; }

private:
  const char *YYCURSOR;
  const char *YYLIMIT;
  const char *YYMARKER;
  size_t x;
  size_t y;
  const char *t;
  size_t tx;
  size_t ty;
  std::string lastError;
};
