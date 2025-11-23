#include <fstream>

#include "../../parser.h"
#include "../../semantic_check.h"
#include<iostream>
#include <gtest/gtest.h>

std::string openFile(std::string path)
{
    path="../../../test/my-testcases/testcases/"+path;
    freopen(path.c_str(),"r",stdin);
    int in;
    std::string code;
    while((in=std::cin.get())!=EOF)
        code.push_back(static_cast<char>(in));
    fclose(stdin);
    std::cin.clear();
    code.push_back('\n');
    return code;
}

void runSemantic(std::string path)
{
    const auto code=openFile(path);
    Tokenizer t(code);
    auto v=t.tokenize();
    Parser p(v);
    auto root=p.parse();
    SemanticCheck sc;
    sc.Analyze(root.get());
}


TEST(semantic3,type1) {
    EXPECT_ANY_THROW(runSemantic("type1.txt"));
}

TEST(semantic3,type2) {
    EXPECT_ANY_THROW(runSemantic("type2.txt"));
}