#include <iostream>
#include "IniParser.h"

int main(){
  std::string str = R"(;commmment line 1.
    ;commmment line 2.
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