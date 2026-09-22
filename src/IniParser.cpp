#include "IniParser.h"
#include "Lexer.h"

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