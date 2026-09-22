# ini file parser library

A classic ini file parser written in C++.

## Example
```C++
#include <iostream>
#include "IniParser.h"

int main(){
  std::string str = R"(;comment line 1.
    ;comment line 2.
    [section1]
      key1 = value1;
      key2 = value2;
      key3 = "value3";
    [section2]
      key5 = "c:/file/file.h";
      key6 = "d:\\file\\file.cpp";
  )";

  auto result = IniParser::parse(str.data(), str.size());
  for(auto& section: result.iniConfig.sections){
    std::cout << "[" <<section.name << "]" << std::endl;
    for(auto& kv: section.kvs){
      std::cout <<"  "<< kv.key << " = {" << kv.val << "}" << std::endl;
    }
  }

  if(!result.error.empty()){
    std::cout << result.error <<std::endl;
  }

  return  result.error.empty()? 0 : 1;
}
```

Output:
```
[section1]
  key1 = {value1}
  key2 = {value2}
  key3 = {value3}
[section2]
  key5 = {c:/file/file.h}
  key6 = {d:\file\file.cpp}
```

## Ini file format
```
ini           ::= {comment}* {section}*
section       ::= "[" identifier "]" {key-and-value}*
key-and-value ::= identifier "=" (identifier | string)

comment       ::= [;] [^\n\x00]* [\r]? [\n];
identifier    ::= [^ "=;[\]\t\v\r\n\f\x00]+;
string        ::= ["] [^"\n\r\x00]* ["];
spaces        ::= [ \t\v\f\r\n]+;

# Note
The string supports escape "back slash" for example: "\\" => "\", "\n" => "n".
```
