//
// Created by ckjsuperhh6602 on 25-9-29.
//
#include "semantic_check.h"

#include <utility>
#include "AST_node.h"
#include "tokenizer.h"

void resolveDependency(ASTNode*node) {

}
void SemanticCheck::pre_processor(ASTNode *node, ASTNode *F, ASTNode *l, ASTNode *f) {
    auto v=node->get_children();
    if (node->realType==nullptr) {
        return;
    }
    if (node->get_type()==TypeName::BlockExpr) {
        node->scope=std::make_pair(new SymbolTable(),F);
    }
    for (auto i:v) {
        if (std::holds_alternative<ASTNode*>(i)) {
            std::get<ASTNode*>(i)->scope=node->scope;
        }
    }
    if (node->get_type() == TypeName::Program || node->get_type() == TypeName::BlockExpr)
        resolveDependency(node);
    if (node->get_type() == TypeName::WhileExpr|| node->get_type() == TypeName::LoopExpr) {
        l = node;
    }
    if (node->get_type() == TypeName::FnStmt) {
        f = node;
    }
    for (auto i=0;i<v.size();i++) {
        if (std::holds_alternative<ASTNode*>(v[i])) {
            auto vi=std::get<ASTNode*>(v[i]);
            node->Id=i;
            pre_processor(vi,node,l,f);
            vi->accept(*this,vi,l,f);
            node->is_hasBreak |= vi->is_hasBreak;
            node->is_hasReturn |= vi->is_hasReturn;
        }
    }
    if (node->get_type()==TypeName::BreakExpr|| node->get_type()==TypeName::ContinueExpr) {
        node->is_hasBreak =true;
    }else if (node->get_type()==TypeName::ReturnExpr) {
        node->is_hasReturn = true;
    }else if (node->get_type()==TypeName::IfExpr) {
        if (auto changer=dynamic_cast<IfExpr*>(node);changer->else_branch) {
            node->is_hasBreak = changer->then_branch->is_hasBreak & changer->else_branch->is_hasBreak;
            node->is_hasReturn = changer->then_branch->is_hasReturn & changer->else_branch->is_hasReturn;
        }else {
            node->is_hasBreak = node->is_hasReturn=false;
        }
    }else if (node->get_type()==TypeName::FnStmt) {
        node->is_hasReturn=false;
    }else if (node->get_type()==TypeName::WhileExpr) {
        node->is_hasBreak=node->is_hasReturn=false;
    }else if (node->get_type()==TypeName::LoopExpr) {
        node->is_hasBreak=false;
    }
}

std::shared_ptr<Type> SemanticCheck::TypeToItem(std::shared_ptr<Type> t) {
    if (t->typeKind==TypeName::UnitType) {
        return t;
    }
    if (t->typeKind!=TypeName::TypeType) {
        throw std::runtime_error("SemanticCheck::TypeToItem: type mismatch");
    }
    return t->typePtr;
}

std::shared_ptr<Type> SemanticCheck::ItemToType(std::shared_ptr<Type> t) {
    if (t->typeKind==TypeName::UnitType) {
        return t;
    }
    return std::make_shared<Type>(TypeName::TypeType,t);
}

void is_StrongDerivable(const std::shared_ptr<Type>& T1,const std::shared_ptr<Type>& T0,bool canRemoveMut=true) {
    if (T1->typeKind==T0->typeKind&&T1->typePtr->equals(T0->typePtr.get())) {
        return;
    }
    if (T1->typeKind==TypeName::NeverType||T1->typeKind==TypeName::VersatileType||
        T0->typeKind==TypeName::VersatileType) {
        return;
    }
    if (T1->typeKind==TypeName::BasicType&&T0->typeKind==TypeName::BasicType) {
        auto Basic_T1=std::dynamic_pointer_cast<BasicType>(T1);
        auto Basic_T0=std::dynamic_pointer_cast<BasicType>(T0);
        if (Basic_T1->kind==TypeName::Integer&&(Basic_T0->kind==TypeName::Iint||Basic_T0->kind==TypeName::Uint||
            Basic_T0->kind==TypeName::Uint||Basic_T0->kind==TypeName::I32||Basic_T0->kind==TypeName::U32||
            Basic_T0->kind==TypeName::Isize||Basic_T0->kind==TypeName::Usize)) {
            return;
        }
        if (Basic_T1->kind==TypeName::Iint&&(Basic_T0->kind==TypeName::I32||Basic_T0->kind==TypeName::Isize)) {
            return;
        }
        if (Basic_T1->kind==TypeName::Uint&&(Basic_T0->kind==TypeName::U32||Basic_T0->kind==TypeName::Usize)) {
            return;
        }
    }
    if (T1->typeKind==TypeName::ArrayType&&T0->typeKind==TypeName::ArrayType) {
        auto Array_T1=std::dynamic_pointer_cast<ArrayType>(T1);
        auto Array_T0=std::dynamic_pointer_cast<ArrayType>(T0);
        if (Array_T1->length==Array_T0->length) {
            is_StrongDerivable(Array_T1->typePtr,Array_T0->typePtr,canRemoveMut);
        }
    }
    //TODO
    throw std::runtime_error("SemanticCheck::ItemToType: type mismatch");

}

