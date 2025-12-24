#include <fstream>

#include "../../parser.h"
#include "../../semantic_check.h"
#include "../../ir.h"
#include<iostream>
#include <gtest/gtest.h>

std::string openFile(std::string path)
{
    path="../../../test/wxzheng/testcases/"+path;
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

void runIr(std::string path) {
    const auto code=openFile(path);
    auto las=std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now()).time_since_epoch().count();
    Tokenizer to(code);
    auto v=to.tokenize();
    Parser p(v);
    auto root=p.parse();
    SemanticCheck sc;
    sc.Analyze(root.get());
    std::fstream fs("my.ll", std::ios::out);
    IRgen ir;
    auto irCode=ir.generateIr(root.get());
    for (const auto& t:irCode) {
        // std::cout << t << std::endl;  // 控制台打印IR
        fs << t << std::endl;        // 写入文件
    }
    auto now=std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now()).time_since_epoch().count();
    std::cerr<<"my time: "<<now-las<<" ms"<<std::endl;
    las=now;
    auto cl=system("clang --target=x86_64-unknown-linux-gnu -march=x86-64 -O0 my.ll -o my -O2");
    now=std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now()).time_since_epoch().count();
    std::cerr<<"clang time: "<<now-las<<" ms"<<std::endl;
    if (cl)
        throw std::runtime_error("clang timeout error");
    las=now;
    cl=system(("./my > my.out < ../../../test/wxzheng/testcases/"+path.substr(0,path.size()-3)+".in").c_str());
    now=std::chrono::time_point_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now()).time_since_epoch().count();
    std::cerr<<"run time: "<<now-las<<" ms"<<std::endl;
    if (WIFSIGNALED(cl))
    {
        std::cerr<<"Runtime Error"<<std::endl;
        throw std::runtime_error("Runtime error");
    }
    cl=system(("diff my.out ../../../test/wxzheng/testcases/"+path.substr(0,path.size()-3)+".out -w").c_str());
    if (cl)
    {
        std::cerr<<"Wrong Answer"<<std::endl;
        throw std::runtime_error("Wrong answer");
    }
}

