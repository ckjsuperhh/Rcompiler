//
// Created by ckjsuperhh6602 on 25-9-19.
//

#ifndef PARSER_H
#define PARSER_H
#include <cmath>
#include <future>

#include "AST_node.h"
#include "semantic_check.h"

#include "tokenizer.h"

enum class Preference {
    LOWEST,
    ASSIGNMENT, // =
    OR, // ||
    AND, // &&
    EQUALITY, // ==, !=
    XOR, // ^  （新增：异或）
    COMPARISON = 6, // <, >, <=, >=
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
            rules[static_cast<size_t>(TokenType::LBrace)].prefix = &Parser::parse_block;



            rules[static_cast<size_t>(TokenType::LParen)].infix = &Parser::parse_call;
            rules[static_cast<size_t>(TokenType::LParen)].precedence=Preference::CALL;
            rules[static_cast<size_t>(TokenType::LBracket)].infix = &Parser::parse_index;
            rules[static_cast<size_t>(TokenType::LBracket)].precedence = Preference::CALL;
            rules[static_cast<size_t>(TokenType::Dot)].infix = &Parser::parse_field;
            rules[static_cast<size_t>(TokenType::Dot)].precedence = Preference::CALL;
            rules[static_cast<size_t>(TokenType::Plus)].infix = &Parser::parse_binary;
            rules[static_cast<size_t>(TokenType::Plus)].precedence=Preference::TERM;
            rules[static_cast<size_t>(TokenType::Minus)].infix = &Parser::parse_binary;
            rules[static_cast<size_t>(TokenType::Minus)].precedence=Preference::TERM;
            rules[static_cast<size_t>(TokenType::Star)].infix = &Parser::parse_binary;
            rules[static_cast<size_t>(TokenType::Star)].precedence=Preference::FACTOR;
            rules[static_cast<size_t>(TokenType::Slash)].infix = &Parser::parse_binary;
            rules[static_cast<size_t>(TokenType::Slash)].precedence=Preference::FACTOR;
            rules[static_cast<size_t>(TokenType::Mod)].infix = &Parser::parse_binary;
            rules[static_cast<size_t>(TokenType::Mod)].precedence=Preference::FACTOR;
            rules[static_cast<size_t>(TokenType::Greater)].infix = &Parser::parse_binary;
            rules[static_cast<size_t>(TokenType::Greater)].precedence=Preference::COMPARISON;
            rules[static_cast<size_t>(TokenType::Less)].infix = &Parser::parse_binary;
            rules[static_cast<size_t>(TokenType::Less)].precedence=Preference::COMPARISON;
            rules[static_cast<size_t>(TokenType::GreaterEqual)].infix = &Parser::parse_binary;
            rules[static_cast<size_t>(TokenType::GreaterEqual)].precedence=Preference::COMPARISON;
            rules[static_cast<size_t>(TokenType::LessEqual)].infix = &Parser::parse_binary;
            rules[static_cast<size_t>(TokenType::LessEqual)].precedence=Preference::COMPARISON;
            rules[static_cast<size_t>(TokenType::Xor)].infix = &Parser::parse_binary;
            rules[static_cast<size_t>(TokenType::Xor)].precedence=Preference::XOR;
            rules[static_cast<size_t>(TokenType::EqEq)].infix = &Parser::parse_binary;
            rules[static_cast<size_t>(TokenType::EqEq)].precedence=Preference::EQUALITY;
            rules[static_cast<size_t>(TokenType::NotEqual)].infix = &Parser::parse_binary;
            rules[static_cast<size_t>(TokenType::NotEqual)].precedence=Preference::EQUALITY;









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

