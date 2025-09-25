//
// Created by ckjsuperhh6602 on 25-9-17.
//

#ifndef SEMANTIC_CHECK_H
#define SEMANTIC_CHECK_H
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "AST_node.h"


struct ASTNode;
struct Expr;
struct Type;
struct BasicType;
struct ArrayType;
struct FunctionType;
struct StructType;
struct TupleType;
struct LiteralExpr;
struct InfiniteLoopExpr;
struct BlockExpr;
struct ArrayInitExpr;
struct ArrayAccessExpr;
struct ArraySimplifiedExpr;
struct VariableExpr;
struct BinaryExpr;
struct UnaryExpr;
struct PathExpr;
struct GroupedExpr;
struct TupleExpr;
struct TupleIndexExpr;
struct FieldAccessExpr;
struct CallExpr;
struct MethodCallExpr;
struct StructExpr;
struct IfExpr;
struct WhileLoopExpr;
struct ExitStmt;
struct ForLoopStmt;
struct FnStmt;
struct WhileLetExpr;
struct ContinueExpr;
struct BreakExpr;
struct RangeExpr;
struct ReturnExpr;
struct LetStmt;
struct MatchExpr;
struct BlockStmt;
struct IfStmt;
struct WhileLoopStmt;
struct WhileLetStmt;
struct InfiniteLoopStmt;
struct UnderscoreExpr;
struct Error;
struct Program;
struct ConstStmt;
struct AssignmentStmt;

struct SymbolEntry {

};

class SymbolTable {
    std::vector<std::unordered_map<std::string,SymbolEntry>> scopes;




};

class SemanticCheck {
    SymbolTable symbol_table;
public:
    static void error(const std::string& msg) {
        throw std::runtime_error(msg);
    }
    // void analyze(const std::shared_ptr<ASTNode> &root) {
    //     root->accept1(this);
    //     root->accept2(this);
    //     root->accept3(this);
    //     root->accept4(this);
    //     root->accept5(this);
    // }
    void phase1(const BasicType *node) {

    }
    void phase2(const BasicType *node) {

    }
    void phase3(const BasicType *node) {

    }
    void phase4(const BasicType *node) {

    }
    void phase5(const BasicType *node) {

    }
    void phase1(const ArrayType*node) {

    }
    void phase2(const ArrayType *node) {

    }
    void phase3(const ArrayType *node) {

    }
    void phase4(const ArrayType *node) {

    }
    void phase5(const ArrayType *node) {

    }
};







#endif //SEMANTIC_CHECK_H
