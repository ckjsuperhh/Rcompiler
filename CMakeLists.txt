cmake_minimum_required(VERSION 3.28)
project(Compiler)

set(CMAKE_CXX_STANDARD 20)

add_executable(Compiler main.cpp
        tokenizer.h
        AST_node.h
        "semantic check.h"
        parser.h)