bool is_Number(const std::shared_ptr<Type>& T) {
    if (T->typeKind!=TypeName::BasicType) {
        return false;
    }
    auto T1=std::dynamic_pointer_cast<BasicType>(T);
    return T1->kind==TypeName::Integer || T1->kind==TypeName::Uint||T1->kind==TypeName::Usize||T1->kind== TypeName::Iint
    ||T1->kind==TypeName::U32||T1->kind==TypeName::I32||T1->kind==TypeName::Isize;
}

bool is_NumberBool(const std::shared_ptr<Type>& T) {
    if (T->typeKind!=TypeName::BasicType) {
        return false;
    }
    auto T1=std::dynamic_pointer_cast<BasicType>(T);
    return T1->kind==TypeName::Bool||is_Number(T1);
}

bool is_Char(const std::shared_ptr<Type>& T) {
    if (T->typeKind!=TypeName::BasicType) {
        return false;
    }
    auto T1=std::dynamic_pointer_cast<BasicType>(T);
    return T1->kind==TypeName::Char||T1->kind==TypeName::String;

}


void SemanticCheck::is_NumDerivable(std::shared_ptr<Type>& T1,std::shared_ptr<Type>& T0) {

}

void SemanticCheck::is_AllDerivable(std::shared_ptr<Type>& T1,std::shared_ptr<Type>& T0) {
    if (T1->typeKind==TypeName::VersatileType) {
        T1=std::move(T0);
        return;
    }
    if (T0->typeKind==TypeName::VersatileType) {
        T0=std::move(T1);
        return;
    }
    if (T1->typeKind==TypeName::ArrayType||T0->typeKind==TypeName::ArrayType) {
        if (T1->typeKind!=T0->typeKind) {
            throw std::runtime_error("SemanticCheck::one is array but the other is not an array(type mismatch)");
        }
        is_AllDerivable(T1->typePtr,T0->typePtr);
        return;
    }
    if (T1->typeKind==TypeName::BasicType&&T0->typeKind==TypeName::BasicType) {
        is_NumDerivable(T1,T0);
    }
    throw std::runtime_error("SemanticCheck::Deriving: type mismatch");
}

SymbolEntry SymbolTable::lookup_i(std::string stri) {

}

SymbolEntry SymbolTable::lookup_t(std::string stri) {

}

void SymbolTable::setItem(std::string stri,SymbolEntry it) {

}

void SymbolTable::setType(std::string stri,SymbolEntry ty) {

}

void SemanticCheck::visit(LetStmt*node,ASTNode* F,ASTNode* l,ASTNode* f) {
    auto T0=TypeToItem(node->type->realType);
    if (node->expr) {
        auto T1=node->expr->realType;
        is_StrongDerivable(T1,T0);
    }
    SymbolEntry val={T0,std::any(),node->is_mutable,false};
    if (node->scope.first->lookup_i(node->identifier).is_Global) {
        throw std::runtime_error("SemanticCheck::visit: global variable is not allowed to be the same name with let variable");
    }
    node->scope.first->setItem(node->identifier,val);
}

