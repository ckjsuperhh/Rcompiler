//
// Created by ckjsuperhh6602 on 25-8-9.
//

#ifndef AST_NODE_H
#define AST_NODE_H
#include <cassert>
#include <iostream>
#include <memory>
#include <optional>
#include <utility>
#include <vector>
#include "semantic check.h"
struct Expr;
struct Stmt;
class SemanticCheck;
enum class TypeName;

enum class TypeName {
    Int, Float, Bool, Void,
    BasicType,
    ArrayType,
    LiteralExpr,
    InfiniteLoopExpr,
    BlockExpr,
    ArrayInitExpr,
    ArrayAccessExpr,
    VariableExpr,
    BinaryExpr,
    UnaryExpr,
    PathExpr,
    GroupedExpr,
    TupleExpr,
    TupleIndexExpr,
    FieldAccessExpr,
    CallExpr,
    MethodCallExpr,
    StructExpr,
    IfExpr,
    WhileLoopExpr,
    ExitStmt,
    ForLoopStmt,
    FnStmt,
    WhileLetExpr,
    ContinueExpr,
    BreakExpr,
    RangeExpr,
    ReturnExpr,
    LetStmt,
    MatchExpr,
    BlockStmt,
    IfStmt,
    WhileLoopStmt,
    WhileLetStmt,
    InfiniteLoopStmt,
    UnderscoreExpr,
    Error,
    TupleType,
    StructType,
    ErrorType,
    Uint,
    String,
    FunctionType,
    i32,
    u32,
    isize,
    usize,
    Program,
    Integer,
    ConstStmt,
    AssignmentStmt,
    ArrayAccessType,
    ArraySimplifiedExpr,
    StructStmt,
    EnumStmt,
    TraitStmt,
    TraitImplStmt,
    Char,
    LoopExpr,
    WhileExpr,
};

struct ASTNode {
    TypeName node_type;
    std::string comment;
    virtual ~ASTNode() = default;
    [[nodiscard]] TypeName get_type() const {
        return node_type;
    }
    // virtual void accept1(SemanticCheck *checker) const override {
    //     checker->phase1(this);
    // }
    // virtual void accept2(SemanticCheck *checker) const override {
    //     checker->phase2(this);
    // }
    // virtual void accept3(SemanticCheck *checker) const override {
    //     checker->phase3(this);
    // }
    // virtual void accept4(SemanticCheck *checker) const override {
    //     checker->phase4(this);
    // }
    // virtual void accept5(SemanticCheck *checker) const override {
    //     checker->phase5(this);
    // }
    explicit ASTNode(const TypeName t):node_type(t){}
};

struct Type  :ASTNode {
    ~Type() override = default;
    virtual bool equals(const Type *other)const =0;
    explicit Type(const TypeName t):ASTNode(t){}
};//这样设计直接可以使用ASTNode中的get_type()

struct BasicType : Type {
    TypeName kind;

    explicit BasicType(const TypeName k): Type(TypeName::BasicType), kind(k) {
    }

    bool equals(const Type *other) const override{
        if (auto basic = dynamic_cast<const BasicType *>(other)) {
            if (basic->kind == TypeName::Integer) {
                return kind == TypeName::Integer || kind == TypeName::i32 || kind == TypeName::u32 || kind == TypeName::isize || kind == TypeName::usize;
            }
            if (kind == TypeName::Integer) {
                return basic->kind == TypeName::Integer || basic->kind == TypeName::i32 || basic->kind == TypeName::u32 || basic->kind == TypeName::isize || basic->kind == TypeName::usize;
            }
            return basic->kind == kind;
        }
        return false; //如果dynamic_cast失败，则必定不是相同的类型
    }
    
};

struct ArrayType : Type {
    std::shared_ptr<Type> elementType; // 嵌套的元素类型
    int dimension; // 数组维度（如2表示二维数组）
    int length;

    explicit ArrayType(std::shared_ptr<Type> elementType, const int l, const int dimension = 1)
        : Type(TypeName::ArrayType), elementType(std::move(elementType)), dimension(dimension), length(l) {
        assert(this->elementType != nullptr);
        assert(dimension > 0);
    }

