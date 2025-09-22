//
// Created by ckjsuperhh6602 on 25-9-19.
//

#ifndef PARSER_H
#define PARSER_H
#include <cmath>
#include <future>

#include "AST_node.h"
#include "semantic check.h"

#include "semantic.h"
#include "tokenizer.h"

enum class Preference {
    LOWEST,
    ASSIGNMENT, // =
    OR, // ||
    AND, // &&
    EQUALITY, // ==, !=
    XOR, // ^  （新增：异或）
    COMPARISON = 6, // <, >, <=, >=
    RANGE = 7, // .., ..=
    TERM = 8, // +, -
    FACTOR = 9, // *, /, %
    UNARY = 10, // !, - (一元)
    CALL = 11, // 函数调用、方法调用
    PRIMARY = 12 // 字面量、标识符、括号表达式等
};

class Parser {
public:
    std::vector<Token> lexer;
    size_t current_pos = 0;
    Token current_token = lexer[0];
    Parser()=default;
    explicit Parser(const std::vector<Token> &l):lexer(l){}
    void consume() {
        current_token = lexer[++current_pos];
    }
    void expect(const TokenType expected_type) const {
        if (current_token.type != expected_type) {
            throw std::runtime_error("Runtime error! Unexpected type!");
        }
    }
    Token peek(const int i = 0) {
        return lexer[current_pos + i];
    }
    [[nodiscard]] bool is_at_end() const {
        return lexer[current_pos].type == TokenType::Eof;
    }
    [[nodiscard]] bool check(const TokenType type) const {
        if (this->is_at_end()) return false;
        return lexer[current_pos].type == type;
    }
    bool match(const TokenType type) {
        if (check(type)) {
            consume();
            return true;
        }
        return false;
    }
    void rewind_to(const size_t pos) {
        current_pos = pos;
        current_token = lexer[pos];
    }

    // Pratt算法相关
    using PrefixParseFn = std::shared_ptr<Expr> (Parser::*)();
    using InfixParseFn = std::shared_ptr<Expr> (Parser::*)(std::shared_ptr<Expr>);
    struct ParseRule {
        PrefixParseFn prefix = nullptr;
        InfixParseFn infix = nullptr;
        Preference precedence = Preference::LOWEST;
    };
    static std::vector<ParseRule> get_rules() {
        static std::vector<ParseRule> rules;
        if (rules.empty()) {
            rules.resize(static_cast<size_t>(TokenType::Eof) + 1);
            rules[static_cast<size_t>(TokenType::Integer)].prefix = &Parser::parse_literal;
            rules[static_cast<size_t>(TokenType::String)].prefix = &Parser::parse_literal;
            rules[static_cast<size_t>(TokenType::Char)].prefix = &Parser::parse_literal;
            rules[static_cast<size_t>(TokenType::Bool)].prefix = &Parser::parse_literal;
            rules[static_cast<size_t>(TokenType::Identifier)].prefix = &Parser::parse_path;
            rules[static_cast<size_t>(TokenType::LBracket)].prefix = &Parser::parse_array;
            rules[static_cast<size_t>(TokenType::Not)].prefix = &Parser::parse_unary;
            rules[static_cast<size_t>(TokenType::Minus)].prefix = &Parser::parse_unary;
            rules[static_cast<size_t>(TokenType::LParen)].prefix = &Parser::parse_group;

            rules[static_cast<size_t>(TokenType::LParen)].infix = &Parser::parse_call;
            rules[static_cast<size_t>(TokenType::LParen)].precedence=Preference::CALL;
            rules[static_cast<size_t>(TokenType::LBracket)].infix = &Parser::parse_index;
            rules[static_cast<size_t>(TokenType::LBracket)].precedence = Preference::CALL;
            rules[static_cast<size_t>(TokenType::Dot)].infix = &Parser::parse_field;
            rules[static_cast<size_t>(TokenType::Dot)].precedence = Preference::CALL;









        }
        return rules;
    }
    std::shared_ptr<Expr> parse_expression(Preference min_precedence = Preference::LOWEST) {
        auto rules = get_rules();
        auto prefix = rules[static_cast<size_t>(peek().type)].prefix;
        if (prefix == nullptr) {
            throw std::runtime_error("Expected expression");
        }
        auto left = (this->*prefix)();
        while (true) {
            auto next_type = peek().type;
            if (const auto next_precedence = rules[static_cast<size_t>(next_type)].precedence; next_precedence <= min_precedence) {
                break;
            }
            const auto infix = rules[static_cast<size_t>(next_type)].infix;
            if (infix == nullptr) {
                break;
            }
            left = (this->*infix)(std::move(left));
        }
        return left;
    }