void SemanticCheck::visit(IfExpr*node,ASTNode* F,ASTNode* l,ASTNode* f) {
    if (node->condition->realType->typeKind!=TypeName::Bool) {
        throw  std::runtime_error("SemanticCheck::visit: condition must be a boolean expression");
    }
    if (node->else_branch==nullptr) {
        is_StrongDerivable(node->then_branch->realType,std::make_shared<UnitType>());
        node->realType=std::make_shared<UnitType>();
    }else {
        auto T0=node->then_branch->realType;
        auto T1=node->else_branch->realType;
        if (T0->typeKind!=TypeName::NeverType) {
            T0=T1;
        }else if (T1->typeKind!=TypeName::NeverType) {
            T1=T0;
        }
        is_AllDerivable(T0,T1);
        node->realType=T0;
    }
}

void SemanticCheck::visit(WhileExpr*node,ASTNode* F,ASTNode* l,ASTNode* f) {
    if (node->condition->realType->typeKind!=TypeName::Bool) {
        throw  std::runtime_error("SemanticCheck::visit: condition must be a boolean expression");
    }
    node->realType=std::make_shared<UnitType>();
}

void SemanticCheck::visit(LoopExpr*node,ASTNode* F,ASTNode* l,ASTNode* f) {
    if (node->realType==nullptr) {
        node->realType=std::make_shared<UnitType>();
    }
    if (!node->block->is_hasAbsoluteBreak) {
        node->realType=std::make_shared<NeverType>();
    }
    is_StrongDerivable(node->block->realType,std::make_shared<UnitType>());
}

void SemanticCheck::visit(BreakExpr*node,ASTNode* F,ASTNode* l,ASTNode* f) {
    node->realType=std::make_shared<NeverType>();
    if (l==nullptr) {
        throw std::runtime_error("SemanticCheck::visit: break must be in a loop");
    }
    if (l->get_type()==TypeName::WhileExpr) {
        if (l->Id==0) {
            throw std::runtime_error("SemanticCheck::visit: break cannot occur in the condition of while");
        }
        if (node->expr!=nullptr) {
            throw std::runtime_error("SemanticCheck::visit: break cannot have a value in while expression");9
        }
    }else {
        std::shared_ptr<Type> T0;
        if (node->expr!=nullptr) {
            T0=node->expr->realType;
        }
        if (T0->typeKind!=TypeName::NeverType) {
            if (l->realType!=nullptr) {
                is_StrongDerivable(l->realType,T0);
            }else {
                 l->realType=T0;
            }
        }
    }
}

void SemanticCheck::visit(FnStmt*node,ASTNode* F,ASTNode* l,ASTNode* f) {
    auto T0=TypeToItem(node->return_type);
    auto T1=node->body->realType;
    is_StrongDerivable(T1,T0);
}

void SemanticCheck::visit(Program*node,ASTNode* F,ASTNode* l,ASTNode* f) {}

void SemanticCheck::visit(StructStmt*node,ASTNode* F,ASTNode* l,ASTNode* f) {}

void SemanticCheck::visit(ArrayAccessExpr *node, ASTNode *F, ASTNode *l, ASTNode *f) {

}

void SemanticCheck::visit(BlockExpr*node,ASTNode* F,ASTNode* l,ASTNode* f) {
    if (node->statements.empty()) {
        node->realType=std::make_shared<UnitType>();
    }else {
        if (!node->statements.back().second) {//最后一位是没有分号的
            node->realType=node->statements.back().first->realType;
        }else {
            node->realType=std::make_shared<UnitType>();
            for (auto &[child,semicolon]:node->statements) {
                if (child->realType->typeKind==TypeName::NeverType||child->is_hasBreak||child->is_hasReturn) {
                    node->realType=std::make_shared<NeverType>();
                }
            }
        }
        for (auto i=0;i<node->statements.size()-1;i++) {
            if (!node->statements[i].second&&node->statements[i].first->realType->typeKind!=TypeName::Unit) {//没有分号但是返回值不是unit
                throw std::runtime_error("SemanticCheck::visit: statement without a semicolon is not a unit");
            }
        }
    }
}

void SemanticCheck::visit(ReturnExpr*node,ASTNode* F,ASTNode* l,ASTNode* f) {
    if (node->expr==nullptr) {
        node->realType=std::make_shared<UnitType>();
    }else {
        node->realType=node->expr->realType;
    }
    if (f==nullptr) {
        throw std::runtime_error("SemanticCheck::visit: return expression exists in a non-function condition");
    }
    auto T1=TypeToItem(f->realType);
    is_StrongDerivable(node->realType,T1);
    node->realType=std::make_shared<NeverType>();
}

