//
// Created by ckjsuperhh6602 on 25-9-29.
//
#include "semantic_check.h"

#include <ranges>
#include <stack>
#include <utility>
#include <sys/wait.h>

#include "AST_node.h"
#include "tokenizer.h"

std::string getNodeIdentifier(ASTNode* node) {
    if (node->node_type == TypeName::StructStmt  ) {
        auto structnode=dynamic_cast<StructStmt*>(node);
        return "Type-"+structnode->name;
        }
        if(node->node_type == TypeName::EnumStmt ) {
        auto enumnode=dynamic_cast<EnumStmt*>(node);
            return "Type-"+enumnode->enum_name;
        }
        if (node->node_type == TypeName::InherentImplStmt) {
            auto inherentimpl=dynamic_cast<InherentImplStmt*>(node);
            return "impl-" + inherentimpl->name;  // 实现块前缀
        }
        if (node->node_type==TypeName::ConstStmt) {
            auto constnode=dynamic_cast<ConstStmt*>(node);
            return constnode->identifier;
        }
    if (node->node_type==TypeName::FnStmt) {
        auto fnnode=dynamic_cast<FnStmt*>(node);
        return fnnode->name;
    }
    throw std::runtime_error("Unknown item type");
}

bool is_Item(const ASTNode* i) {
    return i->node_type==TypeName::StructStmt||i->node_type==TypeName::EnumStmt||i->node_type==TypeName::FnStmt||i->node_type==TypeName::ConstStmt;
}

std::vector<std::shared_ptr<ASTNode>> get_ASTNode_children(ASTNode* node) {
    std::vector<std::shared_ptr<ASTNode>> children;
    if (node->node_type==TypeName::Program) {
        for (const auto ProgramNode=dynamic_cast<Program*>(node); auto &i: ProgramNode->statements) {
            children.push_back(i);
        }
        return children;
    }
    if (node->node_type==TypeName::BlockExpr) {
        for (const auto BlockNode=dynamic_cast<BlockExpr*>(node); auto &i: BlockNode->statements | std::views::keys) {
            children.push_back(i);
        }
        return children;
    }
    throw std::runtime_error("getting ASTNode children doesn't support anything but program and block!");
}