    std::shared_ptr<Expr> parse_literal() {
        const auto &[type, value] = peek();
        consume();
        TypeName t=TypeName::ErrorType;
        switch (type) {
            case TokenType::Integer:
                t=TypeName::Integer;//我决定到semantic里边再说
                break;
            case TokenType::Bool:
                t=TypeName::Bool;
                break;
            case TokenType::String:
                t=TypeName::String;
                break;
            case TokenType::Char:
                t=TypeName::Char;
                break;
            default:
                throw std::runtime_error("Invalid type in literal node");
        }
        return std::make_shared<LiteralExpr>(value,std::make_shared<BasicType>(t));
    }

    std::shared_ptr<Expr> parse_path() {
        std::vector<std::string> segments;
        segments.push_back(peek().value);
        consume();
        while (peek().type == TokenType::DoubleColon) {
            consume();
            expect(TokenType::Identifier);
            segments.push_back(peek().value);
            consume();
        }
        return std::make_shared<PathExpr>(std::move(segments));
    }

    std::shared_ptr<Expr> parse_array() {
        consume();
        if (peek().type == TokenType::RBracket) {
            throw std::runtime_error("Empty array");
        }
        auto element_candidate=parse_expression();
        auto start_pos=current_pos;
        if (match(TokenType::Semicolon)) {
            auto length=parse_expression();
            expect(TokenType::RBracket);
            consume();
            return std::make_shared<ArraySimplifiedExpr>(element_candidate,length);
        }
        rewind_to(start_pos);
        std::vector<std::shared_ptr<Expr>> elements;
        elements.push_back(element_candidate);
        while (match(TokenType::Comma)) {
            if (check(TokenType::RBracket)) {
                break;
            }
            elements.push_back(parse_expression());
        }
        expect(TokenType::RBracket);
        consume();
        return std::make_shared<ArrayInitExpr>(std::move(elements));
    }

    std::shared_ptr<Expr> parse_index(std::shared_ptr<Expr> left) {
        consume();
        auto index=parse_expression();
        expect(TokenType::RBracket);
        consume();
        return std::make_shared<ArrayAccessExpr>(std::move(left),std::move(index));
    }

    std::shared_ptr<Expr> parse_unary() {
        auto op_type = peek().type;
        consume();
        // 一元运算符的操作数优先级需高于 UNARY（确保正确解析嵌套，如 !a + b 不会被解析为 !(a + b)）
        auto operand = parse_expression(Preference::UNARY);
        switch (op_type) {
            case TokenType::Not:
                return std::make_shared<UnaryExpr>("!", std::move(operand));
            case TokenType::Minus:
                return std::make_shared<UnaryExpr>("-", std::move(operand));
            default:
                throw std::runtime_error("Unsupported unary operator");
        }
    }

    std::shared_ptr<Expr> parse_call(std::shared_ptr<Expr> left) {
        consume();
        std::vector<std::shared_ptr<Expr> > args;
        if (!check(TokenType::RParen)) {
            do {
                if (check(TokenType::RParen)) {
                    break;
                }
                args.push_back(parse_expression());
            }while (match(TokenType::Comma));
        }
        consume();
        return std::make_shared<CallExpr>(std::move(left),std::move(args));
    }

    std::shared_ptr<Expr> parse_field(std::shared_ptr<Expr> left) {
        consume();
        auto right=parse_expression();
        return std::make_shared<FieldAccessExpr>(std::move(left),std::move(right));
    }

    std::shared_ptr<Expr> parse_group() {
        consume();
        auto inner=parse_expression();
        expect(TokenType::RParen);
        consume();
        return std::make_shared<GroupedExpr>(inner);
    }

    std::shared_ptr<Expr> parse_continue() {
        consume();
        return std::make_shared<ContinueExpr>();
    }

    std::shared_ptr<Expr> parse_break() {//我怎么判断有没有一个expr紧接在后边?
        consume();
        if (peek().type == TokenType::Semicolon||peek().type == TokenType::RBracket) {
            return std::make_shared<BreakExpr>();
        }
        auto expr=parse_expression();
        return std::make_shared<BreakExpr>(std::move(expr));
    }

