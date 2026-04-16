cmake_minimum_required(VERSION 3.28)
project(Compilateur_JULIESP)

set(CMAKE_CXX_STANDARD 20)

add_executable(Compilateur_JULIESP main.cpp
        Lexer.cpp
        Lexer.h
        pch.h
        Parser.cpp
        Parser.h
        AST.h
        Parser_SpecialForms.cpp
)