    bool equals(const Type *other) const override{
        if (auto *array = dynamic_cast<const ArrayType *>(other)) {
            return dimension == array->dimension &&
                   elementType->equals(array->elementType.get()) &&
                   length == array->length;
        }
        return false;
    }

    ~ArrayType() override = default;

    
};


struct TupleType:Type {
    std::vector<std::shared_ptr<Type>> elementTypes;
    explicit TupleType(std::vector<std::shared_ptr<Type>> elementTypes)
    :Type(TypeName::TupleType), elementTypes(std::move(elementTypes)) {}
    bool equals(const Type*other)const override {
        if (auto *tuple=dynamic_cast<const TupleType*>(other)) {
            if (elementTypes.size()!=tuple->elementTypes.size()) {
                return false;
            }
            for (size_t i=0;i<elementTypes.size();++i) {
                if (!elementTypes[i]->equals(tuple->elementTypes[i].get())) {
                    return false;
                }
            }
            return true;
        }
        return false;
    }
    
};

struct Field {
    std::string name;
    std::shared_ptr<Type> type;
};

struct StructType:Type {
    std::string name;
    std::vector<Field> fields;
    explicit StructType(std::string n,std::vector<Field> f):Type(TypeName::StructType),name(std::move(n)),fields(std::move(f)){}
    bool equals(const Type* other) const override {
        if (auto* structType = dynamic_cast<const StructType*>(other)) {
            // 结构体类型相等需满足：字段数量相同 + 每个字段的名称和类型对应相同
            if (fields.size() != structType->fields.size()) {
                return false;
            }
            for (size_t i = 0; i < fields.size(); ++i) {
                if (fields[i].name != structType->fields[i].name ||
                    !fields[i].type->equals(structType->fields[i].type.get())) {
                    return false;
                    }
            }
            return true;
        }
        return false;
    }
    
};

struct Param {
    std::string name;
    std::shared_ptr<Type> type;
};

struct FunctionType : Type {
    std::vector<Param> parameters;
    std::shared_ptr<Type> return_type;

    explicit FunctionType(std::vector<Param> p, std::shared_ptr<Type> r): Type(TypeName::FunctionType), parameters(std::move(p)), return_type(std::move(r)) {
    }

    bool equals(const Type *other) const override {
        if (auto *func = dynamic_cast<const FunctionType *>(other)) {
            if (return_type->equals(func->return_type.get()) && parameters.size() == func->parameters.size()) {
                for (size_t i = 0; i < parameters.size(); i++) {
                    if (!parameters[i].type->equals(func->parameters[i].type.get())) {
                        return false;
                    }
                }
                return true;
            }
        }
        return false;
    }
    
};

struct ErrorType : Type {
    explicit ErrorType(): Type(TypeName::ErrorType) {
    }

    bool equals(const Type *other) const override {
        return false;
    }//只要是错误，一定判断失败

    
};

struct Expr : ASTNode {
    std::shared_ptr<Type> exprType; //之后再细化了类型的分类，实际上只有很少量的类型需要用到这个，比如具体化我是int，或者说具体表示某个数组
    explicit Expr(const TypeName t, std::shared_ptr<Type> e=nullptr): ASTNode(t), exprType(std::move(e)) {
    }
    ~Expr() override = default;
};

struct LiteralExpr : Expr {
    std::string value;
    explicit LiteralExpr(std::string v, std::shared_ptr<Type> t) : Expr(TypeName::LiteralExpr, std::move(t)), value(std::move(v)) {
    }
    
};

struct ArrayInitExpr : Expr {
    std::vector<std::shared_ptr<Expr> > elements;
    explicit ArrayInitExpr(std::vector<std::shared_ptr<Expr> > es, std::shared_ptr<Type> t = nullptr):
    Expr(TypeName::ArrayInitExpr, std::move(t)), elements(std::move(es)) {
    }
    
};

