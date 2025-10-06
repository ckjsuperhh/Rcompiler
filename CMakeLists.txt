cmake_minimum_required(VERSION 3.28)
project(Compiler)

set(CMAKE_CXX_STANDARD 20)

enable_testing()
#add_subdirectory(test/my-testcases)
add_subdirectory(test/wxzheng)

add_executable(Compiler main.cpp
        tokenizer.h
        AST_node.h
        semantic_check.h
        parser.h
        AST_node.cpp
        semantic_check.cpp
)