void SemanticCheck::visit(BinaryExpr*node,ASTNode* F,ASTNode* l,ASTNode* f) {
    auto T1=node->left->realType,T2=node->right->realType;
    if (T1->typeKind!=TypeName::BasicType||T2->typeKind!=TypeName::BasicType) {
        throw std::runtime_error("ridiculous types around the binary expression");
    }
    T1=std::dynamic_pointer_cast<BasicType>(node->left->realType);
    T2=std::dynamic_pointer_cast<BasicType>(node->right->realType);
    auto E1=node->left->eval,E2=node->right->eval;
    if (node->op=="+"||node->op=="-"||node->op=="*"||node->op=="/"||node->op=="%"||node->op=="<<"||node->op==">>") {
        if (!is_Number(T1)) {
            throw std::runtime_error("SemanticCheck::visit: left operand is not a number");
        }
        is_NumDerivable(T1,T2);
        node->realType=T1;
        if (E1.has_value()&&E2.has_value()) {
            auto lE1=std::any_cast<long long>(E1),lE2=std::any_cast<long long>(E2);
        if (node->op=="+") {
            node->eval=lE1+lE2;
        }else if ( node->op=="-") {
            node->eval=lE1-lE2;
        }else if (node->op=="*") {
            node->eval=lE1*lE2;
        }else if (node->op=="/") {
            node->eval=lE1/lE2;
        }else if (node->op=="%") {
            node->eval=lE1%lE2;
        }else if (node->op=="<<") {
            node->eval=lE1<<lE2;
        }else{
            node->eval=lE1>>lE2;
        }
        }
    }else if (node->op=="^"||node->op=="&"||node->op=="|") {
        if (T1->typeKind==TypeName::Bool&&T2->typeKind==TypeName::Bool) {
            node->realType=T1;
            if (E1.has_value()&&E2.has_value()) {
                auto lE1=std::any_cast<bool>(E1),lE2=std::any_cast<bool>(E2);
                if (node->op=="^") {
                    node->eval=lE1^lE2;
                }else if (node->op=="&") {
                    node->eval=lE1&lE2;
                }else {
                    node->eval=lE1|lE2;
                }
            }
        }else {
            if (!is_Number(T1)) {
                throw std::runtime_error("SemanticCheck::visit: left operand is not a number");
            }
            is_NumDerivable(T1,T2);
            node->realType=T1;
            if (E1.has_value()&&E2.has_value()) {
                auto lE1=std::any_cast<long long>(E1),lE2=std::any_cast<long long>(E2);
                if (node->op=="^") {
                    node->eval=lE1^lE2;
                }else if (node->op=="&") {
                    node->eval=lE1&lE2;
                }else {
                    node->eval=lE1|lE2;
                }
            }
        }
    }else if (node->op==">="||node->op=="<="||node->op==">"||node->op=="<") {
        node->realType=std::make_shared<BasicType>(TypeName::Bool);
        if (T1->typeKind==TypeName::Bool&&T2->typeKind==TypeName::Bool) {
            if (E1.has_value()&&E2.has_value()) {
                auto lE1=std::any_cast<bool>(E1),lE2=std::any_cast<bool>(E2);
                if (node->op==">") {
                    node->eval=lE1>lE2;
                }else if (node->op=="<=") {
                    node->eval=lE1<=lE2;
                }else if (node->op=="<") {
                    node->eval=lE1<lE2;
                }else {
                    node->eval=lE1>=lE2;
                }
            }
        }else if (T1->typeKind==TypeName::Char&&T2->typeKind==TypeName::Char) {
            if (E1.has_value()&&E2.has_value()) {
                auto lE1=std::any_cast<bool>(E1),lE2=std::any_cast<bool>(E2);
                if (node->op==">") {
                    node->eval=lE1>lE2;
                }else if (node->op=="<=") {
                    node->eval=lE1<=lE2;
                }else if (node->op=="<") {
                    node->eval=lE1<lE2;
                }else {
                    node->eval=lE1>=lE2;
                }
            }
        }else {
            is_NumDerivable(T1,T2);
            if (E1.has_value()&&E2.has_value()) {
                auto lE1=std::any_cast<long long>(E1),lE2=std::any_cast<long long>(E2);
                if (node->op==">") {
                    node->eval=lE1>lE2;
                }else if (node->op=="<=") {
                    node->eval=lE1<=lE2;
                }else if (node->op=="<") {
                    node->eval=lE1<lE2;
                }else {
                    node->eval=lE1>=lE2;
                }
            }
        }
    }else if (node->op=="=="||node->op=="!=") {
        //TODO
    }else if (node->op=="||"||node->op=="&&") {
        node->realType=std::make_shared<BasicType>(TypeName::Bool);
        if (T1->typeKind==TypeName::Bool&&T2->typeKind==TypeName::Bool) {
            if (E1.has_value()&&E2.has_value()) {
                auto lE1=std::any_cast<bool>(E1),lE2=std::any_cast<bool>(E2);
                if (node->op=="||") {
                    node->eval=lE1||lE2;
                }else {
                    node->eval=lE1&&lE2;
                }
            }
        }
    }
}

