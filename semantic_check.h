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


struct LoopExpr;
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
struct IdentifierType;

struct SymbolEntry {
    bool is_Global;
};

class SymbolTable {
    std::vector<std::unordered_map<std::string,SymbolEntry>> scopes;

};
class SemanticCheck {
public:
    void pre_processor(ASTNode *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(Program *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(BasicType *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(ArrayType *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(LiteralExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(LoopExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(BlockExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(ArrayInitExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(ArrayAccessExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(BinaryExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(UnaryExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(PathExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(GroupedExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(FieldAccessExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(CallExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(StructExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(IfExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(FnStmt *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(ContinueExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(BreakExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(RangeExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(ReturnExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(LetStmt *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(ConstStmt *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(AssignmentStmt *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(ArraySimplifiedExpr*node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(IdentifierType*node,ASTNode*F,ASTNode* l,ASTNode* f);
    
};







#endif //SEMANTIC_CHECK_H