    std::shared_ptr<Expr> parse_binary(std::shared_ptr<Expr> left) {
        std::string op;
        switch (peek().type) {
            case TokenType::Plus:
                op = "+";
                break;
                case TokenType::Minus:
                op = "-";
                break;
                case TokenType::Star:
                op = "*";
                break;
                case TokenType::Slash:
                op="/";
                break;
                case TokenType::Mod:
                op="%";
                break;
                case TokenType::Greater:
                op=">";
                break;
                case TokenType::GreaterEqual:
                op=">=";
                break;
                case TokenType::Less:
                op="<";
                break;
                case TokenType::LessEqual:
                op="<=";
                break;
                case TokenType::Xor:
                op="^";
                break;
            default:
                throw std::runtime_error("Unexpected op");
        }
        consume();
        auto right=parse_expression();
        return std::make_shared<BinaryExpr>(left,op,right);
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
        if (peek().type == TokenType::RParen) {
            consume();
            return std::make_shared<UnitExpr>();
        }
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
        if (peek().type == TokenType::Semicolon||peek().type == TokenType::RBrace) {
            return std::make_shared<BreakExpr>();
        }
        auto expr=parse_expression();
        return std::make_shared<BreakExpr>(std::move(expr));
    }

    std::shared_ptr<Expr> parse_loop() {
        consume();
        expect(TokenType::LBrace);
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
        expect(TokenType::LBrace);
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
        expect(TokenType::LBrace);
        auto then=parse_block();
        std::shared_ptr<Expr> else_body=nullptr;
        if (match(TokenType::Else)) {
            if (peek().type == TokenType::If) {
                else_body=parse_if();
            }else if (peek().type == TokenType::LBrace){
                else_body=parse_block();
            }else {
                throw std::runtime_error("else without an LBrace!");
            }
        }
        return std::make_shared<IfExpr>(std::move(expr),std::move(then),std::move(else_body));
    }

    std::shared_ptr<Expr> parse_return() {
        consume();
        if (peek().type == TokenType::Semicolon||peek().type == TokenType::RBrace) {
            return std::make_shared<ReturnExpr>();
        }
        auto expr=parse_expression();
        return std::make_shared<ReturnExpr>(std::move(expr));

    }

    std::shared_ptr<Expr> parse_block() {
        consume();
        if (match(TokenType::RBrace)) {
            return std::make_shared<BlockExpr>(std::vector<std::pair<std::shared_ptr<ASTNode>,bool>>());
        }
        std::vector<std::pair<std::shared_ptr<ASTNode>,bool>> elements;
        do {
            auto st=parse_statement();
            bool has_semicolon=match(TokenType::Semicolon);
            elements.emplace_back(st,has_semicolon);
        }while (!match(TokenType::RBrace));
        return std::make_shared<BlockExpr>(std::move(elements));
    }

    std::shared_ptr<Expr> parse_struct() {

    }

