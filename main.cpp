#include <filesystem>
#include <fstream>
#include<iostream>

#include "tokenizer.h"
#include "parser.h"
int main() {
    std::ifstream file("my.in");
    if (!file.is_open()) {
        std::cerr<<"unable to open the file"<<std::endl;
        return -1;
    }
    std::string input((std::istreambuf_iterator(file)),std::istreambuf_iterator<char>());
    // std::cerr<<input<<std::endl;
    try {
        Tokenizer t(input);
        auto v=t.tokenize();
        // t.ShowOutput(v);
        Parser p(v);
        auto root=p.parse();
        if (!root) {
            std::cerr<<"unable to parse the file"<<std::endl;
        }
        std::vector<std::string> tree1 = root->showTree(0,true);
        // 逐行打印
        for (const std::string& line : tree1) {
            std::cout << line << std::endl;
        }
        SemanticCheck sc;
        sc.Analyze(root.get());
        std::vector<std::string> tree2 = root->showTree(0,true);
        // 逐行打印
        for (const std::string& line : tree2) {
            std::cout << line << std::endl;
        }
        IRgen ir;
        for (auto ve=ir.generateIr(root.get()); const auto& vv:ve) {
            std::cout << vv << std::endl;
        }
    }catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        exit(-1);
    }catch (...) {  // 再捕获所有其他异常
        std::cerr << "Unknown error occurred" << std::endl;
        exit(-1);
    }
    return 0;
}
