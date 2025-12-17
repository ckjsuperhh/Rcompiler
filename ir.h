//
// Created by ckjsuperhh6602 on 2025/11/12.
//

#ifndef COMPILER_IR_H
#define COMPILER_IR_H

#include <any>
#include <vector>
#include "AST_Node.h"
#include "semantic_check.h"

struct ASTNode;
struct Type;
struct Param;
struct Expr;
struct Stmt;
struct TypeType;
struct BasicType;
struct ArrayType;
struct Field;
struct IdentifierType;
struct SelfType;
struct FunctionType;
struct ErrorType;
struct AndStrType;
struct UnitType;
struct NeverType;
struct EnumType;
struct ReferenceType;
struct StructType;
struct RustType;
struct LiteralExpr;
struct ArrayInitExpr;
struct ArraySimplifiedExpr;
struct ArrayAccessExpr;
struct UnitExpr;
struct BinaryExpr;
struct UnderscoreExpr;
struct UnaryExpr;
struct PathExpr;
struct CallExpr;
struct FieldAccessExpr;
struct BlockExpr;
struct IfExpr;
struct ReturnExpr;
struct GroupedExpr;
struct AssignmentExpr;
struct application;
struct StructExpr;
struct AsExpr;
struct ContinueExpr;
struct BreakExpr;
struct LoopExpr;
struct WhileExpr;
struct ConstStmt;
struct FnStmt;
struct StructStmt;
struct EnumStmt;
struct InherentImplStmt;
struct LetStmt;
struct Program;

class IRgen {
    public:

    void pre_process(ASTNode* Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(LiteralExpr *Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(LoopExpr *Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(BlockExpr *Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(ArrayInitExpr *Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(ArrayAccessExpr *Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(BinaryExpr *Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(UnaryExpr *Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(PathExpr *Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(GroupedExpr *Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(FieldAccessExpr *Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(CallExpr *Node,ASTNode* r,ASTNode* LastBlock);

    std::vector<std::string> generateIr(ASTNode *root);

    void generateIr(UnitExpr *Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(EnumStmt *Node, ASTNode* r,ASTNode* LastBlock);
    void generateIr(InherentImplStmt *Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(StructExpr *Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(IfExpr *Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(WhileExpr *Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(FnStmt *Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(ContinueExpr *Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(BreakExpr *Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(ReturnExpr *Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(LetStmt *Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(ConstStmt *Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(ArraySimplifiedExpr*Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(RustType*Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(Program*Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(StructStmt*Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(AssignmentExpr*Node,ASTNode* r,ASTNode* LastBlock);
    void generateIr(AsExpr*Node,ASTNode* r,ASTNode* LastBlock);
};


#endif //COMPILER_IR_H