void SemanticCheck::resolveDependency(ASTNode*node,std::shared_ptr<Type> SelfType=nullptr) {
    std::vector<std::string> itemName;
    std::unordered_map<std::string, ASTNode*> itemTable;
    auto v=node->get_children();
    std::vector<ASTNode*> children;
    for (const auto& child:v) {
        if (std::holds_alternative<ASTNode*>(child)) {
            children.push_back(std::get<ASTNode*>(child));
        }
    }
    for (auto childnode:children) {
            childnode->scope=node->scope;
            if (is_Item(childnode)) {
                std::string item_name=getNodeIdentifier(childnode);
                itemName.push_back(item_name);
                itemTable[item_name] = childnode;
            }
    }
    std::ranges::sort(itemName);
    int sum=static_cast<int>(itemName.size());
    std::vector<std::vector<int>> DAG(sum);
    std::vector inDegree(sum,0);
    for (auto child:children) {
        if (is_Item(child)) {
            std::string curID=getNodeIdentifier(child);
            auto curIndex=std::ranges::lower_bound(itemName,curID)-itemName.begin();
            std::stack<ASTNode*> nodeStack;
            for (auto grandson:child->get_children()) {
                if (std::holds_alternative<ASTNode*>(grandson)) {
                    nodeStack.push(std::get<ASTNode*>(grandson));
                }
            }
            while (!nodeStack.empty()) {
                ASTNode* curnode=nodeStack.top();
                nodeStack.pop();
                if (curnode->node_type==TypeName::PathExpr) {
                    std::string refID=getNodeIdentifier(curnode);
                    int refIndex=std::lower_bound(itemName.begin(),itemName.end(),refID)-itemName.begin();
                    if (refIndex<sum&&itemName[refIndex]==refID&&itemTable[refID]->node_type!=TypeName::FnStmt) {
                        DAG[refIndex].push_back(curIndex);
                        inDegree[curIndex]++;
                    }
                }
                for (auto grandgrandson:curnode->get_children()) {
                    if (std::holds_alternative<ASTNode*>(grandgrandson)) {
                        nodeStack.push(std::get<ASTNode*>(grandgrandson));
                    }
                }
            }
        }
    }
    std::vector<int> none_InDegree;
    for (int i=0;i<sum;i++) {
        if (inDegree[i]==0) {
            none_InDegree.push_back(i);
        }
    }
    std::vector<int> Topological_sorting;
    while (!none_InDegree.empty()) {
        int cur=none_InDegree.back();
        none_InDegree.pop_back();
        Topological_sorting.push_back(cur);
        for (int dependence:DAG[cur]) {
            inDegree[dependence]--;
            if (inDegree[dependence]==0) {
                none_InDegree.push_back(dependence);
            }
        }
    }
    if (Topological_sorting.size()!=sum) {
        throw std::runtime_error("after topological sort,the vector doesn't have enough items,so there is circular dependency!");
    }
    if (node->node_type==TypeName::Program) {
        auto ProgramNode=dynamic_cast<Program*>(node);
        auto originalChildren=ProgramNode->statements;
        ProgramNode->statements.clear();
        for (auto idx:Topological_sorting) {
            ASTNode* targetRawPtr = itemTable[itemName[idx]];
            auto it = std::find_if(originalChildren.begin(), originalChildren.end(),
        [targetRawPtr](const std::shared_ptr<ASTNode>& ptr) {
            return ptr.get() == targetRawPtr;
        });
            if (it != originalChildren.end()) {
                ProgramNode->statements.push_back(*it); // 添加对应的智能指针
                originalChildren.erase(it); // 从原始容器中移除，避免重复添加
            }
        }
        for (const auto& child : originalChildren) {
            if (child->node_type == TypeName::InherentImplStmt) {
                ProgramNode->statements.push_back(child);
            }
        }
        for (const auto& child : originalChildren) {
            if (!is_Item(child.get()) && child->node_type != TypeName::InherentImplStmt) {
                ProgramNode->statements.push_back(child);
            }
        }
    }else if (node->node_type==TypeName::BlockExpr){
        auto BlockNode=dynamic_cast<BlockExpr*>(node);
        auto originalChildren=BlockNode->statements;
        BlockNode->statements.clear();
        for (auto idx:Topological_sorting) {
            ASTNode* targetRawPtr = itemTable[itemName[idx]];
            auto it = std::find_if(originalChildren.begin(), originalChildren.end(),
        [targetRawPtr](const std::pair<std::shared_ptr<ASTNode>, bool>& pair) {
            // 比较pair的first元素（智能指针）所管理的裸指针
            return pair.first.get() == targetRawPtr;
        });

            if (it != originalChildren.end()) {
                BlockNode->statements.push_back(*it); // 保留整个pair
                originalChildren.erase(it); // 从原始容器移除，避免重复
            }
        }
        for (const auto& child : originalChildren) {
            if (child.first->node_type == TypeName::InherentImplStmt) {
                BlockNode->statements.push_back(child);
            }
        }
        for (const auto& child : originalChildren) {
            if (!is_Item(child.first.get()) && child.first->node_type != TypeName::InherentImplStmt) {
                BlockNode->statements.push_back(child);
            }
        }
    }else {
        throw std::runtime_error("solve dependency doesn't support anything but program and block!");
    }
    for (const auto& child:get_ASTNode_children(node)) {
      if (child->node_type==TypeName::ConstStmt) {
          auto Constnode=std::dynamic_pointer_cast<ConstStmt>(child);
          Constnode->type->scope=Constnode->scope;
          pre_processor(Constnode->type.get(),nullptr,nullptr,nullptr);
          Constnode->type->accept(*this,nullptr,nullptr,nullptr);
          Constnode->expr->scope=Constnode->scope;
          pre_processor(Constnode->expr.get(),nullptr,nullptr,nullptr);
          Constnode->expr->accept(*this,nullptr,nullptr,nullptr);
          auto T0=TypeToItem(Constnode->type->realType);
          auto T1=Constnode->expr->realType;
          is_StrongDerivable(T1,T0);
          if (!Constnode->expr->eval.has_value()) {
              throw std::runtime_error("A constant should have value when compiling");
          }
          SymbolEntry value={T0,Constnode->expr->eval,false,true};
          if (node->scope.first->lookup_i(Constnode->identifier).is_Global) {
              throw std::runtime_error("global variable should not shadow");
          }
          child->scope.first->setItem(Constnode->identifier,value);
      }else if (child->node_type==TypeName::FnStmt) {
          auto Fnnode=std::dynamic_pointer_cast<FnStmt>(child);

          Fnnode->return_type->scope=Fnnode->scope;
          pre_processor(Fnnode->return_type.get(),nullptr,nullptr,nullptr);
          Fnnode->return_type->accept(*this,nullptr,nullptr,nullptr);


          Fnnode->scope=std::make_pair(new SymbolTable(),node);
          if (node->scope.first->lookup_i(Fnnode->name).is_Global) {
              throw std::runtime_error("global variable should not shadow");
          }
          for (int i=0;i<Fnnode->parameters.size();i++) {
              auto id=Fnnode->parameters[i];
              bool is_mut=false,is_and=false;
              if (id.type->is_and) {
                  is_and=true;
              }
              if (id.type->is_mutable) {
                  is_mut=true;
              }
              if (id.type->typeKind!=TypeName::SelfType) {
                  auto T0=TypeToItem(id.type);
                  if (is_and) {
                      throw std::runtime_error("unsupported and type of parameter");
                  }
                  if (!is_mut) {
                      child->scope.first->setItem(id.name,{T0,std::any(),false,false});
                  }else {
                      child->scope.first->setItem(id.name,{T0,std::any(),true,false});
                  }
              }else if (i||id.type->typeKind==TypeName::SelfType) {
                  throw std::runtime_error("self must be the first parameter");
              }else {
                  // std::shared_ptr<SelfType> T{};
                  //TODO
              }
              //TODO
              node->scope.first->setItem(
                      Fnnode->name,
                      {std::make_shared<FunctionType>(Fnnode->parameters,Fnnode->return_type->realType), std::any(), false, true}
                  );
          }
      }else if (child->node_type==TypeName::StructStmt) {
          auto Structnode=std::dynamic_pointer_cast<StructStmt>(child);
          auto T=std::make_shared<StructType>(++structNum,Structnode->name,new SymbolTable(),Structnode->fields.size());
          child->scope=std::make_pair(T->field,node);
          if (!Structnode->fields.empty()) {
              for (auto i=0;i<T->FieldNum;i++) {
                  auto T0=TypeToItem(Structnode->fields[i].type);
                  child->scope.first->setItem("S-"+Structnode->fields[i].name,{T0,std::any(),true,true});
              }
          }
          if (node->scope.first->lookup_t(Structnode->name).is_Global) {
              throw std::runtime_error("global variable should not shadow");
          }
          node->scope.first->setType(Structnode->name,{ItemToType(T),std::any(),false,true});
      }else if (child->node_type==TypeName::InherentImplStmt) {
          auto Implnode=std::dynamic_pointer_cast<InherentImplStmt>(child);
        auto T=TypeToItem(child->scope.first->lookup_t(Implnode->name).type);
        if (T->typeKind!=TypeName::StructType) {
            throw std::runtime_error("impl only for a struct");
        }
          child->scope=std::make_pair(std::dynamic_pointer_cast<StructType>(T)->field,node);
          std::dynamic_pointer_cast<StructType>(T)->field->setType("Self",{ItemToType(T),std::any(),false,false});
          for (const auto& i:Implnode->cons) {
              i.first->scope=child->scope;
              resolveDependency(i.first.get(),T);
          }
          for (const auto & i:Implnode->fns) {
              i.first->scope=child->scope;
              resolveDependency(i.first.get(),T);
          }
      }else if (child->node_type==TypeName::EnumStmt) {
          auto Enumnode=std::dynamic_pointer_cast<EnumStmt>(child);
          auto T=std::make_shared<EnumType>(++structNum,Enumnode->enum_name,new std::unordered_map<std::string, unsigned int>());
          for (int i=0;i<Enumnode->ids.size();i++) {
              if (T->MemberNames->contains(Enumnode->ids[i])) {
                  throw std::runtime_error("enum "+Enumnode->ids[i]+" must be unique");
              }
              T->MemberNames->emplace(Enumnode->ids[i],i);
          }
          if (node->scope.first->lookup_t(Enumnode->enum_name).is_Global) {
              throw std::runtime_error("global variable should not shadow");
          }
          node->scope.first->setType(Enumnode->enum_name,{ItemToType(T),std::any(),false,true});
      }
    }
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
    return std::make_shared<TypeType>(t);
}

