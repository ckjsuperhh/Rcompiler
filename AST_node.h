//
// Created by ckjsuperhh6602 on 25-8-9.
//

#ifndef AST_NODE_H
#define AST_NODE_H
#include <any>
#include <cassert>
#include <iostream>
#include <memory>
#include <utility>
#include <variant>
#include <vector>

#include "ir.h"
#include "semantic_check.h"
struct ASTNode;
struct Expr;
struct Stmt;
class SemanticCheck;
class IRgen;
enum class TypeName;
inline int variableNum=0;
enum class TypeName {
    Float, Bool, Void,
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
    Iint,
    Uint,
    AndStrType,
    String,
    FunctionType,
    I32,
    U32,
    Isize,
    Usize,
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
    Unit,
    UnitExpr,
    IdentifierType,
    selfType,
    SelfType,
    AsExpr,
    AssignmentExpr,
    Type,
    RustType,
    UnitType,
    TypeType,
    NeverType,
    Versatile,
    VersatileType,
    Int,
    EnumType,
    ReferenceType,
};

struct Type;
struct SymbolTable;

using Element = std::variant<ASTNode*,Type*, std::string, int>;

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
    bool is_hasAbsoluteBreak=false;
    bool isMutable=false;
    bool isVariable=false;
    ASTNode* LoopPtr=nullptr;
    ASTNode* FnPtr=nullptr;
    unsigned long long variableID = 0;
    std::string irRes;
    std::string irRes_p;
    std::string irLabel1;
    std::string irLabel2;
    std::vector<std::any> irCode;
    std::vector<std::any> irCodeStruct;
    virtual void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) =0;
    [[nodiscard]] virtual std::vector<std::string> showSelf() const;
    [[nodiscard]] virtual std::vector<std::string> showTree(int depth , bool is_last) const ;
    [[nodiscard]] virtual std::vector<Element> get_children() const=0;
    explicit ASTNode(const TypeName t,std::shared_ptr<Type> ty=nullptr):node_type(t),realType(std::move(ty)){}
    virtual void IR(IRgen &visitor, ASTNode *r, ASTNode *l)=0;
};

struct Type  {
    TypeName typeKind;
    std::shared_ptr<Type> typePtr;
    bool is_mutable=false;
    bool is_and=false;
    bool is_const=false;
    std::shared_ptr<ConstStmt> Const=nullptr;

    virtual ~Type() = default;
    virtual bool equals(const Type *other)const =0;
    explicit Type(const TypeName t,std::shared_ptr<Type> Ptr=nullptr):typeKind(t),typePtr(std::move(Ptr)){}
    [[nodiscard]] virtual std::vector<Element> get_children() const =0;
    [[nodiscard]] virtual std::string toString() const = 0;
    virtual void accept(SemanticCheck&,ASTNode*,ASTNode*,ASTNode*,std::pair<SymbolTable*,ASTNode*>)=0;
    virtual void IR(IRgen &visitor, ASTNode *r, ASTNode *l) =0;
};//这样设计直接可以使用ASTNode中的get_type()

struct TypeType:Type {
    [[nodiscard]] std::vector<Element> get_children() const override {
        std::vector<Element> res;
        res.emplace_back("TypeType:");
        res.emplace_back(this->typePtr.get());
        return res;
    }
    explicit TypeType(const std::shared_ptr<Type> &t=nullptr,bool is_mut=false):Type(TypeName::TypeType,t) {
        this->is_mutable=is_mut;
    }
    [[nodiscard]] std::string toString() const override {
        return "Type"; // 表示“Type 类型”
    }
    void accept(SemanticCheck& visitor,ASTNode* F,ASTNode* l,ASTNode* f,std::pair<SymbolTable*,ASTNode*>) override ;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override {

    };
    bool equals(const Type *other) const override {
        if (other->typeKind!=this->typeKind) {
            return false;
        }
        auto other_type = dynamic_cast<const TypeType*>(other);
        return other_type->typePtr->equals(this->typePtr.get());
    }
};

