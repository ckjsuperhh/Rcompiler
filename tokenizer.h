//
// Created by ckjsuperhh6602 on 25-8-6.
//

#ifndef TOKENIZER_H
#define TOKENIZER_H
#include <iostream>
#include <utility>
#include<vector>
#include<boost/regex.hpp>
    enum class TokenType;

    struct Token {
        TokenType type{};
        std::string value{};
        Token():type(TokenType()){}
        Token(const TokenType t, const char* v) : type(t), value(v) {}
        Token(const TokenType t, std::string  v) : type(t), value(std::move(v)) {}
    };

    enum class TokenType {
        // 字面量
        String, // 字符串
        Identifier, // 标识符（原IDENTIFIER）
        Char, Bool,

        // 运算符
        Plus, // +（二元）
        Minus, // -（二元）
        Star, // *（原Multiply）
        Slash, // /（原Divide）
        Xor, // ^
        And, // 与运算
        Or, // 或运算
        Not, // 非运算
        Equal, // ==（原EQUAL的规范命名）
        NotEqual, // !=
        Less, // <
        Greater, // >
        LessEqual, // <=
        GreaterEqual, // >=
        As,
        OrOr,

        // 标点符号
        LParen, // (
        RParen, // )
        LBracket, // [
        RBracket, // ]
        LBrace, // {
        RBrace, // }
        Comma, // ,
        Dot, // .
        Semicolon, // ;
        Colon, // :
        DoubleColon, // ::（原ColonColon的规范命名）
        Arrow, // ->
        FatArrow, // =>
        Underscore, // _

        // 关键字
        Await,
        Break,
        Continue,
        Return,
        Exit, // 新增关键字
        If,
        Else,
        Match,
        Loop,
        For,
        While,
        In, // for循环中的in
        Struct,
        Enum,
        Union, // 原第一个版本的Union
        Const,
        Static, // 原第二个版本的Static
        Let,
        Mut, // 可变性关键字
        Fn, // 函数关键字
        Trait, // 特性关键字
        Impl, // 实现关键字

        // 特殊类型
        Whitespace, // 空白（原WHITESPACE）
        Invalid,// 无效 token（原INVALID）
        Integer, EqEq, Mod, Unit , self, Self, Andand,AndStr
        , MinusEqual, SlashEqual,PlusEqual,StarEqual, Basic,Eof // 文件结束



    };


    inline std::string TokenToOutput(const TokenType t) {
    switch (t) {
        // 字面量
        case TokenType::String:
            return "String";
        case TokenType::Identifier:
            return "Identifier";

        // 运算符
        case TokenType::Plus:
            return "Plus";
        case TokenType::Minus:
            return "Minus";
        case TokenType::Star:
            return "Star";
        case TokenType::Slash:
            return "Slash";
        case TokenType::Xor:
            return "Xor";
        case TokenType::And:
            return "And";
        case TokenType::Or:
            return "Or";
        case TokenType::Not:
            return "Not";
        case TokenType::Equal:
            return "Equal";
        case TokenType::NotEqual:
            return "NotEqual";
        case TokenType::Less:
            return "Less";
        case TokenType::Greater:
            return "Greater";
        case TokenType::LessEqual:
            return "LessEqual";
        case TokenType::GreaterEqual:
            return "GreaterEqual";

        // 标点符号
        case TokenType::LParen:
            return "LParen";
        case TokenType::RParen:
            return "RParen";
        case TokenType::LBracket:
            return "LBracket";
        case TokenType::RBracket:
            return "RBracket";
        case TokenType::LBrace:
            return "LBrace";
        case TokenType::RBrace:
            return "RBrace";
        case TokenType::Comma:
            return "Comma";
        case TokenType::Dot:
            return "Dot";
        case TokenType::Semicolon:
            return "Semicolon";
        case TokenType::Colon:
            return "Colon";
        case TokenType::DoubleColon:
            return "DoubleColon";
        case TokenType::Arrow:
            return "Arrow";
        case TokenType::FatArrow:
            return "FatArrow";
        case TokenType::Underscore:
            return "Underscore";

        // 关键字
        case TokenType::Await:
            return "Await";
        case TokenType::Break:
            return "Break";
        case TokenType::Continue:
            return "Continue";
        case TokenType::Return:
            return "Return";
        case TokenType::Exit:
            return "Exit";
        case TokenType::If:
            return "If";
        case TokenType::Else:
            return "Else";
        case TokenType::Match:
            return "Match";
        case TokenType::Loop:
            return "Loop";
        case TokenType::For:
            return "For";
        case TokenType::While:
            return "While";
        case TokenType::In:
            return "In";
        case TokenType::Struct:
            return "Struct";
        case TokenType::Enum:
            return "Enum";
        case TokenType::Union:
            return "Union";
        case TokenType::Const:
            return "Const";
        case TokenType::Static:
            return "Static";
        case TokenType::Let:
            return "Let";
        case TokenType::Mut:
            return "Mut";
        case TokenType::Fn:
            return "Fn";
        case TokenType::Trait:
            return "Trait";
        case TokenType::Impl:
            return "Impl";

        // 特殊类型
        case TokenType::Whitespace:
            return "Whitespace";
        case TokenType::Invalid:
            return "Invalid";
        case TokenType::Eof:
            return "Eof";
        case TokenType::Integer:
            return "Integer";
        case TokenType::EqEq:
            return "EqEq";
        case TokenType::Mod:
            return "Mod";

        default:
            return "UNKNOWN"; // 处理未来可能添加的新枚举值
    }
}


    class Tokenizer {
        std::string input;
        std::string::const_iterator start = input.begin();
        std::string::const_iterator end = input.end();
    public:
        explicit Tokenizer(std::string i): input(std::move(i)){
            start=input.begin();
            end=input.end();
        }

        void cleanComments() {
            std::string result;
            const auto n = input.size();
            int i = 0;
            bool in_block_comment = false;  // 是否在块注释中
            bool in_line_comment = false;   // 是否在行注释中

            while (i < n) {
                // 处理块注释
                if (in_block_comment) {
                    if (i + 1 < n && input[i] == '*' && input[i+1] == '/') {
                        in_block_comment = false;
                        i += 2;  // 跳过 "*/"
                    } else {
                        i++;  // 跳过注释内容
                    }
                    continue;
                }

                // 处理行注释
                if (in_line_comment) {
                    if (input[i] == '\n') {  // 行注释到换行符结束
                        in_line_comment = false;
                        result += input[i];  // 保留换行符
                        i++;
                    } else {
                        i++;  // 跳过注释内容
                    }
                    continue;
                }

                // 检查是否进入块注释
                if (i + 1 < n && input[i] == '/' && input[i+1] == '*') {
                    in_block_comment = true;
                    i += 2;  // 跳过 "/*"
                }
                // 检查是否进入行注释
                else if (i + 1 < n && input[i] == '/' && input[i+1] == '/') {
                    in_line_comment = true;
                    i += 2;  // 跳过 "//"
                }
                // 普通字符，添加到结果
                else {
                    result += input[i];
                    i++;
                }
            }

            this->input=result;
            this->start=input.begin();
            this->end=input.end();
        }

        std::vector<Token> tokenize() {
            this->cleanComments();
            std::vector<Token> tokens;
            boost::regex whitespace(R"(^\s+)");
            boost::smatch match;
            while (start != end) {
                while (boost::regex_search(start, end, match, whitespace, boost::match_continuous)) {
                    start += match.length();//跳过空白符
                }

                tokens.emplace_back(next_token());
                // std::cerr<<"tokenizer:"<<TokenToOutput(tokens.back().type)<<":"<<tokens.back().value<<std::endl;
                while (boost::regex_search(start, end, match, whitespace, boost::match_continuous)) {
                    start += match.length();//跳过空白符
                }
                // std::cerr<<tokens.back().value<<std::endl;
            }
            tokens.emplace_back(TokenType::Eof,"");
            return tokens;
        }

        Token next_token() {
            boost::regex keyword(R"(^\b(crate|mod|move|ref|self|Self|super|type|use|where|try|gen)\b)");
            boost::regex basic(R"(^\b(i32|u32|isize|usize|String|bool|char)\b)");
            boost::regex sstring(R"("[a-zA-Z0-9 ]*")");
            boost::regex underscore(R"(^\_)");;
            boost::regex bbool(R"(^\b(false|true)\b)");
            boost::regex sself(R"(^\b(self)\b)");
            boost::regex SSelf(R"(^\b(Self)\b)");
            boost::regex ttrait(R"(^\b(trait)\b)");
            boost::regex sstatic(R"(^\b(static)\b)");
            boost::regex sstruct(R"(^\b(struct)\b)");
            boost::regex mmatch(R"(^\b(match)\b)");
            boost::regex lloop(R"(^\b(loop)\b)");
            boost::regex iin(R"(^\b(in)\b)");
            boost::regex iimpl(R"(^\b(impl)\b)");
            boost::regex eenum(R"(^\b(enum)\b)");
            boost::regex ccontinue(R"(^\b(continue)\b)");
            boost::regex cconst(R"(^\b(const)\b)");
            boost::regex bbreak(R"(^\b(break)\b)");
            boost::regex iif(R"(^\b(if)\b)");
            boost::regex eelse(R"(^\b(else)\b)");
            boost::regex llet(R"(^\b(let)\b)");
            boost::regex ffn(R"(^\b(fn)\b)");
            boost::regex rreturn(R"(^\b(return)\b)");
            boost::regex mmut(R"(^\b(mut)\b)");
            boost::regex wwhile(R"(^\b(while)\b)");
            boost::regex identifier(R"(^[a-zA-Z][a-zA-Z0-9_]*)");
            boost::regex integer(R"(^\d+(?:_\d+)*(?:_?(?:u32|i32|usize|isize))?)");
            boost::regex op(R"(^(==|!=|>=|<=|\+|\-|\*|\/|%|<|>|=))");
            boost::regex arrow(R"(^->)");
            boost::regex sle(R"(^/=)");
            boost::regex ste(R"(^\*=)");
            boost::regex pe(R"(^\+=)");
            boost::regex me(R"(^\-=)");
            boost::regex aand(R"(^&)");
            boost::regex andand(R"(^&&)");
            boost::regex lparen(R"(^\()");
            boost::regex rparen(R"(^\))");
            boost::regex lbrace(R"(^\{)");
            boost::regex rbrace(R"(^\})");
            boost::regex semicolon(R"(^;)");
            boost::regex colon(R"(^:)");
            boost::regex doublecolon(R"(^::)");
            boost::regex cchar("'[a-zA-Z0-9]'");
            boost::regex aas(R"(^\b(as)\b)");
            boost::regex rbracket(R"(^\])") ;
            boost::regex lbracket(R"(^\[)") ;
            boost::regex comma(R"(^,)") ;
            boost::regex two_pipes(R"(\|\|)");
            boost::regex oor(R"(\|)") ;
            boost::regex dot(R"(^\.)") ;
            boost::regex unit(R"(^\(\)$)");
            boost::regex andstr(R"(&str)");
            boost::smatch match;
            std::vector<Token> waiting_tokens;
            if (boost::regex_search(start, end, match,andstr, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::AndStr, match.str());
            }
            if (boost::regex_search(start, end, match,oor, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Or, match.str());
            }
            if (boost::regex_search(start, end, match,two_pipes, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::OrOr, match.str());
            }
            if (boost::regex_search(start, end, match,underscore, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Underscore, match.str());
            }
            if (boost::regex_search(start, end, match,basic, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Basic, match.str());
            }
            if (boost::regex_search(start, end, match,eelse, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Else, match.str());
            }
            if (boost::regex_search(start, end, match,sstring, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::String, match.str());
            }
            if (boost::regex_search(start, end, match,pe , boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::PlusEqual, match.str());
            }
            if (boost::regex_search(start, end, match,me , boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::MinusEqual, match.str());
            }
            if (boost::regex_search(start, end, match,ste , boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::StarEqual, match.str());
            }
            if (boost::regex_search(start, end, match,sle , boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::SlashEqual, match.str());
            }
            if (boost::regex_search(start, end, match,dot , boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Dot, match.str());
            }
            if (boost::regex_search(start, end, match, aand , boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::And, match.str());
            }
            if (boost::regex_search(start, end, match, andand , boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Andand, match.str());
            }
            if (boost::regex_search(start, end, match, arrow , boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Arrow, match.str());
            }
            if (boost::regex_search(start, end, match, mmut , boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Mut, match.str());
            }
            if (boost::regex_search(start, end, match, aas , boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::As, match.str());
            }
            if (boost::regex_search(start, end, match, cchar , boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Char, match.str());
            }
            if (boost::regex_search(start, end, match, bbool, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Bool, match.str());
            }
            if (boost::regex_search(start, end, match, sself, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::self, match.str());
            }
            if (boost::regex_search(start, end, match, SSelf, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Self, match.str());
            }
            // trait 关键字匹配
            if (boost::regex_search(start, end, match, ttrait, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Trait, match.str());
            }
            if (boost::regex_search(start, end, match, unit, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Unit, match.str());
            }
            if (boost::regex_search(start, end, match, comma, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Comma, match.str());
            }

            if (boost::regex_search(start, end, match, lbracket, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::LBracket, match.str());
            }
            if (boost::regex_search(start, end, match, rbracket, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::RBracket, match.str());
            }

            if (boost::regex_search(start, end, match, colon, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Colon, match.str());
            }

            if (boost::regex_search(start, end, match, doublecolon, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::DoubleColon, match.str());
            }

            // static 关键字匹配
            if (boost::regex_search(start, end, match, sstatic, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Static, match.str());
            }

            // struct 关键字匹配
            if (boost::regex_search(start, end, match, sstruct, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Struct, match.str());;
            }

            // match 关键字匹配
            if (boost::regex_search(start, end, match, mmatch, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Match, match.str());;
            }

            if (boost::regex_search(start, end, match, op, boost::match_continuous)) {
                std::string op_str = match.str(); // 获取匹配到的运算符字符串
                TokenType ty = TokenType::Invalid; // 默认设为无效类型

                // 先判断双字符运算符（长符号优先）
                if (op_str == "==") {
                    ty = TokenType::EqEq;
                } else if (op_str == "!=") {
                    ty = TokenType::NotEqual;
                } else if (op_str == ">=") {
                    ty = TokenType::GreaterEqual;
                } else if (op_str == "<=") {
                    ty = TokenType::LessEqual;
                    // 再判断单字符运算符
                } else if (op_str == "=") {
                    ty = TokenType::Equal;
                } else if (op_str == "+") {
                    ty = TokenType::Plus;
                } else if (op_str == "-") {
                    ty = TokenType::Minus;
                } else if (op_str == "*") {
                    ty = TokenType::Star;
                } else if (op_str == "/") {
                    ty = TokenType::Slash;
                } else if (op_str == "%") {
                    ty = TokenType::Mod;
                } else if (op_str == "<") {
                    ty=TokenType::Less;
                } else if (op_str == ">") {
                    ty = TokenType::Greater;
                }
                // 若都不匹配，保持默认的 Invalid
                waiting_tokens.emplace_back(ty, match.str());
            }

            // loop 关键字匹配
            if (boost::regex_search(start, end, match, lloop, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Loop, match.str());;
            }

            // in 关键字匹配
            if (boost::regex_search(start, end, match, iin, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::In, match.str());;
            }

            // impl 关键字匹配
            if (boost::regex_search(start, end, match, iimpl, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Impl, match.str());;
            }

            // enum 关键字匹配
            if (boost::regex_search(start, end, match, eenum, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Enum, match.str());;
            }

            //  关键字匹配
            if (boost::regex_search(start, end, match, ccontinue, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Continue, match.str());;
            }

            // const 关键字匹配
            if (boost::regex_search(start, end, match, cconst, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Const, match.str());;
            }

            // break 关键字匹配
            if (boost::regex_search(start, end, match, bbreak, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Break, match.str());;
            }

            // if 关键字匹配
            if (boost::regex_search(start, end, match, iif, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::If, match.str());;
            }

            // let 关键字匹配
            if (boost::regex_search(start, end, match, llet, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Let, match.str());;
            }

            // fn 关键字匹配
            if (boost::regex_search(start, end, match, ffn, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Fn, match.str());;
            }

            // return 关键字匹配
            if (boost::regex_search(start, end, match, rreturn, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Return, match.str());;
            }

            // while 关键字匹配
            if (boost::regex_search(start, end, match, wwhile, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::While, match.str());;
            }
            if (boost::regex_search(start, end, match, identifier, boost::match_continuous)) {
                if (match.length() > 64) {
                    exit(-1); //如果匹配关键字过长，那么直接exit
                }

                waiting_tokens.emplace_back(TokenType::Identifier, match.str());;
            }
            if (boost::regex_search(start, end, match, integer, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Integer, match.str());
            }
            if (boost::regex_search(start, end, match, lparen, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::LParen, match.str());;
            }
            if (boost::regex_search(start, end, match, rparen, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::RParen, match.str());;
            }
            if (boost::regex_search(start, end, match, lbrace, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::LBrace, match.str());;
            }
            if (boost::regex_search(start, end, match, rbrace, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::RBrace, match.str());;
            }
            if (boost::regex_search(start, end, match, semicolon, boost::match_continuous)) {
                waiting_tokens.emplace_back(TokenType::Semicolon, match.str());;
            }
            match_raw_str(waiting_tokens);
            if (waiting_tokens.empty()) {
                throw std::runtime_error("Nothing can be matched!");
            }
            Token goal;
            for (auto a:waiting_tokens) {
                if (a.value.length()>goal.value.length()) {
                    goal=a;
                }
            }
            start+=goal.value.length();
            return goal;
        }

        void match_raw_str(std::vector<Token>& waiting_tokens) {
            int i=3;
            std::string target="r#\"";
            if (*start=='r'&&*(start+1)=='#'&&*(start+2)=='\"') {
                for (;*(start+i)!='\"';i++) {
                    target+=*(start+i);
                }
                if (*(start+i+1)!='#') {
                    throw std::runtime_error("INVALID RAW STRING");
                }
                target+="\"#";
                waiting_tokens.emplace_back(TokenType::String,target);
            }
        }

        static void ShowOutput(const std::vector<Token> &tokens) {
            for (const auto &[type, value]: tokens) {
                if (type==TokenType::String) {
                    std::cout<<value<<std::endl;
                }
                std::cout << TokenToOutput(type) << ":" << value << std::endl;
            }
        }
    };
#endif //TOKENIZER_H