struct ArraySimplifiedExpr :Expr{
    std::shared_ptr<Expr> element;
    std::shared_ptr<Expr> length;
    explicit ArraySimplifiedExpr(std::shared_ptr<Expr>e,std::shared_ptr<Expr> length,std::shared_ptr<Type> t = nullptr):
    Expr(TypeName::ArraySimplifiedExpr, std::move(t)), element(std::move(e)) ,length(std::move(length)){
    }
    
};

struct ArrayAccessExpr : Expr {
    std::shared_ptr<Expr> array;
    std::shared_ptr<Expr> index;

    ArrayAccessExpr(std::shared_ptr<Expr> a, std::shared_ptr<Expr> i,std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::ArrayAccessExpr,std::move(t)), array(std::move(a)), index(std::move(i)) {
    }

    
};

struct BinaryExpr : Expr {
    std::shared_ptr<Expr> left;
    std::string op;
    std::shared_ptr<Expr> right;

    BinaryExpr(std::shared_ptr<Expr> left, std::string op, std::shared_ptr<Expr> right,std::shared_ptr<Type> t)
        : Expr(TypeName::BinaryExpr,std::move(t)), left(std::move(left)), op(std::move(op)), right(std::move(right)) {
    }
    
};

struct UnaryExpr : Expr {
    std::string op;
    std::shared_ptr<Expr> right;

    UnaryExpr(std::string op, std::shared_ptr<Expr> right,std::shared_ptr<Type> t=nullptr)
        : Expr(TypeName::UnaryExpr,std::move(t)), op(std::move(op)), right(std::move(right)) {
    }
};

struct PathExpr : Expr {
    std::vector<std::string> segments;
    explicit PathExpr(std::vector<std::string> s, std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::PathExpr, std::move(t)), segments(std::move(s)) {
    }
    
};

     struct CallExpr : Expr {
         std::shared_ptr<Expr> receiver;
         std::vector<std::shared_ptr<Expr> > arguments;

         CallExpr(std::shared_ptr<Expr> r, std::vector<std::shared_ptr<Expr> > a,std::shared_ptr<Type> t=nullptr)
             : Expr(TypeName::CallExpr,std::move(t)), receiver(std::move(r)), arguments(std::move(a)) {
         }
     };

struct FieldAccessExpr : Expr {
    std::shared_ptr<Expr> struct_name;
    std::shared_ptr<Expr> field_expr;
    explicit FieldAccessExpr(std::shared_ptr<Expr> s, std::shared_ptr<Expr> f,std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::FieldAccessExpr,std::move(t)),struct_name(std::move(s)), field_expr(std::move(f)){}
};

struct BlockExpr : Expr {
    std::vector<std::pair<std::shared_ptr<ASTNode> ,bool>> statements;
    explicit BlockExpr(std::vector<std::pair<std::shared_ptr<ASTNode> ,bool>> s, std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::BlockExpr,std::move(t)),statements(std::move(s)){}
};

struct IfExpr: Expr {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Expr> then_branch;
    std::shared_ptr<Expr> else_branch; //可能是block，也可能是if
    IfExpr(std::shared_ptr<Expr> c, std::shared_ptr<Expr> t, std::shared_ptr<Expr> e, std::shared_ptr<Type> ty=nullptr):
    Expr(TypeName::IfStmt,std::move(ty)), condition(std::move(c)), then_branch(std::move(t)), else_branch(std::move(e)) {
    }
};

struct ReturnExpr : Expr {
    std::shared_ptr<Expr> expr;
    explicit ReturnExpr(std::shared_ptr<Expr> e=nullptr,std::shared_ptr<Type> t=nullptr) :
    Expr(TypeName::ReturnExpr,std::move(t)),expr(std::move(e)) {}
};

struct GroupedExpr : Expr {
    std::shared_ptr<Expr> expr;
    explicit GroupedExpr(std::shared_ptr<Expr> e, std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::GroupedExpr,std::move(t)), expr(std::move(e)) {}
};

struct AssignmentExpr : Expr {
    std::shared_ptr<Expr> left;
    std::shared_ptr<Expr> right;
    AssignmentExpr(std::shared_ptr<Expr> l,std::shared_ptr<Expr> r):
    Expr(TypeName::AssignmentStmt), left(std::move(l)), right(std::move(r)) {}
};

