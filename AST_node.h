//
// Created by ckjsuperhh6602 on 25-8-9.
//

#ifndef AST_NODE_H
#define AST_NODE_H
#include <any>
#include <cassert>
#include <iostream>
#include <memory>
#include <optional>
#include <utility>
#include <variant>
#include <vector>
#include "semantic_check.h"
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
    InherentImplStmt,
    unit,
    UnitExpr,
    IdentifierType,
    selfType,
    SelfType,
    AsExpr,
    AssignmentExpr,
};



using Element = std::variant<ASTNode*, std::string, int>;

struct ASTNode {
    TypeName node_type;
    virtual ~ASTNode() = default;
    [[nodiscard]] TypeName get_type() const {
        return node_type;
    }
    std::shared_ptr<Type> realType;
    std::any eval;
    std::pair<SymbolTable*, ASTNode*> scope;
    int Id=0;
    bool is_hasBreak=false, is_hasReturn=false;
    bool isMutable=false;
    virtual void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) =0;
    [[nodiscard]] virtual std::vector<std::string> showSelf() const;
    [[nodiscard]] virtual std::vector<std::string> showTree(int depth , bool is_last) const ;
    [[nodiscard]] virtual std::vector<Element> get_children() const=0;
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
    bool is_mutable=false;
    bool is_and=false;
    ~Type() override = default;
    virtual bool equals(const Type *other)const =0;
    explicit Type(const TypeName t):ASTNode(t){}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override =0;
    [[nodiscard]] std::vector<Element> get_children() const override=0;
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
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct ArrayType : Type {
    std::shared_ptr<Type> elementType; // 嵌套的元素类型
    int dimension; // 数组维度（如2表示二维数组）
    std::shared_ptr<Expr> length;
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    explicit ArrayType(std::shared_ptr<Type> elementType, std::shared_ptr<Expr> l, const int dimension = 1)
        : Type(TypeName::ArrayType), elementType(std::move(elementType)), dimension(dimension), length(std::move(l)) {
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

    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct Field {
    std::string name;
    std::shared_ptr<Type> type;
    Field(std::string  n, std::shared_ptr<Type> t) : name(std::move(n)), type(std::move(t)) {}
};

struct IdentifierType : Type {//包含了单纯的struct,enum等信息
    std::string name;
    explicit IdentifierType(std::string name) :
    Type(TypeName::IdentifierType), name(std::move(name) ){}
    bool equals(const Type *other) const override{
        return false;
    }
    
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct SelfType:Type {
    SelfType():Type(TypeName::SelfType){}
    bool equals(const Type *other) const override{
        return false;
    }
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct Param {
    std::string name;
    std::shared_ptr<Type> type;
};

struct FunctionType : Type {
    std::vector<Param> parameters;
    std::shared_ptr<Type> return_type;
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
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
    
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct ErrorType : Type {
    explicit ErrorType(): Type(TypeName::ErrorType) {
    }
    bool equals(const Type *other) const override {
        return false;
    }//只要是错误，一定判断失败
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct Expr : ASTNode {
    std::shared_ptr<Type> exprType; //之后再细化了类型的分类，实际上只有很少量的类型需要用到这个，比如具体化我是int，或者说具体表示某个数组
    explicit Expr(const TypeName t, std::shared_ptr<Type> e=nullptr): ASTNode(t), exprType(std::move(e)) {
    }
    ~Expr() override = default;
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    [[nodiscard]] std::vector<Element> get_children() const override=0;
};

struct LiteralExpr : Expr {
    std::string value;
    explicit LiteralExpr(std::string v, std::shared_ptr<Type> t) : Expr(TypeName::LiteralExpr, std::move(t)), value(std::move(v)) {
    }
    
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct application {
    std::shared_ptr<Expr> name;
    std::shared_ptr<Expr> variable;
};

struct StructExpr : Expr {
    std::shared_ptr<Expr> structname;
    std::vector<application> apps;
    StructExpr(std::shared_ptr<Expr> s,std::vector<application>a,std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::StructExpr,std::move(t)),structname(s),apps(std::move(a)) {}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }

    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct ArrayInitExpr : Expr {
    std::vector<std::shared_ptr<Expr> > elements;
    explicit ArrayInitExpr(std::vector<std::shared_ptr<Expr> > es, std::shared_ptr<Type> t = nullptr):
    Expr(TypeName::ArrayInitExpr, std::move(t)), elements(std::move(es)) {
    }
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct ArraySimplifiedExpr :Expr{
    std::shared_ptr<Expr> element;
    std::shared_ptr<Expr> length;
    explicit ArraySimplifiedExpr(std::shared_ptr<Expr>e,std::shared_ptr<Expr> length,std::shared_ptr<Type> t = nullptr):
    Expr(TypeName::ArraySimplifiedExpr, std::move(t)), element(std::move(e)) ,length(std::move(length)){
    }
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct ArrayAccessExpr : Expr {
    std::shared_ptr<Expr> array;
    std::shared_ptr<Expr> index;
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    ArrayAccessExpr(std::shared_ptr<Expr> a, std::shared_ptr<Expr> i,std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::ArrayAccessExpr,std::move(t)), array(std::move(a)), index(std::move(i)) {
    }
    
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct UnitExpr : Expr {
    UnitExpr():Expr(TypeName::UnitExpr){}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct AsExpr : Expr {
    std::shared_ptr<Expr> expr;
    std::shared_ptr<Type> type;
    AsExpr(std::shared_ptr<Expr> e, std::shared_ptr<Type> t,std::shared_ptr<Type> ty=nullptr):
    Expr(TypeName::AsExpr,std::move(ty)),expr(std::move(e)), type(std::move(t)) {
    }
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }


    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct BinaryExpr : Expr {
    std::shared_ptr<Expr> left;
    std::string op;
    std::shared_ptr<Expr> right;

    BinaryExpr(std::shared_ptr<Expr> left, std::string op, std::shared_ptr<Expr> right,std::shared_ptr<Type> t=nullptr)
        : Expr(TypeName::BinaryExpr,std::move(t)), left(std::move(left)), op(std::move(op)), right(std::move(right)) {
    }
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct UnaryExpr : Expr {
    std::string op;
    std::shared_ptr<Expr> right;

    UnaryExpr(std::string op, std::shared_ptr<Expr> right,std::shared_ptr<Type> t=nullptr)
        : Expr(TypeName::UnaryExpr,std::move(t)), op(std::move(op)), right(std::move(right)) {
    }
    
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct PathExpr : Expr {
    std::vector<std::string> segments;
    explicit PathExpr(std::vector<std::string> s, std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::PathExpr, std::move(t)), segments(std::move(s)) {
    }
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

     struct CallExpr : Expr {
         std::shared_ptr<Expr> receiver;
         std::vector<std::shared_ptr<Expr> > arguments;

         CallExpr(std::shared_ptr<Expr> r, std::vector<std::shared_ptr<Expr> > a,std::shared_ptr<Type> t=nullptr)
             : Expr(TypeName::CallExpr,std::move(t)), receiver(std::move(r)), arguments(std::move(a)) {
         }
         void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
             return visitor.visit(this,F,l,f);
         }
         
    [[nodiscard]] std::vector<Element> get_children() const override;
     };

struct FieldAccessExpr : Expr {
    std::shared_ptr<Expr> struct_name;
    std::shared_ptr<Expr> field_expr;
    explicit FieldAccessExpr(std::shared_ptr<Expr> s, std::shared_ptr<Expr> f,std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::FieldAccessExpr,std::move(t)),struct_name(std::move(s)), field_expr(std::move(f)){}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct BlockExpr : Expr {
    std::vector<std::pair<std::shared_ptr<ASTNode> ,bool>> statements;
    explicit BlockExpr(std::vector<std::pair<std::shared_ptr<ASTNode> ,bool>> s, std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::BlockExpr,std::move(t)),statements(std::move(s)){}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct IfExpr: Expr {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Expr> then_branch;
    std::shared_ptr<Expr> else_branch; //可能是block，也可能是if
    IfExpr(std::shared_ptr<Expr> c, std::shared_ptr<Expr> t, std::shared_ptr<Expr> e, std::shared_ptr<Type> ty=nullptr):
    Expr(TypeName::IfStmt,std::move(ty)), condition(std::move(c)), then_branch(std::move(t)), else_branch(std::move(e)) {
    }
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct ReturnExpr : Expr {
    std::shared_ptr<Expr> expr;
    explicit ReturnExpr(std::shared_ptr<Expr> e=nullptr,std::shared_ptr<Type> t=nullptr) :
    Expr(TypeName::ReturnExpr,std::move(t)),expr(std::move(e)) {}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct GroupedExpr : Expr {
    std::shared_ptr<Expr> expr;
    explicit GroupedExpr(std::shared_ptr<Expr> e, std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::GroupedExpr,std::move(t)), expr(std::move(e)) {}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct AssignmentExpr : Expr {
    std::shared_ptr<Expr> left;
    std::shared_ptr<Expr> right;
    std::string op;
    AssignmentExpr(std::shared_ptr<Expr> l,std::string o,std::shared_ptr<Expr> r):
    Expr(TypeName::AssignmentStmt), left(std::move(l)),op(std::move(o)), right(std::move(r)) {}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct ContinueExpr : Expr {
    ContinueExpr() : Expr(TypeName::ContinueExpr, nullptr) {}
    
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct BreakExpr : Expr {
    std::shared_ptr<Expr> expr;
    BreakExpr() : Expr(TypeName::BreakExpr, nullptr) {}
    explicit BreakExpr(std::shared_ptr<Expr> e,std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::BreakExpr, std::move(t)),expr(std::move(e)){}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct LoopExpr : Expr {
    std::shared_ptr<Expr> block;
    explicit LoopExpr(std::shared_ptr<Expr> b, std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::LoopExpr, std::move(t)), block(std::move(b)) {}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct WhileExpr : Expr {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Expr> block;
    WhileExpr(std::shared_ptr<Expr> c, std::shared_ptr<Expr> b, std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::WhileExpr, std::move(t)),condition(std::move(c)), block(std::move(b)) {}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct Stmt : ASTNode {
    explicit Stmt(const TypeName t): ASTNode(t) {
    }
    ~Stmt() override =default;
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override =0;
    [[nodiscard]] std::vector<Element> get_children() const override=0;
};

struct ConstStmt : Stmt {
    std::string identifier;
    std::shared_ptr<Type> type;
    std::shared_ptr<Expr> expr;
    ConstStmt(std::string id, std::shared_ptr<Type> t, std::shared_ptr<Expr> e): Stmt(TypeName::ConstStmt), identifier(std::move(id)), type(std::move(t)), expr(std::move(e)) {
    }
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct FnStmt : Stmt {
    std::string name;
    std::vector<Param> parameters;
    std::shared_ptr<Type> return_type;
    std::shared_ptr<Expr> body;
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    FnStmt(std::string n, std::vector<Param> p, std::shared_ptr<Type> r, std::shared_ptr<Expr> b): Stmt(TypeName::FnStmt), name(std::move(n)), parameters(std::move(p)), return_type(std::move(r)), body(std::move(b)) {
    }
    ~FnStmt() override = default;
    
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct StructStmt:Stmt {
    std::string name;
    std::vector<Field> fields;
    StructStmt(std::string n,std::vector<Field>f): Stmt(TypeName::StructStmt), name(std::move(n)), fields(std::move(f)) {
    }
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct EnumStmt:Stmt{
    std::string enum_name;
    std::vector<std::string> ids;
    EnumStmt(std::string n,std::vector<std::string> i): Stmt(TypeName::EnumStmt), enum_name(std::move(n)), ids(std::move(i)) {
    }
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct TraitStmt:Stmt {
    std::string name;
    std::vector<std::pair<std::shared_ptr<FnStmt>,bool> > fns;
    std::vector<std::pair<std::shared_ptr<ConstStmt>,bool>> cons;
    TraitStmt(std::string n,std::vector<std::pair<std::shared_ptr<FnStmt>,bool> >f,std::vector<std::pair<std::shared_ptr<ConstStmt>,bool>> c):
    Stmt(TypeName::TraitStmt),name(std::move(n)),fns(std::move(f)),cons(std::move(c)){}

    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    [[nodiscard]] std::vector<Element> get_children() const override;
};

//区分固有实现还是特征实现
struct InherentImplStmt:Stmt {
    std::string name;
    std::vector<std::pair<std::shared_ptr<FnStmt>,bool> > fns;
    std::vector<std::pair<std::shared_ptr<ConstStmt>,bool>> cons;
    explicit InherentImplStmt(std::string n,std::vector<std::pair<std::shared_ptr<FnStmt>,bool> >f,std::vector<std::pair<std::shared_ptr<ConstStmt>,bool>> c):
    Stmt(TypeName::InherentImplStmt),name(std::move(n)),fns(std::move(f)),cons(std::move(c)){}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct TraitImplStmt:Stmt {
    std::string trait_name;
    std::string struct_name;
    std::vector<std::pair<std::shared_ptr<FnStmt>,bool> > fns;
    std::vector<std::pair<std::shared_ptr<ConstStmt>,bool>> cons;
    TraitImplStmt(std::string t,std::string s,std::vector<std::pair<std::shared_ptr<FnStmt>,bool> >f,std::vector<std::pair<std::shared_ptr<ConstStmt>,bool>> c)
        : Stmt(TypeName::TraitImplStmt),trait_name(std::move(t)), struct_name(std::move(s)), fns(std::move(f)),cons(std::move(c)) {}


    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct LetStmt:Stmt {
    std::string identifier;
    bool is_mutable;
    std::shared_ptr<Type> type;
    std::shared_ptr<Expr> expr;
    LetStmt(std::string i,std::shared_ptr<Type> t,std::shared_ptr<Expr> e, const bool m=false)
        : Stmt(TypeName::LetStmt), identifier(std::move(i)),type(std::move(t)), is_mutable(m), expr(std::move(e)) {
    }
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct Program : ASTNode {
    std::vector<std::shared_ptr<ConstStmt>> cons;
    std::vector<std::shared_ptr<FnStmt>> fns;
    std::vector<std::shared_ptr<EnumStmt>> enums;
    std::vector<std::shared_ptr<StructStmt>> structs;
    std::vector<std::shared_ptr<InherentImplStmt>> inherits;
    std::vector<std::shared_ptr<TraitStmt>> traits;
    std::vector<std::shared_ptr<TraitImplStmt>> impls;
    //其余的全局变量\enum\trait\impl等等我都先不管
    explicit Program(std::vector<std::shared_ptr<ConstStmt>> c,std::vector<std::shared_ptr<FnStmt>> f,std::vector<std::shared_ptr<EnumStmt>> e,
        std::vector<std::shared_ptr<StructStmt>> s,std::vector<std::shared_ptr<InherentImplStmt>> in,
        std::vector<std::shared_ptr<TraitStmt>> t,std::vector<std::shared_ptr<TraitImplStmt>> im):
    ASTNode(TypeName::Program),cons(std::move(c)),fns(std::move(f)),enums(std::move(e)),structs(std::move(s)),inherits(std::move(in)),traits(std::move(t)),impls(std::move(im)){}
    ~Program() override = default;
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override {
        return visitor.visit(this,F,l,f);
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};
#endif //AST_NODE_H
