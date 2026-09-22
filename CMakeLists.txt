cmake_minimum_required(VERSION 4.0)

project(IniParser VERSION 1.0.0 LANGUAGES CXX)

add_library(iniLib
src/IniParser.cpp
src/Lexer.cpp
)

target_include_directories(iniLib PUBLIC
src
)

find_program(RE2C_EXECUTABLE
NAMES re2c re2c.exe
)

if(RE2C_EXECUTABLE)
  message("find re2c = ${RE2C_EXECUTABLE}")
  add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/src/Lexer.cpp
    COMMAND ${RE2C_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Lexer.l" -o "${CMAKE_CURRENT_SOURCE_DIR}/src/Lexer.cpp"
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Lexer.l"
    COMMENT "Generating Lexer.cpp"
    VERBATIM
  )
  add_custom_target(
    generate_ini_lexer
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/Lexer.cpp"
  )
  add_dependencies(iniLib generate_ini_lexer)
endif()

add_subdirectory(tests)
