cmake_minimum_required(VERSION 3.16)
project(juliesp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =============================================================================
# Sources
# =============================================================================

add_executable(juliesp
    src/main.cpp
    src/Lexer.cpp
    src/Parser.cpp
    src/Parser_SpecialForms.cpp
    src/parseArithmetic.cpp
    src/parsePrimitiveCall.cpp
    src/AST.cpp
    src/Visitor.cpp
    src/PrettyPrinter.cpp
    src/GraphvizVisitor.cpp
    src/SymbolTable.cpp
    src/SemanticAnalyzer.cpp
    src/IRGenerator.cpp
    src/CGenerator.cpp
)

# =============================================================================
# Includes
# =============================================================================

target_include_directories(juliesp PRIVATE include)

# =============================================================================
# Installation : `sudo make install` copie juliesp dans /usr/local/bin
# et juliesp_runtime.h dans /usr/local/include
# =============================================================================

install(TARGETS juliesp DESTINATION /usr/local/bin)
install(FILES juliesp_runtime.h DESTINATION /usr/local/include)
install(FILES juliesp.1 DESTINATION /usr/share/man/man1)