TEST(semantic2, comprehensive1) {
    EXPECT_NO_THROW(runIr("comprehensive1.rx"));
}
TEST(semantic2, comprehensive10) {
    EXPECT_NO_THROW(runIr("comprehensive10.rx"));
}
TEST(semantic2, comprehensive11) {
    EXPECT_NO_THROW(runIr("comprehensive11.rx"));
}
TEST(semantic2, comprehensive12) {
    EXPECT_NO_THROW(runIr("comprehensive12.rx"));
}
TEST(semantic2, comprehensive13) {
    EXPECT_NO_THROW(runIr("comprehensive13.rx"));
}
TEST(semantic2, comprehensive14) {
    EXPECT_NO_THROW(runIr("comprehensive14.rx"));
}
TEST(semantic2, comprehensive15) {
    EXPECT_NO_THROW(runIr("comprehensive15.rx"));
}
TEST(semantic2, comprehensive16) {
    EXPECT_NO_THROW(runIr("comprehensive16.rx"));
}
TEST(semantic2, comprehensive17) {
    EXPECT_NO_THROW(runIr("comprehensive17.rx"));
}
TEST(semantic2, comprehensive18) {
    EXPECT_NO_THROW(runIr("comprehensive18.rx"));
}
TEST(semantic2, comprehensive19) {
    EXPECT_NO_THROW(runIr("comprehensive19.rx"));
}
TEST(semantic2, comprehensive2) {
    EXPECT_NO_THROW(runIr("comprehensive2.rx"));
}
TEST(semantic2, comprehensive20) {
    EXPECT_NO_THROW(runIr("comprehensive20.rx"));
}
TEST(semantic2, comprehensive21) {
    EXPECT_NO_THROW(runIr("comprehensive21.rx"));
}
TEST(semantic2, comprehensive22) {
    EXPECT_NO_THROW(runIr("comprehensive22.rx"));
}
TEST(semantic2, comprehensive23) {
    EXPECT_NO_THROW(runIr("comprehensive23.rx"));
}
TEST(semantic2, comprehensive24) {
    EXPECT_NO_THROW(runIr("comprehensive24.rx"));
}
TEST(semantic2, comprehensive25) {
    EXPECT_NO_THROW(runIr("comprehensive25.rx"));
}
TEST(semantic2, comprehensive26) {
    EXPECT_NO_THROW(runIr("comprehensive26.rx"));
}
TEST(semantic2, comprehensive27) {
    EXPECT_NO_THROW(runIr("comprehensive27.rx"));
}
TEST(semantic2, comprehensive28) {
    EXPECT_NO_THROW(runIr("comprehensive28.rx"));
}
TEST(semantic2, comprehensive29) {
    EXPECT_NO_THROW(runIr("comprehensive29.rx"));
}
TEST(semantic2, comprehensive3) {
    EXPECT_NO_THROW(runIr("comprehensive3.rx"));
}
TEST(semantic2, comprehensive30) {
    EXPECT_NO_THROW(runIr("comprehensive30.rx"));
}
TEST(semantic2, comprehensive31) {
    EXPECT_NO_THROW(runIr("comprehensive31.rx"));
}
TEST(semantic2, comprehensive32) {
    EXPECT_NO_THROW(runIr("comprehensive32.rx"));
}
TEST(semantic2, comprehensive33) {
    EXPECT_NO_THROW(runIr("comprehensive33.rx"));
}
TEST(semantic2, comprehensive34) {
    EXPECT_NO_THROW(runIr("comprehensive34.rx"));
}
TEST(semantic2, comprehensive35) {
    EXPECT_NO_THROW(runIr("comprehensive35.rx"));
}
TEST(semantic2, comprehensive36) {
    EXPECT_NO_THROW(runIr("comprehensive36.rx"));
}
TEST(semantic2, comprehensive37) {
    EXPECT_NO_THROW(runIr("comprehensive37.rx"));
}
TEST(semantic2, comprehensive38) {
    EXPECT_NO_THROW(runIr("comprehensive38.rx"));
}
TEST(semantic2, comprehensive39) {
    EXPECT_NO_THROW(runIr("comprehensive39.rx"));
}
TEST(semantic2, comprehensive4) {
    EXPECT_NO_THROW(runIr("comprehensive4.rx"));
}
TEST(semantic2, comprehensive40) {
    EXPECT_NO_THROW(runIr("comprehensive40.rx"));
}
TEST(semantic2, comprehensive41) {
    EXPECT_NO_THROW(runIr("comprehensive41.rx"));
}
TEST(semantic2, comprehensive42) {
    EXPECT_NO_THROW(runIr("comprehensive42.rx"));
}
TEST(semantic2, comprehensive43) {
    EXPECT_NO_THROW(runIr("comprehensive43.rx"));
}
TEST(semantic2, comprehensive44) {
    EXPECT_NO_THROW(runIr("comprehensive44.rx"));
}
TEST(semantic2, comprehensive45) {
    EXPECT_NO_THROW(runIr("comprehensive45.rx"));
}
TEST(semantic2, comprehensive46) {
    EXPECT_NO_THROW(runIr("comprehensive46.rx"));
}
TEST(semantic2, comprehensive47) {
    EXPECT_NO_THROW(runIr("comprehensive47.rx"));
}
TEST(semantic2, comprehensive48) {
    EXPECT_NO_THROW(runIr("comprehensive48.rx"));
}
TEST(semantic2, comprehensive49) {
    EXPECT_NO_THROW(runIr("comprehensive49.rx"));
}
TEST(semantic2, comprehensive5) {
    EXPECT_NO_THROW(runIr("comprehensive5.rx"));
}
TEST(semantic2, comprehensive50) {
    EXPECT_NO_THROW(runIr("comprehensive50.rx"));
}
TEST(semantic2, comprehensive6) {
    EXPECT_NO_THROW(runIr("comprehensive6.rx"));
}
TEST(semantic2, comprehensive7) {
    EXPECT_NO_THROW(runIr("comprehensive7.rx"));
}
TEST(semantic2, comprehensive8) {
    EXPECT_NO_THROW(runIr("comprehensive8.rx"));
}
TEST(semantic2, comprehensive9) {
    EXPECT_NO_THROW(runIr("comprehensive9.rx"));
}
TEST(semantic1, array1) {
    EXPECT_NO_THROW(runSemantic("array1.rx"));
}
TEST(semantic1, array2) {
    EXPECT_NO_THROW(runSemantic("array2.rx"));
}
TEST(semantic1, array3) {
    EXPECT_NO_THROW(runSemantic("array3.rx"));
}
TEST(semantic1, array4) {
    EXPECT_ANY_THROW(runSemantic("array4.rx"));
}
TEST(semantic1, array5) {
    EXPECT_ANY_THROW(runSemantic("array5.rx"));
}
TEST(semantic1, array6) {
    EXPECT_ANY_THROW(runSemantic("array6.rx"));
}
TEST(semantic1, array7) {
    EXPECT_ANY_THROW(runSemantic("array7.rx"));
}
TEST(semantic1, array8) {
    EXPECT_ANY_THROW(runSemantic("array8.rx"));
}
TEST(semantic1, autoref1) {
    EXPECT_NO_THROW(runSemantic("autoref1.rx"));
}
TEST(semantic1, autoref2) {
    EXPECT_NO_THROW(runSemantic("autoref2.rx"));
}
TEST(semantic1, autoref3) {
    EXPECT_NO_THROW(runSemantic("autoref3.rx"));
}
TEST(semantic1, autoref4) {
    EXPECT_NO_THROW(runSemantic("autoref4.rx"));
}
TEST(semantic1, autoref5) {
    EXPECT_NO_THROW(runSemantic("autoref5.rx"));
}
TEST(semantic1, autoref6) {
    EXPECT_NO_THROW(runSemantic("autoref6.rx"));
}
TEST(semantic1, autoref7) {
    EXPECT_ANY_THROW(runSemantic("autoref7.rx"));
}
TEST(semantic1, autoref8) {
    EXPECT_ANY_THROW(runSemantic("autoref8.rx"));
}
TEST(semantic1, autoref9) {
    EXPECT_NO_THROW(runSemantic("autoref9.rx"));
}
TEST(semantic1, basic1) {
    EXPECT_NO_THROW(runSemantic("basic1.rx"));
}
TEST(semantic1, basic10) {
    EXPECT_NO_THROW(runSemantic("basic10.rx"));
}
TEST(semantic1, basic11) {
    EXPECT_NO_THROW(runSemantic("basic11.rx"));
}
TEST(semantic1, basic12) {
    EXPECT_ANY_THROW(runSemantic("basic12.rx"));
}
TEST(semantic1, basic13) {
    EXPECT_ANY_THROW(runSemantic("basic13.rx"));
}
TEST(semantic1, basic14) {
    EXPECT_ANY_THROW(runSemantic("basic14.rx"));
}
TEST(semantic1, basic15) {
    EXPECT_ANY_THROW(runSemantic("basic15.rx"));
}
TEST(semantic1, basic16) {
    EXPECT_ANY_THROW(runSemantic("basic16.rx"));
}
TEST(semantic1, basic17) {
    EXPECT_NO_THROW(runSemantic("basic17.rx"));
}
TEST(semantic1, basic18) {
    EXPECT_NO_THROW(runSemantic("basic18.rx"));
}
TEST(semantic1, basic19) {
    EXPECT_NO_THROW(runSemantic("basic19.rx"));
}
TEST(semantic1, basic2) {
    EXPECT_ANY_THROW(runSemantic("basic2.rx"));
}
TEST(semantic1, basic20) {
    EXPECT_NO_THROW(runSemantic("basic20.rx"));
}
TEST(semantic1, basic21) {
    EXPECT_NO_THROW(runSemantic("basic21.rx"));
}
TEST(semantic1, basic22) {
    EXPECT_NO_THROW(runSemantic("basic22.rx"));
}
TEST(semantic1, basic23) {
    EXPECT_NO_THROW(runSemantic("basic23.rx"));
}
TEST(semantic1, basic24) {
    EXPECT_NO_THROW(runSemantic("basic24.rx"));
}
TEST(semantic1, basic25) {
    EXPECT_NO_THROW(runSemantic("basic25.rx"));
}
TEST(semantic1, basic26) {
    EXPECT_NO_THROW(runSemantic("basic26.rx"));
}
TEST(semantic1, basic27) {
    EXPECT_NO_THROW(runSemantic("basic27.rx"));
}
TEST(semantic1, basic28) {
    EXPECT_ANY_THROW(runSemantic("basic28.rx"));
}
TEST(semantic1, basic29) {
    EXPECT_NO_THROW(runSemantic("basic29.rx"));
}
TEST(semantic1, basic3) {
    EXPECT_ANY_THROW(runSemantic("basic3.rx"));
}
TEST(semantic1, basic30) {
    EXPECT_ANY_THROW(runSemantic("basic30.rx"));
}
TEST(semantic1, basic31) {
    EXPECT_ANY_THROW(runSemantic("basic31.rx"));
}
TEST(semantic1, basic32) {
    EXPECT_ANY_THROW(runSemantic("basic32.rx"));
}
TEST(semantic1, basic33) {
    EXPECT_ANY_THROW(runSemantic("basic33.rx"));
}
TEST(semantic1, basic34) {
    EXPECT_ANY_THROW(runSemantic("basic34.rx"));
}
TEST(semantic1, basic35) {
    EXPECT_ANY_THROW(runSemantic("basic35.rx"));
}
TEST(semantic1, basic36) {
    EXPECT_NO_THROW(runSemantic("basic36.rx"));
}
TEST(semantic1, basic37) {
    EXPECT_NO_THROW(runSemantic("basic37.rx"));
}
TEST(semantic1, basic38) {
    EXPECT_ANY_THROW(runSemantic("basic38.rx"));
}
TEST(semantic1, basic39) {
    EXPECT_NO_THROW(runSemantic("basic39.rx"));
}
TEST(semantic1, basic4) {
    EXPECT_NO_THROW(runSemantic("basic4.rx"));
}
TEST(semantic1, basic40) {
    EXPECT_NO_THROW(runSemantic("basic40.rx"));
}
TEST(semantic1, basic5) {
    EXPECT_NO_THROW(runSemantic("basic5.rx"));
}
TEST(semantic1, basic6) {
    EXPECT_ANY_THROW(runSemantic("basic6.rx"));
}
TEST(semantic1, basic7) {
    EXPECT_ANY_THROW(runSemantic("basic7.rx"));
}
TEST(semantic1, basic8) {
    EXPECT_ANY_THROW(runSemantic("basic8.rx"));
}
TEST(semantic1, basic9) {
    EXPECT_ANY_THROW(runSemantic("basic9.rx"));
}
TEST(semantic1, expr1) {
    EXPECT_ANY_THROW(runSemantic("expr1.rx"));
}
TEST(semantic1, expr10) {
    EXPECT_ANY_THROW(runSemantic("expr10.rx"));
}
TEST(semantic1, expr11) {
    EXPECT_ANY_THROW(runSemantic("expr11.rx"));
}
TEST(semantic1, expr12) {
    EXPECT_ANY_THROW(runSemantic("expr12.rx"));
}
TEST(semantic1, expr13) {
    EXPECT_NO_THROW(runSemantic("expr13.rx"));
}
TEST(semantic1, expr14) {
    EXPECT_ANY_THROW(runSemantic("expr14.rx"));
}
TEST(semantic1, expr15) {
    EXPECT_ANY_THROW(runSemantic("expr15.rx"));
}
TEST(semantic1, expr16) {
    EXPECT_ANY_THROW(runSemantic("expr16.rx"));
}
TEST(semantic1, expr17) {
    EXPECT_NO_THROW(runSemantic("expr17.rx"));
}
TEST(semantic1, expr18) {
    EXPECT_ANY_THROW(runSemantic("expr18.rx"));
}
TEST(semantic1, expr19) {
    EXPECT_NO_THROW(runSemantic("expr19.rx"));
}
TEST(semantic1, expr2) {
    EXPECT_ANY_THROW(runSemantic("expr2.rx"));
}
TEST(semantic1, expr20) {
    EXPECT_ANY_THROW(runSemantic("expr20.rx"));
}
TEST(semantic1, expr21) {
    EXPECT_ANY_THROW(runSemantic("expr21.rx"));
}
TEST(semantic1, expr22) {
    EXPECT_ANY_THROW(runSemantic("expr22.rx"));
}
TEST(semantic1, expr23) {
    EXPECT_ANY_THROW(runSemantic("expr23.rx"));
}
TEST(semantic1, expr24) {
    EXPECT_ANY_THROW(runSemantic("expr24.rx"));
}
TEST(semantic1, expr25) {
    EXPECT_ANY_THROW(runSemantic("expr25.rx"));
}
TEST(semantic1, expr26) {
    EXPECT_ANY_THROW(runSemantic("expr26.rx"));
}
TEST(semantic1, expr27) {
    EXPECT_ANY_THROW(runSemantic("expr27.rx"));
}
TEST(semantic1, expr28) {
    EXPECT_ANY_THROW(runSemantic("expr28.rx"));
}
TEST(semantic1, expr29) {
    EXPECT_ANY_THROW(runSemantic("expr29.rx"));
}
TEST(semantic1, expr3) {
    EXPECT_ANY_THROW(runSemantic("expr3.rx"));
}
TEST(semantic1, expr30) {
    EXPECT_ANY_THROW(runSemantic("expr30.rx"));
}
TEST(semantic1, expr31) {
    EXPECT_ANY_THROW(runSemantic("expr31.rx"));
}
TEST(semantic1, expr32) {
    EXPECT_ANY_THROW(runSemantic("expr32.rx"));
}
TEST(semantic1, expr33) {
    EXPECT_NO_THROW(runSemantic("expr33.rx"));
}
TEST(semantic1, expr34) {
    EXPECT_NO_THROW(runSemantic("expr34.rx"));
}
TEST(semantic1, expr35) {
    EXPECT_ANY_THROW(runSemantic("expr35.rx"));
}
TEST(semantic1, expr36) {
    EXPECT_NO_THROW(runSemantic("expr36.rx"));
}
TEST(semantic1, expr37) {
    EXPECT_ANY_THROW(runSemantic("expr37.rx"));
}
TEST(semantic1, expr38) {
    EXPECT_NO_THROW(runSemantic("expr38.rx"));
}
TEST(semantic1, expr39) {
    EXPECT_ANY_THROW(runSemantic("expr39.rx"));
}
TEST(semantic1, expr4) {
    EXPECT_ANY_THROW(runSemantic("expr4.rx"));
}
TEST(semantic1, expr40) {
    EXPECT_ANY_THROW(runSemantic("expr40.rx"));
}
TEST(semantic1, expr5) {
    EXPECT_ANY_THROW(runSemantic("expr5.rx"));
}
TEST(semantic1, expr6) {
    EXPECT_ANY_THROW(runSemantic("expr6.rx"));
}
TEST(semantic1, expr7) {
    EXPECT_ANY_THROW(runSemantic("expr7.rx"));
}
TEST(semantic1, expr8) {
    EXPECT_ANY_THROW(runSemantic("expr8.rx"));
}
TEST(semantic1, expr9) {
    EXPECT_ANY_THROW(runSemantic("expr9.rx"));
}
TEST(semantic1, if1) {
    EXPECT_NO_THROW(runSemantic("if1.rx"));
}
TEST(semantic1, if10) {
    EXPECT_NO_THROW(runSemantic("if10.rx"));
}
TEST(semantic1, if11) {
    EXPECT_ANY_THROW(runSemantic("if11.rx"));
}
TEST(semantic1, if12) {
    EXPECT_ANY_THROW(runSemantic("if12.rx"));
}
TEST(semantic1, if13) {
    EXPECT_ANY_THROW(runSemantic("if13.rx"));
}
TEST(semantic1, if14) {
    EXPECT_ANY_THROW(runSemantic("if14.rx"));
}
TEST(semantic1, if15) {
    EXPECT_ANY_THROW(runSemantic("if15.rx"));
}
TEST(semantic1, if2) {
    EXPECT_NO_THROW(runSemantic("if2.rx"));
}
TEST(semantic1, if3) {
    EXPECT_NO_THROW(runSemantic("if3.rx"));
}
TEST(semantic1, if4) {
    EXPECT_NO_THROW(runSemantic("if4.rx"));
}
TEST(semantic1, if5) {
    EXPECT_NO_THROW(runSemantic("if5.rx"));
}
TEST(semantic1, if6) {
    EXPECT_NO_THROW(runSemantic("if6.rx"));
}
TEST(semantic1, if7) {
    EXPECT_NO_THROW(runSemantic("if7.rx"));
}
TEST(semantic1, if8) {
    EXPECT_NO_THROW(runSemantic("if8.rx"));
}
TEST(semantic1, if9) {
    EXPECT_NO_THROW(runSemantic("if9.rx"));
}
TEST(semantic1, loop1) {
    EXPECT_NO_THROW(runSemantic("loop1.rx"));
}
TEST(semantic1, loop10) {
    EXPECT_ANY_THROW(runSemantic("loop10.rx"));
}
TEST(semantic1, loop2) {
    EXPECT_NO_THROW(runSemantic("loop2.rx"));
}
TEST(semantic1, loop3) {
    EXPECT_NO_THROW(runSemantic("loop3.rx"));
}
TEST(semantic1, loop4) {
    EXPECT_NO_THROW(runSemantic("loop4.rx"));
}
TEST(semantic1, loop5) {
    EXPECT_NO_THROW(runSemantic("loop5.rx"));
}
TEST(semantic1, loop6) {
    EXPECT_ANY_THROW(runSemantic("loop6.rx"));
}
TEST(semantic1, loop7) {
    EXPECT_ANY_THROW(runSemantic("loop7.rx"));
}
TEST(semantic1, loop8) {
    EXPECT_ANY_THROW(runSemantic("loop8.rx"));
}
TEST(semantic1, loop9) {
    EXPECT_ANY_THROW(runSemantic("loop9.rx"));
}
TEST(semantic1, misc1) {
    EXPECT_NO_THROW(runSemantic("misc1.rx"));
}
TEST(semantic1, misc10) {
    EXPECT_NO_THROW(runSemantic("misc10.rx"));
}
TEST(semantic1, misc11) {
    EXPECT_NO_THROW(runSemantic("misc11.rx"));
}
TEST(semantic1, misc12) {
    EXPECT_NO_THROW(runSemantic("misc12.rx"));
}
TEST(semantic1, misc13) {
    EXPECT_NO_THROW(runSemantic("misc13.rx"));
}
TEST(semantic1, misc14) {
    EXPECT_NO_THROW(runSemantic("misc14.rx"));
}
TEST(semantic1, misc15) {
    EXPECT_NO_THROW(runSemantic("misc15.rx"));
}
TEST(semantic1, misc16) {
    EXPECT_NO_THROW(runSemantic("misc16.rx"));
}
TEST(semantic1, misc17) {
    EXPECT_NO_THROW(runSemantic("misc17.rx"));
}
TEST(semantic1, misc18) {
    EXPECT_NO_THROW(runSemantic("misc18.rx"));
}
TEST(semantic1, misc19) {
    EXPECT_NO_THROW(runSemantic("misc19.rx"));
}
TEST(semantic1, misc2) {
    EXPECT_NO_THROW(runSemantic("misc2.rx"));
}
TEST(semantic1, misc20) {
    EXPECT_NO_THROW(runSemantic("misc20.rx"));
}
TEST(semantic1, misc21) {
    EXPECT_NO_THROW(runSemantic("misc21.rx"));
}
TEST(semantic1, misc22) {
    EXPECT_NO_THROW(runSemantic("misc22.rx"));
}
TEST(semantic1, misc23) {
    EXPECT_NO_THROW(runSemantic("misc23.rx"));
}
TEST(semantic1, misc24) {
    EXPECT_NO_THROW(runSemantic("misc24.rx"));
}
TEST(semantic1, misc25) {
    EXPECT_NO_THROW(runSemantic("misc25.rx"));
}
TEST(semantic1, misc26) {
    EXPECT_NO_THROW(runSemantic("misc26.rx"));
}
TEST(semantic1, misc27) {
    EXPECT_NO_THROW(runSemantic("misc27.rx"));
}
TEST(semantic1, misc28) {
    EXPECT_NO_THROW(runSemantic("misc28.rx"));
}
TEST(semantic1, misc29) {
    EXPECT_NO_THROW(runSemantic("misc29.rx"));
}
TEST(semantic1, misc3) {
    EXPECT_NO_THROW(runSemantic("misc3.rx"));
}
TEST(semantic1, misc30) {
    EXPECT_NO_THROW(runSemantic("misc30.rx"));
}
TEST(semantic1, misc31) {
    EXPECT_NO_THROW(runSemantic("misc31.rx"));
}
TEST(semantic1, misc32) {
    EXPECT_NO_THROW(runSemantic("misc32.rx"));
}
TEST(semantic1, misc33) {
    EXPECT_NO_THROW(runSemantic("misc33.rx"));
}
TEST(semantic1, misc34) {
    EXPECT_NO_THROW(runSemantic("misc34.rx"));
}
TEST(semantic1, misc35) {
    EXPECT_NO_THROW(runSemantic("misc35.rx"));
}
TEST(semantic1, misc36) {
    EXPECT_NO_THROW(runSemantic("misc36.rx"));
}
TEST(semantic1, misc37) {
    EXPECT_NO_THROW(runSemantic("misc37.rx"));
}
TEST(semantic1, misc38) {
    EXPECT_NO_THROW(runSemantic("misc38.rx"));
}
TEST(semantic1, misc39) {
    EXPECT_NO_THROW(runSemantic("misc39.rx"));
}
TEST(semantic1, misc4) {
    EXPECT_NO_THROW(runSemantic("misc4.rx"));
}
TEST(semantic1, misc40) {
    EXPECT_NO_THROW(runSemantic("misc40.rx"));
}
TEST(semantic1, misc41) {
    EXPECT_NO_THROW(runSemantic("misc41.rx"));
}
TEST(semantic1, misc42) {
    EXPECT_NO_THROW(runSemantic("misc42.rx"));
}
TEST(semantic1, misc43) {
    EXPECT_NO_THROW(runSemantic("misc43.rx"));
}
TEST(semantic1, misc44) {
    EXPECT_NO_THROW(runSemantic("misc44.rx"));
}
TEST(semantic1, misc45) {
    EXPECT_NO_THROW(runSemantic("misc45.rx"));
}
TEST(semantic1, misc46) {
    EXPECT_NO_THROW(runSemantic("misc46.rx"));
}
TEST(semantic1, misc47) {
    EXPECT_NO_THROW(runSemantic("misc47.rx"));
}
TEST(semantic1, misc48) {
    EXPECT_NO_THROW(runSemantic("misc48.rx"));
}
TEST(semantic1, misc49) {
    EXPECT_NO_THROW(runSemantic("misc49.rx"));
}
TEST(semantic1, misc5) {
    EXPECT_NO_THROW(runSemantic("misc5.rx"));
}
TEST(semantic1, misc50) {
    EXPECT_NO_THROW(runSemantic("misc50.rx"));
}
TEST(semantic1, misc51) {
    EXPECT_NO_THROW(runSemantic("misc51.rx"));
}
TEST(semantic1, misc52) {
    EXPECT_NO_THROW(runSemantic("misc52.rx"));
}
TEST(semantic1, misc53) {
    EXPECT_NO_THROW(runSemantic("misc53.rx"));
}
TEST(semantic1, misc54) {
    EXPECT_NO_THROW(runSemantic("misc54.rx"));
}
TEST(semantic1, misc55) {
    EXPECT_NO_THROW(runSemantic("misc55.rx"));
}
TEST(semantic1, misc56) {
    EXPECT_ANY_THROW(runSemantic("misc56.rx"));
}
TEST(semantic1, misc57) {
    EXPECT_ANY_THROW(runSemantic("misc57.rx"));
}
TEST(semantic1, misc58) {
    EXPECT_ANY_THROW(runSemantic("misc58.rx"));
}
TEST(semantic1, misc59) {
    EXPECT_ANY_THROW(runSemantic("misc59.rx"));
}
TEST(semantic1, misc6) {
    EXPECT_NO_THROW(runSemantic("misc6.rx"));
}
TEST(semantic1, misc60) {
    EXPECT_ANY_THROW(runSemantic("misc60.rx"));
}
TEST(semantic1, misc61) {
    EXPECT_ANY_THROW(runSemantic("misc61.rx"));
}
TEST(semantic1, misc62) {
    EXPECT_ANY_THROW(runSemantic("misc62.rx"));
}
TEST(semantic1, misc63) {
    EXPECT_ANY_THROW(runSemantic("misc63.rx"));
}
TEST(semantic1, misc64) {
    EXPECT_ANY_THROW(runSemantic("misc64.rx"));
}
TEST(semantic1, misc65) {
    EXPECT_NO_THROW(runSemantic("misc65.rx"));
}
TEST(semantic1, misc7) {
    EXPECT_NO_THROW(runSemantic("misc7.rx"));
}
TEST(semantic1, misc8) {
    EXPECT_NO_THROW(runSemantic("misc8.rx"));
}
TEST(semantic1, misc9) {
    EXPECT_NO_THROW(runSemantic("misc9.rx"));
}
TEST(semantic1, return1) {
    EXPECT_ANY_THROW(runSemantic("return1.rx"));
}
TEST(semantic1, return10) {
    EXPECT_ANY_THROW(runSemantic("return10.rx"));
}
TEST(semantic1, return11) {
    EXPECT_ANY_THROW(runSemantic("return11.rx"));
}
TEST(semantic1, return12) {
    EXPECT_NO_THROW(runSemantic("return12.rx"));
}
TEST(semantic1, return13) {
    EXPECT_NO_THROW(runSemantic("return13.rx"));
}
TEST(semantic1, return14) {
    EXPECT_NO_THROW(runSemantic("return14.rx"));
}
TEST(semantic1, return15) {
    EXPECT_NO_THROW(runSemantic("return15.rx"));
}
TEST(semantic1, return2) {
    EXPECT_NO_THROW(runSemantic("return2.rx"));
}
TEST(semantic1, return3) {
    EXPECT_ANY_THROW(runSemantic("return3.rx"));
}
TEST(semantic1, return4) {
    EXPECT_ANY_THROW(runSemantic("return4.rx"));
}
TEST(semantic1, return5) {
    EXPECT_ANY_THROW(runSemantic("return5.rx"));
}
TEST(semantic1, return6) {
    EXPECT_ANY_THROW(runSemantic("return6.rx"));
}
TEST(semantic1, return7) {
    EXPECT_ANY_THROW(runSemantic("return7.rx"));
}
TEST(semantic1, return8) {
    EXPECT_NO_THROW(runSemantic("return8.rx"));
}
TEST(semantic1, return9) {
    EXPECT_ANY_THROW(runSemantic("return9.rx"));
}
TEST(semantic1, type1) {
    EXPECT_ANY_THROW(runSemantic("type1.rx"));
}
TEST(semantic1, type10) {
    EXPECT_ANY_THROW(runSemantic("type10.rx"));
}
TEST(semantic1, type11) {
    EXPECT_ANY_THROW(runSemantic("type11.rx"));
}
TEST(semantic1, type12) {
    EXPECT_ANY_THROW(runSemantic("type12.rx"));
}
TEST(semantic1, type13) {
    EXPECT_ANY_THROW(runSemantic("type13.rx"));
}
TEST(semantic1, type14) {
    EXPECT_ANY_THROW(runSemantic("type14.rx"));
}
TEST(semantic1, type15) {
    EXPECT_ANY_THROW(runSemantic("type15.rx"));
}
TEST(semantic1, type16) {
    EXPECT_ANY_THROW(runSemantic("type16.rx"));
}
TEST(semantic1, type17) {
    EXPECT_ANY_THROW(runSemantic("type17.rx"));
}
TEST(semantic1, type18) {
    EXPECT_ANY_THROW(runSemantic("type18.rx"));
}
TEST(semantic1, type19) {
    EXPECT_ANY_THROW(runSemantic("type19.rx"));
}
TEST(semantic1, type2) {
    EXPECT_ANY_THROW(runSemantic("type2.rx"));
}
TEST(semantic1, type20) {
    EXPECT_ANY_THROW(runSemantic("type20.rx"));
}
TEST(semantic1, type3) {
    EXPECT_ANY_THROW(runSemantic("type3.rx"));
}
TEST(semantic1, type4) {
    EXPECT_ANY_THROW(runSemantic("type4.rx"));
}
TEST(semantic1, type5) {
    EXPECT_ANY_THROW(runSemantic("type5.rx"));
}
TEST(semantic1, type6) {
    EXPECT_ANY_THROW(runSemantic("type6.rx"));
}
TEST(semantic1, type7) {
    EXPECT_ANY_THROW(runSemantic("type7.rx"));
}
TEST(semantic1, type8) {
    EXPECT_ANY_THROW(runSemantic("type8.rx"));
}
TEST(semantic1, type9) {
    EXPECT_ANY_THROW(runSemantic("type9.rx"));
}