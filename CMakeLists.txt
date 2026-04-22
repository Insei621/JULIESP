cmake_minimum_required(VERSION 3.28)
project(Compilateur_JULIESP)

set(CMAKE_CXX_STANDARD 20)

add_executable(Compilateur_JULIESP src/main.cpp
        src/Lexer.cpp
        include/Lexer.h
        include/pch.h
        src/Parser.cpp
        include/Parser.h
        include/AST.h
        src/Parser_SpecialForms.cpp
        src/Visitor.cpp
        include/Visitor.h
        src/PrettyPrinter.cpp
        include/PrettyPrinter.h
        src/AST.cpp
        src/parseArithmetic.cpp
        src/parsePrimitiveCall.cpp
        src/GraphvizVisitor.cpp
        include/GraphvizVisitor.h
        src/SymbolTable.cpp
        include/SymbolTable.h
        src/SemanticAnalyzer.cpp
        include/SemanticAnalyzer.h
        include/IR.h
        include/IRGenerator.h
        src/IRGenerator.cpp
        include/CGenerator.h
        src/CGenerator.cpp
)
