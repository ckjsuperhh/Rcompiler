#include <filesystem>
#include <fstream>
#include<iostream>

#include "tokenizer.h"
#include "parser.h"
int main() {
    std::ifstream file("../testcases/type/type1.txt");
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
    }catch (...) {
        exit(-1);
    }
    return 0;
}