struct BasicType : Type {
    TypeName kind;

    explicit BasicType(const TypeName k): Type(TypeName::BasicType), kind(k) {
    }

    bool equals(const Type *other) const override{
        if (auto basic = dynamic_cast<const BasicType *>(other)) {
            if (basic->kind == TypeName::Int) {
                return kind == TypeName::Int || kind == TypeName::I32 || kind == TypeName::U32 ||
                    kind == TypeName::Isize || kind == TypeName::Usize;
            }
            if (kind == TypeName::Int) {
                return basic->kind == TypeName::Int || basic->kind == TypeName::I32 ||
                    basic->kind == TypeName::U32 || basic->kind == TypeName::Isize || basic->kind == TypeName::Usize;
            }
            return basic->kind == kind;
        }
        return false; //如果dynamic_cast失败，则必定不是相同的类型
    }
    void accept(SemanticCheck& visitor,ASTNode* F,ASTNode* l,ASTNode* f,std::pair<SymbolTable*,ASTNode*> scope) override ;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override {

    };
    [[nodiscard]] std::string toString() const override {
        // 需根据你的 TypeName 枚举值调整，确保与实际定义匹配
        switch(kind) {
            case TypeName::Integer: return "int";
            case TypeName::I32: return "i32";
            case TypeName::U32: return "u32";
            case TypeName::Isize: return "isize";
            case TypeName::Usize: return "usize";
            case TypeName::Bool: return "bool";
            case TypeName::Char: return "char";
            case TypeName::String: return "String";
            default: return "BasicType(Unknown)"; // 兜底，避免未定义枚举
        }
    }
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct ArrayType : Type {
    int dimension; // 数组维度（如2表示二维数组）
    std::shared_ptr<Expr> length;
    explicit ArrayType(std::shared_ptr<Type> elementType, std::shared_ptr<Expr> l, const int dimension = 1):
    Type(TypeName::ArrayType),  dimension(dimension), length(std::move(l)) {
        this->typePtr=std::move(elementType);
        assert(this->typePtr != nullptr);
        assert(dimension > 0);
    }
    void accept(SemanticCheck& visitor,ASTNode* F,ASTNode* l,ASTNode* f,std::pair<SymbolTable*,ASTNode*>) override ;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override {

    };
    bool equals(const Type *other) const override{
        if (auto *array = dynamic_cast<const ArrayType *>(other)) {
            return dimension == array->dimension &&
                   typePtr->equals(array->typePtr.get()) &&
                   length == array->length;
        }
        return false;
    }
    // 实现 toString：格式为“[元素类型; 长度]”（多维数组递归展开）
    [[nodiscard]] std::string toString() const override {
        // 处理空指针（防御性编程）
        if (!typePtr) return "[nullptr; ?]";

        // 简化：假设 length 是常量（若需支持变量，需 Expr 加 toString）
        std::string len_str = "?";
        // if (length) len_str = length->toString(); // 若 Expr 实现了 toString，可启用

        // 递归生成多维数组字符串（如二维数组："[[i32; 5]; 3]"）
        std::string base = "[" + typePtr->toString() + "; " + len_str + "]";
        if (dimension > 1) {
            for (int i = 2; i <= dimension; ++i) {
                base = "[" + base + "; " + len_str + "]";
            }
        }
        return base;
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
    // 实现 toString：直接返回自定义类型名
    [[nodiscard]] std::string toString() const override {
        return name; // 如 "Person"、"Color"
    }
    void accept(SemanticCheck& visitor,ASTNode* F,ASTNode* l,ASTNode* f,std::pair<SymbolTable*,ASTNode*>) override ;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override {

    };
    [[nodiscard]] std::vector<Element> get_children() const override ;
};

struct SelfType:Type {
    SelfType():Type(TypeName::SelfType){}
    bool equals(const Type *other) const override {
        return false;
    }
    // bool equals(const Type *other) const override{
    //     return false;
    // }
    // 实现 toString：返回 "Self"（符合 Rust 等语言的自引用习惯）
    [[nodiscard]] std::string toString() const override {
        return "Self";
    }
    [[nodiscard]] std::vector<Element> get_children() const override ;
    void accept(SemanticCheck& visitor,ASTNode* F,ASTNode* l,ASTNode* f,std::pair<SymbolTable*,ASTNode*>) override ;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override {

    }
};

struct Param {
    std::string name="";
    std::shared_ptr<Type> type=nullptr;
    int varnum=-1;
};

struct FunctionType : Type {
    int funcnum;
    std::string name;
    std::vector<Param> parameters;
    std::shared_ptr<SelfType> selfType;
    std::shared_ptr<Type> return_type;
    explicit FunctionType(std::vector<Param> p, std::shared_ptr<Type> r,int func,std::string name="",std::shared_ptr<SelfType> s=nullptr):
        Type(TypeName::FunctionType), parameters(std::move(p)),funcnum(func), return_type(std::move(r)),selfType(std::move(s)) ,name(std::move(name)){
    }
    void accept(SemanticCheck& visitor,ASTNode* F,ASTNode* l,ASTNode* f,std::pair<SymbolTable*,ASTNode*>) override ;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override {

    };
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
    // 实现 toString：格式为“(参数1类型, 参数2类型) -> 返回值类型”
    [[nodiscard]] std::string toString() const override {
        // 处理返回值空指针
        std::string ret_str = "nullptr";
        if (return_type) ret_str = return_type->toString();

        // 拼接参数类型（如 "i32, bool"）
        std::string params_str;
        for (size_t i = 0; i < parameters.size(); ++i) {
            if (i > 0) params_str += ", ";
            // 若需显示参数名：params_str += parameters[i].name + ": ";
            params_str += parameters[i].type->toString();
        }

        // 处理 Self 参数（如成员方法："&Self, i32"）
        if (selfType) {
            params_str = (is_and ? "&" : "") + selfType->toString() +
                        (params_str.empty() ? "" : ", " + params_str);
        }

        // 最终函数类型字符串
        return "(" + params_str + ") -> " + ret_str;
    }
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};

struct ErrorType : Type {
    explicit ErrorType(): Type(TypeName::ErrorType) {
    }
    bool equals(const Type *other) const override {
        return false;
    }//只要是错误，一定判断失败
    [[nodiscard]] std::vector<Element> get_children() const override;
    void accept(SemanticCheck& visitor,ASTNode* F,ASTNode* l,ASTNode* f,std::pair<SymbolTable*,ASTNode*>) override ;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override {

    };
    // 实现 toString：明确标识错误类型
    [[nodiscard]] std::string toString() const override {
        return "ErrorType";
    }
};

struct AndStrType:Type {
      explicit AndStrType():Type(TypeName::AndStrType){}
    bool equals(const Type *other) const override {
          return other->typeKind==TypeName::AndStrType;
      }
    [[nodiscard]] std::string toString() const override {
          return "&str";
      }
    [[nodiscard]] std::vector<Element> get_children() const override;
    void accept(SemanticCheck& visitor,ASTNode* F,ASTNode* l,ASTNode* f,std::pair<SymbolTable*,ASTNode*>) override ;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override {

    };
};


struct UnitType : Type {
    explicit UnitType(): Type(TypeName::UnitType) {}
    bool equals(const Type *other) const override {
        return false;
    }
    // 新增：实现 get_children（原代码可能遗漏，需补全）
    [[nodiscard]] std::vector<Element> get_children() const override {
        return {};
    }
    void accept(SemanticCheck& visitor,ASTNode* F,ASTNode* l,ASTNode* f,std::pair<SymbolTable*,ASTNode*>) override ;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override {

    };
    // 实现 toString：返回单元类型标识（符合 Rust 习惯）
    [[nodiscard]] std::string toString() const override {
        return "()";
    }
};

struct NeverType : Type {
    explicit NeverType(): Type(TypeName::NeverType) {}
    bool equals(const Type *other) const override {
        return true;
    }
    // 新增：实现 get_children（原代码可能遗漏，需补全）
    [[nodiscard]] std::vector<Element> get_children() const override {
        return {};
    }

    // 实现 toString：返回永不类型标识（符合 Rust 习惯）
    [[nodiscard]] std::string toString() const override {
        return "!";
    }
    void accept(SemanticCheck& visitor,ASTNode* F,ASTNode* l,ASTNode* f,std::pair<SymbolTable*,ASTNode*>) override ;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override {

    };
};

struct EnumType : Type {
    int structID;
    std::string structName;
    std::unordered_map<std::string, unsigned int>* MemberNames;
    EnumType(int structID,
    std::string structName,
    std::unordered_map<std::string, unsigned int>* MemberNames):Type(TypeName::EnumType),structID(structID),structName(std::move(structName)),MemberNames(MemberNames){}
    // EnumType的三个纯虚函数实现
    bool equals(const Type* other) const override {
        // 先判断是否为EnumType类型
        if (const auto* other_enum = dynamic_cast<const EnumType*>(other)) {
            // 枚举类型相等的条件：结构ID相同且成员表相同
            return structID == other_enum->structID &&
                   *MemberNames == *other_enum->MemberNames;
        }
        return false;
    }
    void accept(SemanticCheck& visitor,ASTNode* F,ASTNode* l,ASTNode* f,std::pair<SymbolTable*,ASTNode*>) override ;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override {

    };
    [[nodiscard]] std::vector<Element> get_children() const override {
        std::vector<Element> children;
        // 添加枚举基本信息
        children.emplace_back("structID: " + std::to_string(structID));
        children.emplace_back("name: " + structName);

        // 添加所有枚举成员
        for (const auto& [name, value] : *MemberNames) {
            children.emplace_back(name + ": " + std::to_string(value));
        }
        return children;
    }

    [[nodiscard]] std::string toString() const override {
        std::string result = "EnumType { id: " + std::to_string(structID) +
                            ", name: " + structName + ", members: { ";

        // 拼接所有成员
        size_t count = 0;
        for (const auto& [name, value] : *MemberNames) {
            if (count > 0) result += ", ";
            result += name + ": " + std::to_string(value);
            count++;
        }
        result += " } }";
        return result;
    }
};

struct ReferenceType : Type {
    explicit ReferenceType(std::shared_ptr<Type> t,bool is_mut=false):Type(TypeName::ReferenceType,std::move(t)) {
        this->is_mutable=is_mut;
    }
    bool equals(const Type *other) const override {
        return false;
    }
    [[nodiscard]] std::vector<Element> get_children() const override {
        return  typePtr->get_children();
    }
    [[nodiscard]] std::string toString() const override {
        return "ReferenceType";
    }
    void accept(SemanticCheck &, ASTNode *, ASTNode *, ASTNode *,std::pair<SymbolTable*,ASTNode*>) override ;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override {

    };
};

struct StructType:Type {
    int structID;
    std::string structName;
    SymbolTable* field;
    int FieldNum;
    std::vector<Field> fields;
    StructType(int id, std::string name, SymbolTable* s,int f,std::vector<Field> fi):
    Type(TypeName::StructType),structID(id),structName(std::move(name)),field(s),FieldNum(f),fields(std::move(fi)) {}
    // StructType的三个纯虚函数实现
    bool equals(const Type* other) const override {
        // 先判断是否为StructType类型
        if (const auto* other_struct = dynamic_cast<const StructType*>(other)) {
            // 结构体类型相等的条件：结构ID相同且字段数量相同
            return structID == other_struct->structID &&
                   FieldNum == other_struct->FieldNum;
        }
        return false;
    }
    void accept(SemanticCheck& visitor,ASTNode* F,ASTNode* l,ASTNode* f,std::pair<SymbolTable*,ASTNode*>) override ;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override {

    };
    [[nodiscard]] std::vector<Element> get_children() const override {
        std::vector<Element> children;
        // 添加结构体基本信息
        children.emplace_back("structID: " + std::to_string(structID));
        children.emplace_back("name: " + structName);
        children.emplace_back("field count: " + std::to_string(FieldNum));

        // 如果需要可以添加字段符号表信息（根据你的SymbolTable实现调整）
        // children.push_back("fields: " + field->toString()); // 假设SymbolTable有toString方法
        return children;
    }

    [[nodiscard]] std::string toString() const override {
        return "StructType { id: " + std::to_string(structID) +
               ", name: " + structName + ", field count: " + std::to_string(FieldNum) + " }";
    }
};

struct RustType : ASTNode {
    explicit RustType(std::shared_ptr<Type> t,bool is_mut=false):
    ASTNode(TypeName::RustType,std::move(t)) {
        this->isMutable=is_mut;
    }
    [[nodiscard]] std::vector<Element> get_children() const override {
        std::vector<Element> children;
        children.emplace_back("RustType:");
        children.emplace_back(this->realType.get());
        return children;
    }
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override ;
};

struct Expr : ASTNode {//之后再细化了类型的分类，实际上只有很少量的类型需要用到这个，比如具体化我是int，或者说具体表示某个数组
    explicit Expr(const TypeName t, std::shared_ptr<Type> e=nullptr): ASTNode(t,std::move(e)) {
    }
    ~Expr() override = default;
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override=0;
    [[nodiscard]] std::vector<Element> get_children() const override=0;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override =0;
};

struct LiteralExpr : Expr {
    std::string value;
    explicit LiteralExpr(std::string v, std::shared_ptr<Type> t,std::any eval=std::any()) :
        Expr(TypeName::LiteralExpr, std::move(t)), value(std::move(v)) {
        this->eval=std::move(eval);
    }
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct ArrayInitExpr : Expr {
    std::vector<std::shared_ptr<Expr> > elements;
    explicit ArrayInitExpr(std::vector<std::shared_ptr<Expr> > es, std::shared_ptr<Type> t = nullptr):
    Expr(TypeName::ArrayInitExpr, std::move(t)), elements(std::move(es)) {
    }
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct ArraySimplifiedExpr :Expr{
    std::shared_ptr<Expr> element;
    std::shared_ptr<Expr> length;
    explicit ArraySimplifiedExpr(std::shared_ptr<Expr>e,std::shared_ptr<Expr> length,std::shared_ptr<Type> t = nullptr):
    Expr(TypeName::ArraySimplifiedExpr, std::move(t)), element(std::move(e)) ,length(std::move(length)){
    }
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct ArrayAccessExpr : Expr {
    std::shared_ptr<Expr> array;
    std::shared_ptr<Expr> index;
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    ArrayAccessExpr(std::shared_ptr<Expr> a, std::shared_ptr<Expr> i,std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::ArrayAccessExpr,std::move(t)), array(std::move(a)), index(std::move(i)) {
    }
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct UnitExpr : Expr {
    UnitExpr():Expr(TypeName::UnitExpr){}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct BinaryExpr : Expr {
    std::shared_ptr<Expr> left;
    std::string op;
    std::shared_ptr<Expr> right;

    BinaryExpr(std::shared_ptr<Expr> left, std::string op, std::shared_ptr<Expr> right,std::shared_ptr<Type> t=nullptr)
        : Expr(TypeName::BinaryExpr,std::move(t)), left(std::move(left)), op(std::move(op)), right(std::move(right)) {
    }
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct UnderscoreExpr:Expr {
    UnderscoreExpr():Expr(TypeName::UnderscoreExpr,std::make_shared<NeverType>()){}
    [[nodiscard]] std::vector<Element> get_children() const override;
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct UnaryExpr : Expr {
    std::string op;
    std::shared_ptr<Expr> right;

    UnaryExpr(std::string op, std::shared_ptr<Expr> right,bool is_mutable=false,std::shared_ptr<Type> t=nullptr)
        : Expr(TypeName::UnaryExpr,std::move(t)), op(std::move(op)), right(std::move(right)) {
        this->isMutable=is_mutable;
    }
    
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct PathExpr : Expr {
    std::vector<std::string> segments;
    explicit PathExpr(std::vector<std::string> s, std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::PathExpr, std::move(t)), segments(std::move(s)) {
    }
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

     struct CallExpr : Expr {
         std::shared_ptr<Expr> receiver;
         std::vector<std::shared_ptr<Expr> > arguments;

         CallExpr(std::shared_ptr<Expr> r, std::vector<std::shared_ptr<Expr> > a,std::shared_ptr<Type> t=nullptr)
             : Expr(TypeName::CallExpr,std::move(t)), receiver(std::move(r)), arguments(std::move(a)) {
         }

         void accept(SemanticCheck &visitor, ASTNode *F, ASTNode *l, ASTNode *f) override ;
         
    [[nodiscard]] std::vector<Element> get_children() const override;
         void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
     };

struct FieldAccessExpr : Expr {
    std::shared_ptr<Expr> struct_name;
    std::shared_ptr<Expr> field_expr;
    explicit FieldAccessExpr(std::shared_ptr<Expr> s, std::shared_ptr<Expr> f,std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::FieldAccessExpr,std::move(t)),struct_name(std::move(s)), field_expr(std::move(f)){}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct BlockExpr : Expr {
    std::vector<std::pair<std::shared_ptr<ASTNode> ,bool>> statements;
    explicit BlockExpr(std::vector<std::pair<std::shared_ptr<ASTNode> ,bool>> s, std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::BlockExpr,std::move(t)),statements(std::move(s)){}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct IfExpr: Expr {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Expr> then_branch;
    std::shared_ptr<Expr> else_branch; //可能是block，也可能是if
    IfExpr(std::shared_ptr<Expr> c, std::shared_ptr<Expr> t, std::shared_ptr<Expr> e, std::shared_ptr<Type> ty=nullptr):
    Expr(TypeName::IfStmt,std::move(ty)), condition(std::move(c)), then_branch(std::move(t)), else_branch(std::move(e)
        ) {}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct ReturnExpr : Expr {
    std::shared_ptr<Expr> expr;
    explicit ReturnExpr(std::shared_ptr<Expr> e=nullptr,std::shared_ptr<Type> t=nullptr) :
    Expr(TypeName::ReturnExpr,std::move(t)),expr(std::move(e)) {}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct GroupedExpr : Expr {
    std::shared_ptr<Expr> expr;
    explicit GroupedExpr(std::shared_ptr<Expr> e, std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::GroupedExpr,std::move(t)), expr(std::move(e)) {}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct AssignmentExpr : Expr {
    std::shared_ptr<Expr> left;
    std::string op;
    std::shared_ptr<Expr> right;
    AssignmentExpr(std::shared_ptr<Expr> l,std::string op,std::shared_ptr<Expr> r):
    Expr(TypeName::AssignmentExpr), left(std::move(l)),op(std::move(op)), right(std::move(r)) {}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct application {
    std::shared_ptr<Expr> name;
    std::shared_ptr<Expr> variable;
};

struct StructExpr : Expr {
    std::shared_ptr<Expr> structname;
    std::vector<application> apps;
    
    StructExpr(std::shared_ptr<Expr> s,std::vector<application> a, std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::StructExpr,std::move(t)), apps(std::move(a)), structname(std::move(s)) {}
    [[nodiscard]] std::vector<Element> get_children() const override;
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct AsExpr : Expr {
    std::shared_ptr<Expr> expr;
    std::shared_ptr<RustType> type;
    explicit AsExpr(std::shared_ptr<Expr> e,std::shared_ptr<RustType> type,std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::AsExpr,std::move(t)), expr(std::move(e)),type(std::move(type)) {}
    [[nodiscard]] std::vector<Element> get_children() const override;
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct ContinueExpr : Expr {
    ContinueExpr() : Expr(TypeName::ContinueExpr, nullptr) {}
    
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct BreakExpr : Expr {
    std::shared_ptr<Expr> expr;
    BreakExpr() : Expr(TypeName::BreakExpr, nullptr) {}
    explicit BreakExpr(std::shared_ptr<Expr> e,std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::BreakExpr, std::move(t)),expr(std::move(e)){}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct LoopExpr : Expr {
    std::shared_ptr<Expr> block;
    explicit LoopExpr(std::shared_ptr<Expr> b, std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::LoopExpr, std::move(t)), block(std::move(b)) {}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct WhileExpr : Expr {
    std::shared_ptr<Expr> condition;
    std::shared_ptr<Expr> block;
    WhileExpr(std::shared_ptr<Expr> c, std::shared_ptr<Expr> b, std::shared_ptr<Type> t=nullptr):
    Expr(TypeName::WhileExpr, std::move(t)),condition(std::move(c)), block(std::move(b)) {}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct Stmt : ASTNode {
    explicit Stmt(const TypeName t): ASTNode(t) {
    }
    ~Stmt() override =default;
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override =0;
    [[nodiscard]] std::vector<Element> get_children() const override=0;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override=0;
};

struct ConstStmt : Stmt {
    std::string identifier;
    std::shared_ptr<RustType> type;
    std::shared_ptr<Expr> expr;
    ConstStmt(std::string id, std::shared_ptr<RustType> t, std::shared_ptr<Expr> e):
        Stmt(TypeName::ConstStmt), identifier(std::move(id)), type(std::move(t)), expr(std::move(e)) {
    }
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct FnStmt : Stmt {
    std::string name;
    std::vector<Param> parameters;
    std::shared_ptr<RustType> return_type;
    std::shared_ptr<Expr> body;
    bool is_crate=false;
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    FnStmt(std::string n, std::vector<Param> p, std::shared_ptr<RustType> r, std::shared_ptr<Expr> b):
        Stmt(TypeName::FnStmt), name(std::move(n)), parameters(std::move(p)),
        return_type(std::move(r)), body(std::move(b)) {
        if (!return_type) {
            return_type=std::make_shared<RustType>(std::make_shared<TypeType>(std::make_shared<UnitType>()));
        }
    }
    ~FnStmt() override = default;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
    
    
    [[nodiscard]] std::vector<Element> get_children() const override;
};


struct StructStmt:Stmt {
    std::string name;
    std::vector<Field> fields;
    std::shared_ptr<Type> Struct_Type=nullptr;
    StructStmt(std::string n,std::vector<Field>f,std::shared_ptr<Type> S=nullptr):
        Stmt(TypeName::StructStmt), name(std::move(n)), fields(std::move(f)), Struct_Type(std::move(S)){
    }
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct EnumStmt:Stmt{
    std::string enum_name;
    std::vector<std::string> ids;
    EnumStmt(std::string n,std::vector<std::string> i): Stmt(TypeName::EnumStmt), enum_name(std::move(n)), ids(std::move(i)) {
    }
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

// struct TraitStmt:Stmt {
//     std::string name;
//     std::vector<std::pair<std::shared_ptr<FnStmt>,bool> > fns;
//     std::vector<std::pair<std::shared_ptr<ConstStmt>,bool>> cons;
//     TraitStmt(std::string n,std::vector<std::pair<std::shared_ptr<FnStmt>,bool> >f,std::vector<std::pair<std::shared_ptr<ConstStmt>,bool>> c):
//     Stmt(TypeName::TraitStmt),name(std::move(n)),fns(std::move(f)),cons(std::move(c)){}
//     [[nodiscard]] std::vector<Element> get_children() const override;
// };

//区分固有实现还是特征实现
struct InherentImplStmt:Stmt {
    std::string name;
    std::vector<std::pair<std::shared_ptr<FnStmt>,bool> > fns;
    std::vector<std::pair<std::shared_ptr<ConstStmt>,bool>> cons;
    explicit InherentImplStmt(std::string n,std::vector<std::pair<std::shared_ptr<FnStmt>,bool> >f,std::vector<std::pair<std::shared_ptr<ConstStmt>,bool>> c):
    Stmt(TypeName::InherentImplStmt),name(std::move(n)),fns(std::move(f)),cons(std::move(c)){}
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

// struct TraitImplStmt:Stmt {
//     std::string trait_name;
//     std::string struct_name;
//     std::vector<std::pair<std::shared_ptr<FnStmt>,bool> > fns;
//     std::vector<std::pair<std::shared_ptr<ConstStmt>,bool>> cons;
//     TraitImplStmt(std::string t,std::string s,std::vector<std::pair<std::shared_ptr<FnStmt>,bool> >f,std::vector<std::pair<std::shared_ptr<ConstStmt>,bool>> c)
//         : Stmt(TypeName::TraitImplStmt),trait_name(std::move(t)), struct_name(std::move(s)), fns(std::move(f)),cons(std::move(c)) {}
//
//     [[nodiscard]] std::vector<Element> get_children() const override;
// };

struct LetStmt:Stmt {
    std::string identifier;
    bool is_mutable;
    std::shared_ptr<RustType> type;
    std::shared_ptr<Expr> expr;
    LetStmt(std::string i,std::shared_ptr<RustType> t,std::shared_ptr<Expr> e, const bool m=false)
        : Stmt(TypeName::LetStmt), identifier(std::move(i)),type(std::move(t)), is_mutable(m), expr(std::move(e)) {
    }
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};

struct Program : ASTNode {
    std::vector<std::shared_ptr<ASTNode>> statements;
    // std::vector<std::shared_ptr<ConstStmt>> cons;
    // std::vector<std::shared_ptr<FnStmt>> fns;
    // std::vector<std::shared_ptr<EnumStmt>> enums;
    // std::vector<std::shared_ptr<StructStmt>> structs;
    // std::vector<std::shared_ptr<InherentImplStmt>> inherits;
    // std::vector<std::shared_ptr<TraitStmt>> traits;
    // std::vector<std::shared_ptr<TraitImplStmt>> impls;
    //其余的全局变量\enum\trait\impl等等我都先不管
    // explicit Program(std::vector<std::shared_ptr<ConstStmt>> c,std::vector<std::shared_ptr<FnStmt>> f,std::vector<std::shared_ptr<EnumStmt>> e,
    //     std::vector<std::shared_ptr<StructStmt>> s,std::vector<std::shared_ptr<InherentImplStmt>> in,
    //     std::vector<std::shared_ptr<TraitStmt>> t,std::vector<std::shared_ptr<TraitImplStmt>> im):
    // ASTNode(TypeName::Program),cons(std::move(c)),fns(std::move(f)),enums(std::move(e)),structs(std::move(s)),inherits(std::move(in)),traits(std::move(t)),impls(std::move(im)){}
    explicit Program(std::vector<std::shared_ptr<ASTNode>> s):
    ASTNode(TypeName::Program), statements(std::move(s)) {}
    ~Program() override = default;
    void accept(SemanticCheck &visitor,ASTNode* F,ASTNode* l,ASTNode* f) override ;
    
    [[nodiscard]] std::vector<Element> get_children() const override;
    void IR(IRgen &visitor, ASTNode *r, ASTNode *l) override;
};
#endif //AST_NODE_H