    std::shared_ptr<EnumStmt> parse_enum() {
        expect(TokenType::Identifier);
        auto name=peek().value;
        consume();
        expect(TokenType::LBrace);
        consume();
        std::vector<std::string> ids;
        if (!match(TokenType::RBrace)) {
            do {
                if (match(TokenType::RBrace)) {
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
        std::shared_ptr<Expr> expr=nullptr;
        if (match(TokenType::Equal)) {
            expr=parse_expression();
        }
        return std::make_shared<LetStmt>(name,std::move(type_annotation),std::move(expr),is_mutable);
    }

    std::shared_ptr<FnStmt> parse_function() {
        consume();
        expect(TokenType::Identifier);
        auto name=peek().value;
        consume();
        expect(TokenType::LParen);
        consume();
        std::vector<Param> params;
        if (!match(TokenType::RParen)) {
            Param t;
            if (parse_self(t)) {
                params.emplace_back(std::move(t));
            }
            while (match(TokenType::Comma)) {
                if (match(TokenType::RParen)) {
                    break;
                }
                expect(TokenType::Identifier);
                t.name=peek().value;
                consume();
                expect(TokenType::Colon);
                consume();
                t.type=parse_type();
                params.emplace_back(t);
            }
        }
        std::shared_ptr<Type> func_type=nullptr;
        if (match(TokenType::Arrow)) {
            func_type=parse_type();
        }
        if (peek().type == TokenType::Semicolon) {
            return std::make_shared<FnStmt>(name,params,std::move(func_type),nullptr);
        }
        expect(TokenType::LBrace);
        auto body=parse_block();
        return std::make_shared<FnStmt>(name,params,std::move(func_type),std::move(body));
    }

    bool parse_self(Param &p) {
        if (!(peek().type==TokenType::self||peek(1).type==TokenType::self||peek(2).type==TokenType::self)) {
            return false;
        }
        std::shared_ptr<Type> type;
        if (match(TokenType::And)) {
            type->is_and=true;
        }
        if (match(TokenType::Mut)) {
            type->is_mutable=true;
        }
        if (match(TokenType::self)) {
            type->node_type=TypeName::selfType;
        }else {
            throw std::runtime_error("invalid things before self type");
        }
        p={"self",std::move(type)};
        return true;
    }

    std::shared_ptr<Stmt> parse_impl() {
    consume();
    expect(TokenType::Identifier);
    auto name = peek().value;
    consume();

    if (match(TokenType::For)) {
        expect(TokenType::Identifier);
        auto struct_name = peek().value;
        consume();
        expect(TokenType::LBrace);
        consume();

        std::vector<std::pair<std::shared_ptr<FnStmt>, bool>> fns;
        std::vector<std::pair<std::shared_ptr<ConstStmt>, bool>> cons;

        if (!match(TokenType::RBrace)) {
            while (!match(TokenType::RBrace)) {
                auto stmt = parse_statement();  // stmt是shared_ptr<Stmt>

                if (stmt->node_type == TypeName::FnStmt) {
                    // 使用dynamic_pointer_cast转换智能指针（关键修正）
                    auto fn_stmt = std::dynamic_pointer_cast<FnStmt>(stmt);
                    if (!fn_stmt) {
                        throw std::runtime_error("Expected FnStmt in impl block");
                    }
                    fns.emplace_back(fn_stmt, match(TokenType::Semicolon));
                } else if (stmt->node_type == TypeName::ConstStmt) {
                    // 使用dynamic_pointer_cast转换智能指针（关键修正）
                    auto const_stmt = std::dynamic_pointer_cast<ConstStmt>(stmt);
                    if (!const_stmt) {
                        throw std::runtime_error("Expected ConstStmt in impl block");
                    }
                    cons.emplace_back(const_stmt, match(TokenType::Semicolon));
                } else {
                    throw std::runtime_error("invalid things within impl scope");
                }
            }
        }

        return std::make_shared<TraitImplStmt>(name, struct_name, fns, cons);
    }

    expect(TokenType::LBrace);
    consume();

    std::vector<std::pair<std::shared_ptr<FnStmt>, bool>> fns;
    std::vector<std::pair<std::shared_ptr<ConstStmt>, bool>> cons;

    if (!match(TokenType::RBrace)) {
        while (!match(TokenType::RBrace)) {
            auto stmt = parse_statement();  // stmt是shared_ptr<Stmt>

            if (stmt->node_type == TypeName::FnStmt) {
                // 使用dynamic_pointer_cast转换智能指针（关键修正）
                auto fn_stmt = std::dynamic_pointer_cast<FnStmt>(stmt);
                if (!fn_stmt) {
                    throw std::runtime_error("Expected FnStmt in impl block");
                }
                fns.emplace_back(fn_stmt, match(TokenType::Semicolon));
            } else if (stmt->node_type == TypeName::ConstStmt) {
                // 使用dynamic_pointer_cast转换智能指针（关键修正）
                auto const_stmt = std::dynamic_pointer_cast<ConstStmt>(stmt);
                if (!const_stmt) {
                    throw std::runtime_error("Expected ConstStmt in impl block");
                }
                cons.emplace_back(const_stmt, match(TokenType::Semicolon));
            } else {
                throw std::runtime_error("invalid things within impl scope");
            }
        }
    }

    return std::make_shared<InherentImplStmt>(name, fns, cons);
}

std::shared_ptr<TraitStmt> parse_trait() {
    consume();
    expect(TokenType::Identifier);
    auto name = peek().value;
    consume();
    expect(TokenType::LBrace);
    consume();

    std::vector<std::pair<std::shared_ptr<FnStmt>, bool>> fns;
    std::vector<std::pair<std::shared_ptr<ConstStmt>, bool>> cons;

    if (!match(TokenType::RBrace)) {
        while (!match(TokenType::RBrace)) {
            auto stmt = parse_statement();  // stmt是shared_ptr<Stmt>

            if (stmt->node_type == TypeName::FnStmt) {
                // 使用dynamic_pointer_cast转换智能指针（关键修正）
                auto fn_stmt = std::dynamic_pointer_cast<FnStmt>(stmt);
                if (!fn_stmt) {
                    throw std::runtime_error("Expected FnStmt in trait block");
                }
                fns.emplace_back(fn_stmt, match(TokenType::Semicolon));
            } else if (stmt->node_type == TypeName::ConstStmt) {
                // 使用dynamic_pointer_cast转换智能指针（关键修正）
                auto const_stmt = std::dynamic_pointer_cast<ConstStmt>(stmt);
                if (!const_stmt) {
                    throw std::runtime_error("Expected ConstStmt in trait block");
                }
                cons.emplace_back(const_stmt, match(TokenType::Semicolon));
            } else {
                throw std::runtime_error("invalid things within trait scope");
            }
        }
    }

    return std::make_shared<TraitStmt>(name, fns, cons);
}


    std::shared_ptr<StructStmt> parse_struct_declare() {
        consume();
        expect(TokenType::Identifier);
        auto name=peek().value;
        consume();
        expect(TokenType::LBrace);
        consume();
        std::vector<Field> elements;
        if (!match(TokenType::RBrace)) {
            do {
                if (match(TokenType::Comma)) {
                    expect(TokenType::RBrace);
                    consume();
                    break;
                }
                expect(TokenType::Identifier);
                std::string id=peek().value;
                consume();
                expect(TokenType::Colon);
                consume();
                auto type_annotation = parse_type();
                elements.emplace_back(id,std::move(type_annotation));
            }while (match(TokenType::Comma));
        }
        return std::make_shared<StructStmt>(name,elements);
    }

    std::shared_ptr<ConstStmt> parse_const() {
        consume();
        expect(TokenType::Identifier);
        std::string name = peek().value;
        consume();
        expect(TokenType::Colon);
        consume();
        auto type_annotation = parse_type();
        std::shared_ptr<Expr> expr=nullptr;
        if (match(TokenType::Equal)) {
            expr=parse_expression();
        }
        return std::make_shared<ConstStmt>(name,std::move(type_annotation),std::move(expr));
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

    std::shared_ptr<Type> parse_basic_type() {
        std::string type_name = current_token.value;
        if (type_name=="bool") {
            consume();
            return std::make_shared<BasicType>(TypeName::Bool);
        }
        if (type_name=="char") {
            consume();
            return std::make_shared<BasicType>(TypeName::Char);
        }
        if (type_name=="String") {
            consume();
            return std::make_shared<BasicType>(TypeName::String);
        }
        if (type_name=="i32") {
            consume();
            return std::make_shared<BasicType>(TypeName::i32);
        }
        if (type_name=="u32") {
            consume();
            return std::make_shared<BasicType>(TypeName::u32);
        }
        if (type_name=="isize") {
            consume();
            return std::make_shared<BasicType>(TypeName::isize);
        }
        if (type_name=="usize") {
            consume();
            return std::make_shared<BasicType>(TypeName::usize);
        }
        if (type_name=="()") {
            consume();
            return std::make_shared<BasicType>(TypeName::unit);
        }
        consume();
        return std::make_shared<ErrorType>();
    }



    std::shared_ptr<Type> parse_array_type() {
        consume();
        std::shared_ptr<Type> elem_type = parse_type();
        expect(TokenType::Semicolon);
        consume();
        auto length=parse_expression();
        expect(TokenType::RBrace);
        consume();
        if (const auto a=dynamic_cast<ArrayType*>(elem_type.get());a) {
            return std::make_shared<ArrayType>(elem_type,length,a->dimension+1);
        }
        return std::make_shared<ArrayType>(elem_type, length);
    }


    std::shared_ptr<Type> parse_identifier_type() {
        auto name=current_token.value;
        consume();
        return std::make_shared<IdentifierType>(name);
    }

    std::shared_ptr<Type> parse_Self_type() {
        consume();
        return std::make_shared<SelfType>();
    }

    std::shared_ptr<Type> parse_type() {
        bool is_and=false;
        bool is_mutable=false;
        if (match(TokenType::And)) {
            is_and=true;
        }
        if (match(TokenType::Mut)) {
            is_mutable=true;
        }
        std::shared_ptr<Type> type;
        switch (peek().type) {
            case TokenType::LBracket:
                type= parse_array_type();
                break;
            case TokenType::Identifier:
                type= parse_identifier_type();
                break;
            case TokenType::Self:
                type= parse_Self_type();
                break;
            default:
                type= parse_basic_type();
        }
        type->is_and=is_and;
        type->is_mutable=is_mutable;
        return type;
    }

std::shared_ptr<ASTNode> parse() {
    std::vector<std::shared_ptr<ConstStmt>> cons;
    std::vector<std::shared_ptr<FnStmt>> fns;
    std::vector<std::shared_ptr<EnumStmt>> enums;
    std::vector<std::shared_ptr<StructStmt>> structs;
    std::vector<std::shared_ptr<InherentImplStmt>> inherits;
    std::vector<std::shared_ptr<TraitStmt>> traits;
    std::vector<std::shared_ptr<TraitImplStmt>> impls;

    while (!is_at_end()) {
        auto stmt = parse_statement();  // stmt 是 shared_ptr<ASTNode>

        // 使用 dynamic_pointer_cast 转换智能指针（关键修正）
        switch (stmt->get_type()) {
            // std::cerr<<stmt->get_type()
            case TypeName::ConstStmt: {
                auto const_stmt = std::dynamic_pointer_cast<ConstStmt>(stmt);
                if (const_stmt) cons.emplace_back(const_stmt);
                break;
            }
            case TypeName::FnStmt: {
                auto fn_stmt = std::dynamic_pointer_cast<FnStmt>(stmt);
                if (fn_stmt) fns.emplace_back(fn_stmt);
                break;
            }
            case TypeName::EnumStmt: {
                auto enum_stmt = std::dynamic_pointer_cast<EnumStmt>(stmt);
                if (enum_stmt) enums.emplace_back(enum_stmt);
                break;
            }
            case TypeName::StructStmt: {
                auto struct_stmt = std::dynamic_pointer_cast<StructStmt>(stmt);
                if (struct_stmt) structs.emplace_back(struct_stmt);
                break;
            }
            case TypeName::InherentImplStmt: {
                auto inherent_stmt = std::dynamic_pointer_cast<InherentImplStmt>(stmt);
                if (inherent_stmt) inherits.emplace_back(inherent_stmt);
                break;
            }
            case TypeName::TraitStmt: {
                auto trait_stmt = std::dynamic_pointer_cast<TraitStmt>(stmt);
                if (trait_stmt) traits.emplace_back(trait_stmt);
                break;
            }
            case TypeName::TraitImplStmt: {
                auto impl_stmt = std::dynamic_pointer_cast<TraitImplStmt>(stmt);
                if (impl_stmt) impls.emplace_back(impl_stmt);
                break;
            }
            default:
                throw std::runtime_error("Unexpected statement type");
        }
    }

    return std::make_shared<Program>(cons, fns, enums, structs, inherits, traits, impls);
}
};
#endif //PARSER_H
