#include <filesystem>
#include <fstream>
#include<iostream>

#include "tokenizer.h"
#include "parser.h"
int main() {
    std::ifstream file("../../testcases/type/type1.txt");
    if (!file.is_open()) {
        std::cerr<<"unable to open the file"<<std::endl;
        return -1;
    }
    std::string input((std::istreambuf_iterator(file)),std::istreambuf_iterator<char>());
    try {
        Tokenizer t(input);
        auto v=t.tokenize();
        //t.ShowOutput(v);
        Parser p(v);
        auto root=p.parse();
        if (!root) {
            std::cerr<<"unable to parse the file"<<std::endl;
        }
        std::vector<std::string> tree = root->showTree(0,true);

        // 逐行打印
        for (const std::string& line : tree) {
            std::cout << line << std::endl;
        }
    }catch (...) {
        exit(-1);
    }
    return 0;
}
