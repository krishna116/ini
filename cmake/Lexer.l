#include "Lexer.h"

Lexer::Token Lexer::lex(){
  auto yyskip = [&](){ if(*(++YYCURSOR) == '\n'){ ++y; x = 1; }else{ ++x; } };
  auto yypeek = [&](){ return *YYCURSOR; };
  auto yybackup = [&](){ YYMARKER = YYCURSOR; };
  auto yyrestore = [&](){ YYCURSOR = YYMARKER; };
  auto getPosStr = [&](){ return std::string{" at y = " + std::to_string(y) + ", x = " + std::to_string(x)}; };

start:
  t = YYCURSOR;
  tx = x;
  ty = y;

  /*!re2c
  re2c:yyfill:enable = 0;
  re2c:define:YYCTYPE = "char";
  re2c:api = generic;
  re2c:api:style = free-form;
  re2c:YYPEEK   = "yypeek();";
  re2c:YYSKIP   = "yyskip();";
  re2c:YYBACKUP = "yybackup();";
  re2c:YYRESTORE= "yyrestore();";

  Spaces  = [ \t\v\f\r\n]+;
  CmTLine = ";" [^\n\x00]* [\r]? [\n];
  String  = ["];
  Ident   = [^ "=;[\]\t\v\r\n\f\x00]+;

  "["     { return Token{Token::Type::BraceLeft, std::string{t, (size_t)(YYCURSOR - t)}, tx, ty};}
  "]"     { return Token{Token::Type::BraceRight, std::string{t, (size_t)(YYCURSOR - t)}, tx, ty};}
  Ident   { return Token{Token::Type::Identifier, std::string{t, (size_t)(YYCURSOR - t)}, tx, ty};}

  String  { Token token{Token::Type::String, "", tx, ty};
            while(YYCURSOR < YYLIMIT){
              if(*YYCURSOR == '"'){
                ++YYCURSOR;
                return token;
              }else if(*YYCURSOR == '\\'){
                if(YYCURSOR + 1 < YYLIMIT){
                  token.text.push_back(*(YYCURSOR + 1));
                  YYCURSOR += 2;
                };
              } if(*YYCURSOR == '\n' || *YYCURSOR == '\r' || *YYCURSOR == 0){
                break;
              }else{
                token.text.push_back(*YYCURSOR);
                ++YYCURSOR;
              }
            }
            lastError = "string is not closed" + getPosStr();
            return Token{Token::Type::Finished, "", tx, ty};
            YYCURSOR = YYLIMIT;
          }

  "="     { goto start; }
  Spaces  { goto start; }
  CmTLine { goto start; }

  *       { lastError = "unknown char " + std::to_string(yych) + getPosStr(); 
            return Token{Token::Type::Finished, "", tx, ty}; 
            YYCURSOR = YYLIMIT;
          }

  [\x00]  { return Token{Token::Type::Finished, "", tx, ty}; }
  */
}