//
// Created by ckjsuperhh6602 on 25-9-17.
//

#ifndef SEMANTIC_CHECK_H
#define SEMANTIC_CHECK_H
#include <any>
#include <memory>
#include <string>
#include <unordered_map>

#include "AST_node.h"

struct SelfType;
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
struct WhileExpr;
struct RustType;
struct StructStmt;
struct AssignmentExpr;
struct AsExpr;
struct UnitExpr;
struct EnumStmt;
struct InherentImplStmt;
inline unsigned int structNum = 0;

struct SymbolEntry {
    std::shared_ptr<Type> type;
    std::any eval;
    bool is_mutable;
    bool is_Global;
};
struct SymbolTable {
        std::unordered_map<std::string,SymbolEntry>item_Table,type_Table;
        SymbolEntry lookup_t(std::string stri, ASTNode *F=nullptr);
        SymbolEntry lookup_i(std::string stri, ASTNode *F=nullptr);

        void setItem(std::string, SymbolEntry);

        void setType(std::string, SymbolEntry);
    };
class SemanticCheck {

public:
    void resolveDependency(ASTNode *node, std::shared_ptr<Type> SelfType);

    void pre_processor(ASTNode *node,ASTNode* F,ASTNode* l,ASTNode* f);

    static std::shared_ptr<Type> TypeToItem(std::shared_ptr<Type> t);

    static std::shared_ptr<Type> ItemToType(std::shared_ptr<Type> t);

    static void is_StrongDerivable(const std::shared_ptr<Type> &T1, const std::shared_ptr<Type> &T0, bool canRemoveMut=true);

    void is_NumDerivable(std::shared_ptr<Type> &T1, std::shared_ptr<Type> &T0);

    void is_AllDerivable(std::shared_ptr<Type> &T1, std::shared_ptr<Type> &T0);

    void visit(Program *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(RustType *node,ASTNode *F,ASTNode *l,ASTNode* f);

    void TypeCheck(ArrayType *node, ASTNode *F, ASTNode *l, ASTNode *f);

    void TypeCheck(BasicType *node, ASTNode *F, ASTNode *l, ASTNode *f);

    void TypeCheck(SelfType *node, ASTNode *F, ASTNode *l, ASTNode *f);

    void TypeCheck(FunctionType *node, ASTNode *F, ASTNode *l, ASTNode *f);

    void TypeCheck(StructType *node, ASTNode *F, ASTNode *l, ASTNode *f);

    void TypeCheck(IdentifierType *node, ASTNode *F, ASTNode *l, ASTNode *f);

    void loadBuiltin(ASTNode *node);

    void Analyze(ASTNode *node);

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

    void visit(UnitExpr *node, ASTNode *F, ASTNode *l, ASTNode *f);

    void visit(EnumStmt *node, ASTNode *F, ASTNode *l, ASTNode *f);

    void visit(InherentImplStmt *node, ASTNode *F, ASTNode *l, ASTNode *f);

    void visit(StructExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(IfExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(WhileExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(FnStmt *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(ContinueExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(BreakExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(ReturnExpr *node,ASTNode* F,ASTNode* l,ASTNode* f);
    static void visit(LetStmt *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(ConstStmt *node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(ArraySimplifiedExpr*node,ASTNode* F,ASTNode* l,ASTNode* f);

    void is_AsTrans(std::shared_ptr<Type> T1, std::shared_ptr<Type> T2);

    void visit(StructStmt*node,ASTNode* F,ASTNode* l,ASTNode* f);
    void visit(AssignmentExpr *node, ASTNode *F, ASTNode *l, ASTNode *f);
    void visit(AsExpr *node, ASTNode *F, ASTNode *l, ASTNode *f);
};







#endif //SEMANTIC_CHECK_H