void SemanticCheck::visit(UnaryExpr*node,ASTNode* F,ASTNode* l,ASTNode* f) {

}

void SemanticCheck::visit(LiteralExpr *node, ASTNode *F, ASTNode *l, ASTNode *f) {

}

void SemanticCheck::visit(StructExpr *node, ASTNode *F, ASTNode *l, ASTNode *f) {

}

void SemanticCheck::visit(AssignmentExpr *node, ASTNode *F, ASTNode *l, ASTNode *f) {
    auto T1=node->left->realType,T2=node->right->realType;
    if (T1->typeKind!=TypeName::BasicType||T2->typeKind!=TypeName::BasicType) {
        throw std::runtime_error("ridiculous types around the binary expression");
    }
    T1=std::dynamic_pointer_cast<BasicType>(node->left->realType);
    T2=std::dynamic_pointer_cast<BasicType>(node->right->realType);
    auto E1=node->left->eval,E2=node->right->eval;
    if (node->op=="=") {

    }else if (node->op=="+="||node->op=="-="||node->op=="*="||node->op=="/=") {

    }
}

void SemanticCheck::visit(ContinueExpr *node, ASTNode *F, ASTNode *l, ASTNode *f) {
    node->realType=std::make_shared<NeverType>();
    if (l==nullptr) {
        throw std::runtime_error("SemanticCheck::visit: continue expression in a none-loop expression");
    }
}

void SemanticCheck::visit(ConstStmt *node, ASTNode *F, ASTNode *l, ASTNode *f) {

}

void SemanticCheck::visit(ArrayInitExpr *node, ASTNode *F, ASTNode *l, ASTNode *f) {

}

void SemanticCheck::visit(GroupedExpr *node, ASTNode *F, ASTNode *l, ASTNode *f) {

}

void SemanticCheck::visit(ArraySimplifiedExpr *node, ASTNode *F, ASTNode *l, ASTNode *f) {

}

void SemanticCheck::is_AsTrans(std::shared_ptr<Type> T1,std::shared_ptr<Type> T2) {
    if (T1->equals(T2.get())||T2->typeKind==TypeName::NeverType) {
        return;
    }
    if (T1->typeKind==TypeName::BasicType&&T2->typeKind==TypeName::BasicType) {
        auto Basic_T1=std::dynamic_pointer_cast<BasicType>(T1);
        auto Basic_T2=std::dynamic_pointer_cast<BasicType>(T2);
        if (is_NumberBool(T2)||Basic_T2->kind==TypeName::Char) {
            if (Basic_T1->kind==TypeName::I32||Basic_T1->kind==TypeName::Isize||Basic_T1->kind==TypeName::U32||Basic_T1->kind==TypeName::Usize) {
                return;
            }
        }
    }
    throw std::runtime_error("error type of as transfer");
}

void SemanticCheck::visit(AsExpr *node, ASTNode *F, ASTNode *l, ASTNode *f) {
    auto T1= TypeToItem(node->type->realType),T2=node->expr->realType;
    is_AsTrans(T1,T2);
    node->realType=T1;
    auto E2=node->expr->eval;
    if (E2.has_value()) {
        if (T1->equals(T2.get())||is_Number(T2)) {
            node->eval = E2;
        }else if (T2->typeKind==TypeName::BasicType) {
            auto Basic_T2=std::dynamic_pointer_cast<BasicType>(T2);
            if (Basic_T2->kind==TypeName::Char) {
                node->eval=std::any_cast<long long>(E2);
            }else if (Basic_T2->kind==TypeName::Bool) {
                node->eval=std::any_cast<long long>(E2);
            }
        }
    }
}