    std::shared_ptr<Expr> parse_loop() {
        consume();
        expect(TokenType::LBracket);
        auto block=parse_block();
        return std::make_shared<LoopExpr>(std::move(block));
    }

    std::shared_ptr<Expr> parse_while() {
        consume();
        expect(TokenType::LParen);
        consume();
        auto expr=parse_expression();
        expect(TokenType::RParen);
        consume();
        expect(TokenType::LBracket);
        auto body=parse_block();
        return std::make_shared<WhileExpr>(expr,body);
    }

    std::shared_ptr<Expr> parse_if() {
        consume();
        expect(TokenType::LParen);
        consume();
        auto expr=parse_expression();
        expect(TokenType::RParen);
        consume();
        expect(TokenType::LBracket);
        auto then=parse_block();
        std::shared_ptr<Expr> else_body=nullptr;
        if (match(TokenType::Else)) {
            if (peek().type == TokenType::If) {
                else_body=parse_if();
            }else if (peek().type == TokenType::LBracket){
                else_body=parse_block();
            }else {
                throw std::runtime_error("else without an LBracket!");
            }
        }
        return std::make_shared<IfExpr>(std::move(expr),std::move(then),std::move(else_body));
    }

    std::shared_ptr<Expr> parse_return() {
        consume();
        if (peek().type == TokenType::Semicolon||peek().type == TokenType::RBracket) {
            return std::make_shared<ReturnExpr>();
        }
        auto expr=parse_expression();
        return std::make_shared<ReturnExpr>(std::move(expr));

    }

    std::shared_ptr<Expr> parse_block() {
        consume();
        if (match(TokenType::RBracket)) {
            return std::make_shared<BlockExpr>(std::vector<std::pair<std::shared_ptr<ASTNode>,bool>>());
        }
        std::vector<std::pair<std::shared_ptr<ASTNode>,bool>> elements;
        do {
            auto st=parse_statement();
            bool has_semicolon=match(TokenType::Semicolon);
            elements.emplace_back(st,has_semicolon);
        }while (match(TokenType::RBracket));
        return std::make_shared<BlockExpr>(std::move(elements));
    }

    std::shared_ptr<Expr> parse_struct() {

    }

    std::shared_ptr<EnumStmt> parse_enum() {
        expect(TokenType::Identifier);
        auto name=peek().value;
        consume();
        expect(TokenType::LBracket);
        consume();
        std::vector<std::string> ids;
        if (!match(TokenType::RBracket)) {
            do {
                if (match(TokenType::RBracket)) {
                    break;
                }
                expect(TokenType::Identifier);
                ids.push_back(peek().value);
            }while (match(TokenType::Comma));
        }
        return std::make_shared<EnumStmt>(name,ids);
    }

    std::shared_ptr<LetStmt> parse_let() {
        consume();
        bool is_mutable = false;
        if (peek().type == TokenType::Mut) {
            is_mutable=true;
            consume();
        }
        expect(TokenType::Identifier);
        std::string name = peek().value;
        consume();
        expect(TokenType::Colon);
        consume();
        auto type_annotation = parse_type();
        expect(TokenType::Equal);
        consume();
        auto expr=parse_expression();
        return std::make_shared<LetStmt>(is_mutable,name,std::move(type_annotation),std::move(expr));
    }

    std::shared_ptr<ASTNode> parse_statement() {
        switch (peek().type) {
            case TokenType::Let:
                return parse_let();
                case TokenType::Const:
                return parse_const();
            case TokenType::Fn:
                return parse_function();
                case TokenType::Struct:
                return parse_struct_declare();
                case TokenType::Enum:
                return parse_enum();
            case TokenType::Trait:
                return parse_trait();
                case TokenType::Impl:
                return parse_impl();
                case TokenType::If://下边三个虽然是表达式，但是实际上他们在作为开头的时候，默认会进行一个直接的读入，作为块语句
                return parse_if();
            case TokenType::Loop:
                return parse_loop();
                case TokenType::While:
                return parse_while();
                default:
                return parse_expression();
        }
    }

    std::shared_ptr<Type> parse_type() {

    }




};
#endif //PARSER_H