void SemanticCheck::visit(PathExpr *node, ASTNode *F, ASTNode *l, ASTNode *f) {

}

void SemanticCheck::visit(FieldAccessExpr *node, ASTNode *F, ASTNode *l, ASTNode *f) {

}

void SemanticCheck::visit(CallExpr *node, ASTNode *F, ASTNode *l, ASTNode *f) {

}

void SemanticCheck::visit(UnitExpr *node, ASTNode *F, ASTNode *l, ASTNode *f) {
    node->realType=std::make_shared<UnitType>();
}

void SemanticCheck::visit(EnumStmt *node, ASTNode *F, ASTNode *l, ASTNode *f) {

}

void SemanticCheck::visit(InherentImplStmt *node, ASTNode *F, ASTNode *l, ASTNode *f) {

}

void SemanticCheck::is_StrongDerivable(const std::shared_ptr<Type>& T1,const std::shared_ptr<Type>& T0,bool canRemoveMut) {
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
        if (Basic_T1->kind==TypeName::Int&&(Basic_T0->kind==TypeName::Iint||Basic_T0->kind==TypeName::Uint||
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
    return T1->kind==TypeName::Int || T1->kind==TypeName::Uint||T1->kind==TypeName::Usize||T1->kind== TypeName::Iint
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
    if (!item_Table.contains(stri)) {
        throw std::runtime_error("SymbolTable::lookup_i: symbol table does not exist");
    }
    return item_Table[stri];
}

SymbolEntry SymbolTable::lookup_t(std::string stri) {
    if (!type_Table.contains(stri)) {
        throw std::runtime_error("SymbolTable::lookup_t: variable does not exist in the Symbol Table");
    }
    return type_Table[stri];
}

void SymbolTable::setItem(std::string stri,SymbolEntry it) {
    item_Table[stri]=it;
}

void SymbolTable::setType(std::string stri,SymbolEntry ty) {
    type_Table[stri]=std::move(ty);
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
            throw std::runtime_error("SemanticCheck::visit: break cannot have a value in while expression");
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
    auto T0=TypeToItem(node->return_type->realType);
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
    auto &T_son=node->right->realType;
    auto E_son=node->right->eval;
    if (node->op=="-") {
        auto Basic_T0 = std::make_shared<BasicType>(TypeName::Iint);
        std::shared_ptr<Type> T0=Basic_T0;
        is_NumDerivable(T_son,T0);
        if (E_son.has_value()) {
            node->eval=-std::any_cast<long long>(E_son);
        }
    }else if (node->op=="!") {
        if (!is_NumberBool(T_son)) {
            throw std::runtime_error("Error in SemanticCheck::visit unary expression: Not a number");
        }
        node->realType=T_son;
        if (E_son.has_value()) {
            if (E_son.type()==typeid(long long)) {
                node->eval=~std::any_cast<long long>(E_son);
            }else if (E_son.type()==typeid(bool)) {
                node->eval=~std::any_cast<bool>(E_son);
            }
        }
    }//TODO
}

void SemanticCheck::visit(LiteralExpr *node, ASTNode *F, ASTNode *l, ASTNode *f) {
    auto T=std::static_pointer_cast<BasicType>(node->exprType);
    if (T->kind==TypeName::Bool) {
        node->realType=std::make_shared<BasicType>(TypeName::Bool);
        node->eval=node->value=="true";
    }else if (T->kind==TypeName::Char) {
        node->realType=std::make_shared<BasicType>(TypeName::Char);
        if (node->value[0]=='\\') {
            if (node->value[1]=='n') {
                node->eval='\n';
            }else if (node->value[1]=='t') {
                node->eval='\t';
            }else if (node->value[1]=='r') {
                node->eval='\r';
            }else if (node->value[1]=='\\') {
                node->eval='\\';
            }else if (node->value[1]=='\'') {
                node->eval='\'';
            }else if (node->value[1]=='"') {
                node->eval='"';
            }else if (node->value[1]=='0') {
                node->eval='\0';
            }else {
                throw std::runtime_error("Invalid type in char node");
            }
        }else {
            node->eval=node->value[0];
        }
    }else if (T->kind==TypeName::Integer) {
        std::string inform;
        for (auto ch:node->value) {
            if (ch!='_') {
                inform.push_back(ch);
            }
        }
        if (inform.size()>3&&inform.substr(inform.size()-3,3)=="i32") {
            node->realType=std::make_shared<BasicType>(TypeName::I32);
            node->eval = static_cast<long long>(std::stoi(inform.substr(0, inform.size()-3), nullptr, 0));
        }else if (inform.size()>3&&inform.substr(inform.size()-3,3)=="u32") {
            node->realType=std::make_shared<BasicType>(TypeName::U32);
            node->eval = static_cast<long long>(std::stoi(inform.substr(0, inform.size()-3), nullptr, 0));
        }else if (inform.size()>3&&inform.substr(inform.size()-5,5)=="isize") {
            node->realType=std::make_shared<BasicType>(TypeName::Isize);
            node->eval = static_cast<long long>(std::stoi(inform.substr(0, inform.size()-5), nullptr, 0));
        }else if (inform.size()>3&&inform.substr(inform.size()-5,5)=="usize") {
            node->realType=std::make_shared<BasicType>(TypeName::Usize);
            node->eval = static_cast<long long>(std::stoi(inform.substr(0, inform.size()-5), nullptr, 0));
        }else {//不存在后缀的integer，那么就需要推导类型了
            auto res = std::stoll(inform);
            node->eval=res;
            if (F->node_type==TypeName::UnaryExpr) {
                auto Unary_father=dynamic_cast<UnaryExpr*>(F);
                if (Unary_father->op=="-") {
                    res=-res;
                }
            }
            if (res>UINT_MAX || res<INT_MIN) {
                throw std::runtime_error("Invalid number in literal node");
            }
            if (res>=0&&res<=INT_MAX) {
                node->realType=std::make_shared<BasicType>(TypeName::Int);
            }else if (res>=0&&res>=INT_MAX) {
                node->realType=std::make_shared<BasicType>(TypeName::Iint);
            }else {
                node->realType=std::make_shared<BasicType>(TypeName::Uint);
            }
        }
    }
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
    if (node->elements.empty()) {
        throw std::runtime_error("SemanticCheck::visit: empty array initialization");
    }
    auto& T=node->elements[0]->realType;
    std::vector<std::any> array_children;
    bool strtok=true;
    for (const auto& child:node->elements) {
        if (T->typeKind==TypeName::NeverType) {
            T=child->realType;
        }else if (child->realType->typeKind==TypeName::NeverType) {
            child->realType=T;
        }else {
            is_AllDerivable(T,child->realType);
        }
        if (!child->eval.has_value()) {
            strtok=false;
        }
        if (strtok) {
            array_children.push_back(child->eval);
        }
    }
    node->realType=std::make_shared<ArrayType>(T,std::make_shared<LiteralExpr>(std::to_string(static_cast<unsigned int>(array_children.size())),std::make_shared<BasicType>(TypeName::Usize)));
    if (strtok) {
        node->eval=array_children;
    }
}

void SemanticCheck::visit(GroupedExpr *node, ASTNode *F, ASTNode *l, ASTNode *f) {
    node->realType=node->expr->realType;
    node->eval=node->expr->eval;
}

void SemanticCheck::visit(ArraySimplifiedExpr *node, ASTNode *F, ASTNode *l, ASTNode *f) {
    auto T=node->length->realType;
    auto E=node->length->eval;
    if (T->typeKind!=TypeName::BasicType) {
        throw std::runtime_error("SemanticCheck::visit:array length is not an integer");
    }
    auto Basic_T=std::dynamic_pointer_cast<BasicType>(T);
    if (!E.has_value()||(Basic_T->typeKind!=TypeName::Unit&&Basic_T->typeKind!=TypeName::Usize&&Basic_T->typeKind!=TypeName::Int)) {
        throw std::runtime_error("SemanticCheck::visit: array length is not a usize type");
    }
    auto len=static_cast<unsigned int>(std::any_cast<long long>(E));
    node->realType=std::make_shared<ArrayType>(T,std::make_shared<LiteralExpr>(std::to_string(len),std::make_shared<BasicType>(TypeName::Usize)));
    auto E0=node->element->eval;
    if (E.has_value()) {
        std::vector<std::any> array_children;
        for (auto i=0;i<len;i++) {
            array_children.push_back(E0);
        }
        node->eval=array_children;
    }
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

void SemanticCheck::visit(RustType* node, ASTNode *F, ASTNode *l, ASTNode *f) {
    if (node->realType->typeKind==TypeName::IdentifierType) {
        auto Id_T=std::dynamic_pointer_cast<IdentifierType>(node->realType);
        auto T=node->scope.first->lookup_t(Id_T->name).type;
        if (T->typeKind!=TypeName::Type) {//我这个位置有点没写明白
            throw std::runtime_error("SemanticCheck::visit: I don't know!!!");
        }
        node->realType=T;
    }else if (node->realType->typeKind==TypeName::ArrayType) {
        auto Array_T=std::dynamic_pointer_cast<ArrayType>(node->realType);
        auto T=Array_T->length->realType;
        auto E=Array_T->length->eval;
        if (T->typeKind!=TypeName::BasicType) {
            throw std::runtime_error("SemanticCheck::visit:array length is not an integer");
        }
        auto Basic_T=std::dynamic_pointer_cast<BasicType>(T);
        if (!E.has_value()||(Basic_T->typeKind!=TypeName::Unit&&Basic_T->typeKind!=TypeName::Usize&&Basic_T->typeKind!=TypeName::Int)) {
            throw std::runtime_error("SemanticCheck::visit: array length is not a usize type");
        }
        //RustType中本身已经维护好了realType(parser中),所以我后续就没有什么必要再往后写了
    }
}