struct ContinueExpr : Expr {
    ContinueExpr() : Expr(TypeName::ContinueExpr, nullptr) {}
};

struct BreakExpr : Expr {
    std::shared_ptr<Expr> expr;
    BreakExpr() : Expr(TypeName::BreakExpr, nullptr) {}
    explicit BreakExpr(std::shared_ptr<Expr> e,std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::BreakExpr, std::move(t)),expr(std::move(e)){}
};

struct LoopExpr : Expr {
    std::shared_ptr<Expr> block;
    explicit LoopExpr(std::shared_ptr<Expr> b, std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::LoopExpr, std::move(t)), block(std::move(b)) {}
};

struct WhileExpr : Expr {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Expr> block;
    WhileExpr(std::shared_ptr<Expr> c, std::shared_ptr<Expr> b, std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::WhileExpr, std::move(t)),condition(std::move(c)), block(std::move(b)) {}
};

struct Stmt : ASTNode {
    explicit Stmt(const TypeName t): ASTNode(t) {
    }
    ~Stmt() override =default;
};

struct ConstStmt : Stmt {
    std::string identifier;
    std::shared_ptr<Type> type;
    std::shared_ptr<Expr> expr;
    ConstStmt(std::string id, std::shared_ptr<Type> t, std::shared_ptr<Expr> e): Stmt(TypeName::ConstStmt), identifier(std::move(id)), type(std::move(t)), expr(std::move(e)) {
    }
};

struct FnStmt : Stmt {
    std::string name;
    std::vector<Param> parameters;
    std::shared_ptr<Type> return_type;
    std::shared_ptr<BlockStmt> body;
    FnStmt(std::string n, std::vector<Param> p, std::shared_ptr<Type> r, std::shared_ptr<BlockStmt> b): Stmt(TypeName::FnStmt), name(std::move(n)), parameters(std::move(p)), return_type(std::move(r)), body(std::move(b)) {
    }
    FnStmt(std::string n, std::vector<Param> p, std::shared_ptr<BlockStmt> b): Stmt(TypeName::FnStmt), name(std::move(n)), parameters(std::move(p)), body(std::move(b)) {
    }
    ~FnStmt() override = default;
};

struct StructStmt:Stmt {
    std::string name;
    std::vector<Field> fields;
    StructStmt(std::string n,std::vector<Field>f): Stmt(TypeName::StructStmt), name(std::move(n)), fields(std::move(f)) {
    }
    
};

struct EnumStmt:Stmt{
    std::string enum_name;
    std::vector<std::string> ids;
    EnumStmt(std::string n,std::vector<std::string> i): Stmt(TypeName::EnumStmt), enum_name(std::move(n)), ids(std::move(i)) {
    }
    
};

struct TraitStmt:Stmt {
    std::vector<FnStmt> fns;
    std::vector<ConstStmt> cons;
    TraitStmt(std::vector<FnStmt> f,std::vector<ConstStmt> c):Stmt(TypeName::TraitStmt),fns(std::move(f)),cons(std::move(c)){}
    
};

//区分固有实现还是特征实现
struct InherentImplStmt:Stmt {
    std::vector<FnStmt> fns;
    explicit InherentImplStmt(std::vector<FnStmt> f):Stmt(TypeName::TraitImplStmt),fns(std::move(f)){}
    
};

struct TraitImplStmt:Stmt {
    std::string struct_name;
    std::vector<FnStmt> fns;
    TraitImplStmt(std::vector<FnStmt> f,std::string n):Stmt(TypeName::TraitImplStmt),fns(std::move(f)),struct_name(std::move(n)){}
    
};

struct LetStmt : Stmt {
    bool is_mutable;
    std::string name;
    std::shared_ptr<Type> type_annotation;
    std::shared_ptr<Expr> expr;
    LetStmt(const bool m, std::string n, std::shared_ptr<Type> t, std::shared_ptr<Expr> e)
        : Stmt(TypeName::LetStmt), is_mutable(m), name(std::move(n)), type_annotation(std::move(t)), expr(std::move(e)) {
    }
    ~LetStmt() override = default;

    
};




#endif //AST_NODE_H
