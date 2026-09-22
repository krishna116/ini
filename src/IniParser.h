#pragma once

#include <string>
#include <vector>
#include <cstddef>
#include <cstdint>

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
