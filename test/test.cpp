//
// Created by ckjsuperhh6602 on 25-9-25.
//
#include <fstream>

#include "../parser.h"
#include "../semantic_check.h"
#include<iostream>
#include <gtest/gtest.h>

std::string openFile(std::string path)
{
    path="../../test/testcases/"+path;
    // std::ifstream file(path);
    // std::string input((std::istreambuf_iterator(file)),std::istreambuf_iterator<char>());

    freopen(path.c_str(),"r",stdin);
    int in;
    std::string code;
    while((in=std::cin.get())!=EOF)
        code.push_back(static_cast<char>(in));
    fclose(stdin);
    std::cin.clear();
    // code.push_back('\n');
    return code;
}

void runParser(std::string path)
{
    const auto code=openFile(path);
    Tokenizer t(code);
    const auto v=t.tokenize();
    // t.ShowOutput(v);
    Parser p(v);
    auto root=p.parse();
}

// void runSemantic(std::string path)
// {
//     const auto code=openFile(path);
//     semanticCheckType(parser(code).solve());
// }


TEST(Parser, TYPE1){EXPECT_NO_THROW(runParser("type1.txt"));}
