//
// Created by ckjsuperhh6602 on 25-9-29.
//
#include "semantic_check.h"
#include "AST_node.h"

void SemanticCheck::pre_processor(ASTNode *node, ASTNode *F, ASTNode *l, ASTNode *f) {
    auto v=node->get_children();
    if (node->realType==nullptr) {
        return;
    }
    if (node->get_type()==TypeName::BlockExpr) {
        node->scope=std::make_pair(std::shared_ptr<SymbolTable>(),F);
    }
    for (auto i:v) {
        if (std::holds_alternative<ASTNode*>(i)) {
            std::get<ASTNode*>(i)->scope=node->scope;
        }
    }
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
            node->is_hasBreak |= vi->hasBreak;
            node->is_hasReturn |= vi->hasReturn;
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
