//
// Single include file for IniParser.
//
#pragma once

#include <string>
#include <vector>
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

{
	char yych;
	yych = yypeek();;
	switch (yych) {
		case 0x00: goto yy1;
		case '\t':
		case '\n':
		case '\v':
		case '\f':
		case '\r':
		case ' ': goto yy4;
		case '"': goto yy6;
		case ';': goto yy7;
		case '=': goto yy9;
		case '[': goto yy10;
		case ']': goto yy11;
		default: goto yy2;
	}
yy1:
	yyskip();
	{ return Token{Token::Type::Finished, "", tx, ty}; }
yy2:
	yyskip();
	yych = yypeek();;
	switch (yych) {
		case 0x00:
		case '\t':
		case '\n':
		case '\v':
		case '\f':
		case '\r':
		case ' ':
		case '"':
		case ';':
		case '=':
		case '[':
		case ']': goto yy3;
		default: goto yy2;
	}
yy3:
	{ return Token{Token::Type::Identifier, std::string{t, (size_t)(YYCURSOR - t)}, tx, ty};}
yy4:
	yyskip();
	yych = yypeek();;
	switch (yych) {
		case '\t':
		case '\n':
		case '\v':
		case '\f':
		case '\r':
		case ' ': goto yy4;
		default: goto yy5;
	}
yy5:
	{ goto start; }
yy6:
	yyskip();
	{ Token token{Token::Type::String, "", tx, ty};
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
yy7:
	yyskip();
	yybackup();
	yych = yypeek();;
	if (yych >= 0x01) goto yy13;
yy8:
	{ lastError = "unknown char " + std::to_string(yych) + getPosStr(); 
            return Token{Token::Type::Finished, "", tx, ty}; 
            YYCURSOR = YYLIMIT;
          }
yy9:
	yyskip();
	{ goto start; }
yy10:
	yyskip();
	{ return Token{Token::Type::BraceLeft, std::string{t, (size_t)(YYCURSOR - t)}, tx, ty};}
yy11:
	yyskip();
	{ return Token{Token::Type::BraceRight, std::string{t, (size_t)(YYCURSOR - t)}, tx, ty};}
yy12:
	yyskip();
	yych = yypeek();;
yy13:
	switch (yych) {
		case 0x00: goto yy14;
		case '\n': goto yy15;
		default: goto yy12;
	}
yy14:
	yyrestore();
	goto yy8;
yy15:
	yyskip();
	{ goto start; }
}
}

class IniParser{
public:
  struct IniConfig{
    struct Section{
      struct KeyAndValue{
        std::string key;
        std::string val;
      };
      using KVArray = std::vector<KeyAndValue>;
      std::string name;
      KVArray kvs;
    };
    using SectionArray = std::vector<Section>;
    SectionArray sections;
  };

  struct ParserResult{
    IniConfig iniConfig;
    std::string error;
  };

  static ParserResult parse(const char* data, size_t size);
};


using Token = Lexer::Token;

IniParser::ParserResult IniParser::parse(const char* data, size_t size){
  Lexer lexer(data, size);

  ParserResult result;
  auto token = lexer.lex();

  auto parseKeyAndValue = [&](IniConfig::Section::KVArray& kvArray){
    token = lexer.lex();
    IniConfig::Section::KeyAndValue kv;
    while(token.type != Token::Type::Finished){
      if(token.type == Token::Type::Identifier){
        kv.key = token.text;
        token = lexer.lex();
        if(token.type == Token::Type::Identifier || token.type == Token::Type::String){
          kv.val = token.text;
          kvArray.push_back(kv);
          token = lexer.lex();
        }else{
          result.error = "key(" + kv.key + ") need a value" + "(Error at " + token.getPosText() + ")"; 
          return false;
        }
      }else{
        break;
      }
    }
    return true;
  };

  auto parseSection = [&](IniConfig::Section& section){
    token = lexer.lex();
    if(token.type == Token::Type::Identifier){
      section.name = token.text;
      token = lexer.lex();
      if(token.type != Token::Type::BraceRight){
        result.error = "section name(" + section.name + ") is not closed" + "(Error at " + token.getPosText() + ")"; 
        return false;
      }
      return parseKeyAndValue(section.kvs);
    }else{
      result.error = "section name is invalid(Error at " + token.getPosText() + ")";
      return false;
    }
  };

  while(token.type != Token::Type::Finished){
    if(token.type == Token::Type::BraceLeft){
      IniConfig::Section section;
      if(!parseSection(section)){
        if(result.error.empty()) result.error = "parsing failed.";
        break;
      }else{
        result.iniConfig.sections.push_back(section);
      }
    }else if(token.type != Token::Type::Finished){
      result.error= "unknown token " + token.text + "(Error at " + token.getPosText() + ")";
      break;
    }
  }

  if(lexer.hasError()){
    if(result.error.empty()){
      result.error = lexer.getLastError();
    }else{
      result.error += "\n" + lexer.getLastError();
    }
  }

  return result;
}