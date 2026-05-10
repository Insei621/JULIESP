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
        src/DebugPrinter.cpp
        include/DebugPrinter.h
        src/CompilerConfig.cpp
        include/CompilerConfig.h
)

# =============================================================================
# Includes
# =============================================================================

target_include_directories(juliesp PRIVATE include)

# =============================================================================
# Installation
# =============================================================================

install(TARGETS juliesp
    DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)

install(FILES juliesp_runtime.h
    DESTINATION ${CMAKE_INSTALL_PREFIX}/include)

install(FILES juliesp.1
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/man/man1)
