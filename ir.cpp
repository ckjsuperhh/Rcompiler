//
// Created by ckjsuperhh6602 on 2025/11/12.
//
#include "ir.h"
#include<iostream>

#include "AST_node.h"
#include "AST_Node.h"

std::string Op_To_String(const std::string& Op){
    if (Op == "+") {
        return "add";
    }
    if (Op == "-") {
        return "sub";
    }
    if (Op == "*") {
        return "mul";
    }
    if (Op == "/") {
        return "sdiv";
    }
    if (Op == "%") {
        return "srem";
    }
    if (Op == "<<") {
        return "shl";
    }
    if (Op == ">>") {
        return "ashr";
    }
    if (Op == "&") {
        return "and";
    }
    if (Op == "^") {
        return "xor";
    }
    if (Op == "|") {
        return "or";
    }
    if (Op == "==") {
        return "eq";
    }
    if (Op == "!=") {
        return "ne";
    }
    if (Op == "<") {
        return "slt";
    }
    if (Op == ">") {
        return "sgt";
    }
    if (Op == "<=") {
        return "sle";
    }
    if (Op == ">=") {
        return "sge";
    }

    return "illegal";
}

std::string Op_To_uString(const std::string Op) {
    if (Op == "+") {
        return "add";
    }
    if (Op == "-") {
        return "sub";
    }
    if (Op == "*") {
        return "mul";
    }
    if (Op == "==") {
        return "eq";
    }
    if (Op == "!=") {
        return "ne";
    }
    if (Op == "<") {
        return "ult";
    }
    if (Op == ">") {
        return "ugt";
    }
    if (Op == "<=") {
        return "ule";
    }
    if (Op == ">=") {
        return "uge";
    }
    if (Op == "/") {
        return "udiv";
    }
    if (Op == "%") {
        return "urem";
    }
    if (Op == ">>") {
        return "lshr";
    }
    return "illegal";
}

std::string Type_To_String(std::shared_ptr<Type> type) {
    if (type->typeKind==TypeName::TypeType) {
        type=SemanticCheck::TypeToItem(type);
    }
    if (type->typeKind==TypeName::UnitType) {
        return "void";
    }
    if (type->typeKind==TypeName::BasicType) {
        auto Basic_T=std::dynamic_pointer_cast<BasicType>(type);
        if (Basic_T->kind==TypeName::Char) {
            return "i8";
        }
        if (Basic_T->kind==TypeName::Int||Basic_T->kind==TypeName::Uint||Basic_T->kind==TypeName::U32||
            Basic_T->kind==TypeName::I32||Basic_T->kind==TypeName::Usize||Basic_T->kind==TypeName::Isize||
            Basic_T->kind==TypeName::Iint) {
            return "i32";
        }
        if (Basic_T->kind==TypeName::Bool) {
            return "i1";
        }
    }
    if (type->typeKind==TypeName::ArrayType) {
        auto Array_T=std::dynamic_pointer_cast<ArrayType>(type);
        return "["+std::to_string(std::any_cast<long long>(Array_T->length->eval))+" x "+Type_To_String(Array_T->typePtr)+"]";
    }
    if (type->typeKind==TypeName::EnumType) {
        return "i32";
    }
    if (type->typeKind==TypeName::StructType) {
        auto Struct_T=std::dynamic_pointer_cast<StructType>(type);
        return "%v"+std::to_string(Struct_T->structID);
    }
    if (type->typeKind==TypeName::ReferenceType) {
        return "ptr";
    }
    return "illegal type";
}

std::string Type_To_String_ptr(std::shared_ptr<Type> type) {
    if (type->typeKind==TypeName::TypeType) {
        type=SemanticCheck::TypeToItem(type);
    }
    if (type->typeKind==TypeName::ArrayType||type->typeKind==TypeName::StructType||type->typeKind==TypeName::ReferenceType) {
        return "ptr";
    }
    return Type_To_String(type);
}

std::string Constant_To_String(const std::any eval,std::shared_ptr<Type> type) {
    if (eval.type() == typeid(long long)) {
        return std::to_string(static_cast<unsigned int>(std::any_cast<long long>(eval)));
    }
    if (eval.type() == typeid(bool)) {
        return std::any_cast<bool>(eval) ? "1" : "0";
    }
    if (eval.type() == typeid(char)) {
        return std::to_string(static_cast<unsigned int>(static_cast<unsigned char>(std::any_cast<char>(eval))));
    }
    if (type->typeKind==TypeName::UnitType) {
        return "()";
    }
    if (eval.type() == typeid(std::vector<std::any>)){
        std::string res="[";
        auto arr = std::any_cast<std::vector<std::any>>(eval);
        for (int i=0;i<arr.size();i++)
        {
            if (i)
                res+=", ";
            res+=Type_To_String(type->typePtr);
            res+=" ";
            res+=Constant_To_String(arr[i], type->typePtr);
        }
        res+="]";
        return res;
    }
    throw std::exception();
}

bool is_BigType(std::shared_ptr<Type> type) {
    if (type->typeKind==TypeName::TypeType) {
        type=SemanticCheck::TypeToItem(type);
    }
    if (type->typeKind==TypeName::ArrayType||type->typeKind==TypeName::StructType) {
        return true;
    }
    return false;
}

void MemCopy(std::vector<std::any>& res,std::string dest,std::string src,std::shared_ptr<Type> type) {
    auto size_p="%v"+std::to_string(++variableNum);
    auto var="%v"+std::to_string(++variableNum);
    res.emplace_back(std::string(size_p+" = getelementptr "+Type_To_String(type)+", ptr null, i32 1"));
    res.emplace_back(std::string(var+" = ptrtoint ptr "+size_p+" to i32"));
    res.emplace_back(std::string("call void @llvm.memcpy.p0.p0.i32(ptr " +dest+", ptr "+src+", i32 "+var+", i1 false)"));
}

void Load(std::vector<std::any>& res,std::string dest,std::string src,std::shared_ptr<Type> type) {
    if (dest=="@graph") {
        std::cerr<<"Loading graph..."<<std::endl;
    }
    if (is_BigType(type)) {
        MemCopy(res,dest,src,type);
    }else {
        res.emplace_back(std::string(dest+" = load "+Type_To_String(type)+", ptr "+src));
    }
}

void Store(std::vector<std::any>& res,std::string dest,std::string src,std::shared_ptr<Type> type) {
    if (is_BigType(type)) {
        MemCopy(res,dest,src,type);
    }else {
        res.emplace_back(std::string("store "+Type_To_String(type)+" "+src+", ptr "+dest));
    }
}

std::string SetDent(int i) {
    std::string res;
    while (i--) {
        res.push_back(' ');
    }
    return res;
}

void IRgen::pre_process(ASTNode* Node,ASTNode* r,ASTNode* LastBlock) {
    if (Node->node_type==TypeName::ConstStmt && Node->eval.type() != typeid(std::vector<std::any>)){
        auto Const_Node=dynamic_cast<ConstStmt*>(Node);
        auto T=Const_Node->expr;
        if (T->eval.type() == typeid(std::string))
        {
            Node->irRes = "@s"+std::to_string(++variableNum);
            auto value = std::any_cast<std::string>(T->eval);
            r->irCodeStruct.emplace_back(std::string(Node->irRes+" = global ["+std::to_string(value.size()+1)+" x i8] c\""+
               value+"\\00\""));

        }
        else {
            Node->irRes = Constant_To_String(T->eval, T->realType);
        }
        return ;
    }
    if (Node->node_type==TypeName::ConstStmt) {
        return;//前边已经处理过constant了
    }
    if (Node->node_type==TypeName::WhileExpr||Node->node_type==TypeName::LoopExpr) {
        Node->irLabel1="%v"+std::to_string(++variableNum);
        Node->irLabel2="%v"+std::to_string(++variableNum);
        if (Node->node_type==TypeName::LoopExpr) {
            Node->irRes="%v"+std::to_string(++variableNum);
            Node->irRes_p="%v"+std::to_string(++variableNum);
        }
    }
    if (Node->node_type==TypeName::FnStmt) {
        Node->irLabel1="%v"+std::to_string(++variableNum);
        Node->irRes_p="%v"+std::to_string(++variableNum);
        auto FnNode=dynamic_cast<FnStmt*>(Node);
        LastBlock=FnNode->body.get();
    }
    for (auto child:Node->get_children()) {
        if (std::holds_alternative<ASTNode *>(child)) {
            auto ch=std::get<ASTNode *>(child);
            pre_process(ch,r,LastBlock);
            ch->IR(*this,r, LastBlock);
        }
    }
}

void IRgen::generateIr(LiteralExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    // Node->irRes="%v"+std::to_string(++variableNum);
    auto Basic_T=std::dynamic_pointer_cast<BasicType>(Node->realType);
    if (Basic_T->kind==TypeName::Bool) {
        if (Node->eval.has_value()) {
            auto E=std::any_cast<bool>(Node->eval);
            if (E) {
                Node->irRes="true";
            }else {
                Node->irRes="false";
            }
        }
    }else if (Basic_T->kind==TypeName::Char) {
        if (Node->eval.has_value()) {
            auto E=static_cast<long long>(std::any_cast<char>(Node->eval));
            Node->irRes=std::to_string(E);
            // Node->irCode.emplace_back(std::string(std::string(Node->irRes+" = alloca i8")));
            // Node->irCode.emplace_back(std::string("store i8 "+std::to_string(E)+", ptr "+Node->irRes));
        }
    }else if (Basic_T->kind==TypeName::Int||Basic_T->kind==TypeName::Uint||Basic_T->kind==TypeName::Iint||
        Basic_T->kind==TypeName::I32||Basic_T->kind==TypeName::U32||Basic_T->kind==TypeName::Isize||
        Basic_T->kind==TypeName::Usize) {
        if (Node->eval.has_value()) {
            auto E=std::any_cast<long long>(Node->eval);
            Node->irRes=std::to_string(E);
            // Node->irCode.emplace_back(std::string(std::string(Node->irRes+" = alloca i32")));
            // Node->irCode.emplace_back(std::string("store i32 "+std::to_string(E)+", ptr "+Node->irRes));
        }
    }
}

void IRgen::generateIr(Program *Node,ASTNode* r,ASTNode* LastBlock) {
    for (auto child:Node->get_children()) {
        if (std::holds_alternative<ASTNode *>(child)) {
            auto ch=std::get<ASTNode *>(child);
            if (ch->node_type==TypeName::StructStmt) {
                r->irCodeStruct.emplace_back(ch->irCode);
            }else {
                r->irCode.emplace_back(ch->irCode);
            }
        }
    }
}

void IRgen::generateIr(InherentImplStmt *Node,ASTNode* r,ASTNode* LastBlock) {
    for (auto child:Node->get_children()) {
        if (std::holds_alternative<ASTNode *>(child)) {
            auto ch=std::get<ASTNode *>(child);
            if (ch->realType->typeKind==TypeName::StructType) {
                r->irCodeStruct.emplace_back(ch->irCode);
            }else {
                r->irCode.emplace_back(ch->irCode);
            }
        }
    }
}

void IRgen::generateIr(BinaryExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    if (Node->op=="=="||Node->op=="!="||Node->op=="<"||Node->op==">"||Node->op=="<="||Node->op==">=") {
        Node->irCode.emplace_back(Node->left->irCode);
        Node->irCode.emplace_back(Node->right->irCode);
        Node->irRes= "%v"+std::to_string(++variableNum);
        if (Node->left->realType->typeKind==TypeName::BasicType&&Node->right->realType->typeKind==TypeName::BasicType) {
            auto Basic_l=std::dynamic_pointer_cast<BasicType>(Node->left->realType);
            if (Basic_l->kind==TypeName::U32||Basic_l->kind==TypeName::Usize||Basic_l->kind==TypeName::Uint) {
                Node->irCode.emplace_back(Node->irRes+" = icmp "+Op_To_uString(Node->op)+" "+
                    Type_To_String(Node->left->realType)+" "+Node->left->irRes+", "+Node->right->irRes);
            }else {
                Node->irCode.emplace_back(Node->irRes+" = icmp "+Op_To_String(Node->op)+" "+
                    Type_To_String(Node->left->realType)+" "+Node->left->irRes+", "+Node->right->irRes);
            }
        }
    }else if (Node->op=="&&") {
        Node->irLabel1="%v"+std::to_string(++variableNum);
        Node->irLabel2="%v"+std::to_string(++variableNum);
        Node->irRes="%v"+std::to_string(++variableNum);
        Node->irRes_p="%v"+std::to_string(++variableNum);//一个临时值

        LastBlock->irCode.emplace_back(std::string(Node->irRes_p+" = alloca i1"));
        Store(Node->irCode,Node->irRes_p,"0",std::make_shared<BasicType>(TypeName::Bool));//先把目标值设置成0
        Node->irCode.emplace_back(Node->left->irCode);
        Node->irCode.emplace_back(std::string("br i1 "+Node->left->irRes+", label "+Node->irLabel1+", label "+Node->irLabel2));//判断左侧的代码会不会导致短路

        Node->irCode.emplace_back(std::string("\n"));
        Node->irCode.emplace_back(std::string("out"));
        Node->irCode.emplace_back(std::string(Node->irLabel1.substr(1)+":"));
        Node->irCode.emplace_back(std::string("in"));
        Node->irCode.emplace_back(Node->right->irCode);
        Store(Node->irCode,Node->irRes_p,Node->right->irRes,std::make_shared<BasicType>(TypeName::Bool));//能够来到此处就说明没有短路,那么结果就只看right的值
        Node->irCode.emplace_back(std::string("br label "+Node->irLabel2));

        Node->irCode.emplace_back(std::string("\n"));
        Node->irCode.emplace_back(std::string("out"));
        Node->irCode.emplace_back(std::string(Node->irLabel2.substr(1)+":"));
        Node->irCode.emplace_back(std::string("in"));
        Load(Node->irCode,Node->irRes,Node->irRes_p,std::make_shared<BasicType>(TypeName::Bool));//最终结果存到irRes中,也保证了irRes_p只被赋值过一遍
    }else if (Node->op=="||"){
        Node->irLabel1="%v"+std::to_string(++variableNum);
        Node->irLabel2="%v"+std::to_string(++variableNum);
        Node->irRes="%v"+std::to_string(++variableNum);
        Node->irRes_p="%v"+std::to_string(++variableNum);//一个临时值

        LastBlock->irCode.emplace_back(std::string(Node->irRes_p+" = alloca i1"));
        Store(Node->irCode,Node->irRes_p,"1",std::make_shared<BasicType>(TypeName::Bool));//先把目标值设置成0
        Node->irCode.emplace_back(Node->left->irCode);
        Node->irCode.emplace_back(std::string("br i1 "+Node->left->irRes+", label "+Node->irLabel2+", label "+Node->irLabel1));//如果为真,那么就直接并块,否则就跳转去判断下一个

        Node->irCode.emplace_back(std::string("\n"));
        Node->irCode.emplace_back(std::string("out"));
        Node->irCode.emplace_back(std::string(Node->irLabel1.substr(1)+":"));
        Node->irCode.emplace_back(std::string("in"));
        Node->irCode.emplace_back(Node->right->irCode);
        Store(Node->irCode,Node->irRes_p,Node->right->irRes,std::make_shared<BasicType>(TypeName::Bool));//能够来到此处就说明没有短路,那么结果就只看right的值
        Node->irCode.emplace_back(std::string("br label "+Node->irLabel2));

        Node->irCode.emplace_back(std::string("\n"));
        Node->irCode.emplace_back(std::string("out"));
        Node->irCode.emplace_back(std::string(Node->irLabel2.substr(1)+":"));
        Node->irCode.emplace_back(std::string("in"));
        Load(Node->irCode,Node->irRes,Node->irRes_p,std::make_shared<BasicType>(TypeName::Bool));//最终结果存到irRes中,也保证了irRes_p只被赋值过一遍
    }else {
        Node->irCode.emplace_back(Node->left->irCode);
        Node->irCode.emplace_back(Node->right->irCode);
        Node->irRes= "%v"+std::to_string(++variableNum);
        auto Ty=std::dynamic_pointer_cast<BasicType>(Node->realType);
        std::string op;
        if (Ty->kind==TypeName::Int||Ty->kind==TypeName::Iint||Ty->kind==TypeName::Float||Ty->kind==TypeName::I32||Ty->kind==TypeName::Isize) {
            op=Op_To_String(Node->op);
        }else {
            op=Op_To_uString(Node->op);
        }
        Node->irCode.emplace_back(std::string(Node->irRes+" = "+op+" "+Type_To_String(Node->realType)+" "+Node->left->irRes+", "+Node->right->irRes));
    }
}

void IRgen::generateIr(AssignmentExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    if (Node->op=="=") {
        Node->irCode.emplace_back(Node->left->irCode);
        Node->irCode.emplace_back(Node->right->irCode);
        Node->irRes= "()";
        if (Node->right->realType->typeKind!=TypeName::NeverType&&Node->right->realType->typeKind!=TypeName::UnitType) {
            Store(Node->irCode,Node->left->irRes_p,Node->right->irRes,Node->left->realType);
        }
    }else{
        Node->irCode.emplace_back(Node->left->irCode);
        Node->irCode.emplace_back(Node->right->irCode);
        Node->irRes= "()";
        std::string var="%v"+std::to_string(++variableNum);
        Node->irCode.emplace_back(std::string(var+" = "+Op_To_String(Node->op.substr(0,Node->op.size()-1))+" "+
            Type_To_String(Node->left->realType)+" "+Node->left->irRes+", "+Node->right->irRes));
        Store(Node->irCode,Node->left->irRes_p,var,Node->right->realType);
    }
}

void IRgen::generateIr(UnitExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    Node->irRes="()";
}

void IRgen::generateIr(ArrayInitExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    Node->irRes=Node->irRes_p="%v"+std::to_string(++variableNum);
    LastBlock->irCode.emplace_back(Node->irRes_p+" = alloca "+Type_To_String(Node->realType));
    for (auto i=0;i<Node->elements.size();i++) {//我怀疑这边可能有错,因为elements是一个数组,或许没有被成功遍历到
        Node->irCode.emplace_back(Node->elements[i]->irCode);
        std::string var="%v"+std::to_string(++variableNum);
        Node->irCode.emplace_back(var+" = getelementptr "+
            Type_To_String(Node->realType)+", ptr "+Node->irRes_p+", i32 0, i32 "+std::to_string(i));
        Store(Node->irCode, var,Node->elements[i]->irRes,Node->elements[i]->realType);
        //%v42 = getelementptr [3 x i32], ptr %arr_ptr, i32 0, i32 2,计算第几个位置,便于后续存储
    }
}

void IRgen::generateIr(ArraySimplifiedExpr*Node,ASTNode* r,ASTNode* LastBlock) {
    Node->irRes=Node->irRes_p="%v"+std::to_string(++variableNum);
    LastBlock->irCode.emplace_back(Node->irRes_p+" = alloca "+Type_To_String(Node->realType));
    Node->irCode.emplace_back(Node->element->irCode);
    auto len = std::any_cast<long long>(Node->length->eval);
    if (len<10) {
        for (auto i=0;i<len;i++) {//是否会爆栈？需不需要优化？
            std::string var="%v"+std::to_string(++variableNum);
            Node->irCode.emplace_back(var+" = getelementptr "+
            Type_To_String(Node->realType)+", ptr "+Node->irRes_p+", i32 0, i32 "+std::to_string(i));
            Store(Node->irCode, var,Node->element->irRes,Node->element->realType);
        }
    }else {//主动写成循环的形式
        auto LoopVar="%v"+std::to_string(++variableNum);
        auto CondLabel = "%v"+std::to_string(++variableNum);
        auto BlockLabel = "%v"+std::to_string(++variableNum);
        auto EndLabel = "%v"+std::to_string(++variableNum);
        LastBlock->irCode.emplace_back(std::string(LoopVar+" = alloca i32"));
        Store(Node->irCode,LoopVar,"0",std::make_shared<BasicType>(TypeName::I32));
        Node->irCode.emplace_back(std::string("br label "+CondLabel));

        Node->irCode.emplace_back(std::string("out"));
        Node->irCode.emplace_back(std::string(CondLabel.substr(1)+":"));
        Node->irCode.emplace_back(std::string("in"));
        auto LoadLoopVar = "%v"+std::to_string(++variableNum);
        Load(Node->irCode,LoadLoopVar,LoopVar,std::make_shared<BasicType>(TypeName::I32));
        auto CondVar = "%v"+std::to_string(++variableNum);
        Node->irCode.emplace_back(std::string(CondVar+" = icmp slt i32 "+LoadLoopVar+", "+std::to_string(len)));
        Node->irCode.emplace_back(std::string("br i1 "+CondVar+", label "+BlockLabel)+", label "+EndLabel);

        Node->irCode.emplace_back(std::string("out"));
        Node->irCode.emplace_back(std::string(BlockLabel.substr(1)+":"));
        Node->irCode.emplace_back(std::string("in"));
        auto ElemVar = "%v"+std::to_string(++variableNum);
        Node->irCode.emplace_back(std::string(ElemVar+" = getelementptr "+Type_To_String(Node->realType)+", ptr "+
            Node->irRes_p+", i32 0, i32 "+LoadLoopVar));
        Store(Node->irCode,ElemVar,Node->element->irRes,Node->element->realType);
        auto NewLoopVar = "%v"+std::to_string(++variableNum);
        Node->irCode.emplace_back(std::string(NewLoopVar+" = add i32 "+LoadLoopVar+", 1"));
        Store(Node->irCode,LoopVar,NewLoopVar,std::make_shared<BasicType>(TypeName::I32));
        Node->irCode.emplace_back(std::string("br label "+CondLabel));

        Node->irCode.emplace_back(std::string("out"));
        Node->irCode.emplace_back(std::string(EndLabel.substr(1)+":"));
        Node->irCode.emplace_back(std::string("in"));
    }

}

void IRgen::generateIr(ArrayAccessExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    if (Node->array->get_type()==TypeName::FieldAccessExpr||Node->array->get_type()==TypeName::ArrayAccessExpr) {
        Node->irCode.emplace_back(Node->array->irCode);
    }
    //a.b[]除外
    // 猛然发现array对应的是一个指针,也就是说我不应该有生成他的irCode,而是直接使用他的irRes
    Node->irCode.emplace_back(Node->index->irCode);
    Node->irRes_p=Node->irRes_p="%v"+std::to_string(++variableNum);
    auto T=Node->array->realType;
    if (T->typeKind==TypeName::ReferenceType) {//auto deref
        // Load(Node->irCode,Node->array->irRes,Node->array->irRes_p,T);//我其实不太清楚这样对不对
        T=T->typePtr;
        // Load(Node->irCode,,,);
    }//实际上并非需要自动解引用,如果传入的就是&的话,本质上还是指向数组的指针,实际上数组中真正有用的也是irRes_p
    Node->irCode.emplace_back(Node->irRes_p+" = getelementptr "+Type_To_String(T)+", ptr "+
        Node->array->irRes_p+", i32 0, i32 "+Node->index->irRes);
    if (is_BigType(Node->realType)) {
        Node->irRes=Node->irRes_p;
    }else {
        Node->irRes= "%v"+std::to_string(++variableNum);
        Load(Node->irCode,Node->irRes,Node->irRes_p,Node->realType);
    }
}

void IRgen::generateIr(LoopExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    if (Node->realType->typeKind==TypeName::UnitType||Node->realType->typeKind==TypeName::NeverType) {
        Node->irRes="()";
    }else {
        LastBlock->irCode.emplace_back(std::string(Node->irRes_p+" = alloca "+Type_To_String(Node->realType)));
    }
    Node->irCode.emplace_back(std::string("br label "+Node->irLabel1));
    Node->irCode.emplace_back(std::string("\n"));
    Node->irCode.emplace_back(std::string("out"));
    Node->irCode.emplace_back(std::string(Node->irLabel1.substr(1)+":"));
    Node->irCode.emplace_back(std::string("in"));
    Node->irCode.emplace_back(Node->block->irCode);
    if (Node->block->realType->typeKind!=TypeName::NeverType) {
        Node->irCode.emplace_back(std::string("br label "+Node->irLabel1));
    }

    Node->irCode.emplace_back(std::string("\n"));
    Node->irCode.emplace_back(std::string("out"));
    Node->irCode.emplace_back(std::string(Node->irLabel2.substr(1)+":"));
    Node->irCode.emplace_back(std::string("in"));

    if (Node->realType->typeKind!=TypeName::NeverType||Node->realType->typeKind!=TypeName::UnitType) {
        if (is_BigType(Node->realType)) {
            Node->irRes=Node->irRes_p;
        }else {
            Load(Node->irCode,Node->irRes,Node->irRes_p,Node->realType);
        }
    }
}

void IRgen::generateIr(WhileExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    if (Node->condition->eval.has_value()) {
        if (const auto B=std::any_cast<bool>(Node->condition->eval); B==1) {//恒为1,无限循环
            Node->irCode.emplace_back(std::string("br label "+Node->irLabel1));
            Node->irCode.emplace_back(std::string("\n"));
            Node->irCode.emplace_back(std::string("out"));
            Node->irCode.emplace_back(std::string(Node->irLabel1.substr(1)+":"));
            Node->irCode.emplace_back(std::string("in"));
            Node->irCode.emplace_back(Node->block->irCode);//假模假式把后续生成出来
            if (Node->block->realType->typeKind!=TypeName::NeverType) {
                Node->irCode.emplace_back(std::string("br label "+Node->irLabel1));
            }
            Node->irCode.emplace_back(std::string("\n"));
            Node->irCode.emplace_back(std::string("out"));
            Node->irCode.emplace_back(std::string(Node->irLabel2.substr(1)+":"));
            Node->irCode.emplace_back(std::string("in"));//假模假式的退出块
        }//如果是恒假就不生成了
    }else {
        std::string var="%v"+std::to_string(++variableNum);
        Node->irCode.emplace_back(std::string("br label "+Node->irLabel1));
        Node->irCode.emplace_back(std::string("\n"));
        Node->irCode.emplace_back(std::string("out"));
        Node->irCode.emplace_back(std::string(Node->irLabel1.substr(1)+":"));
        Node->irCode.emplace_back(std::string("in"));
        Node->irCode.emplace_back(Node->condition->irCode);
        Node->irCode.emplace_back(std::string("br i1 "+Node->condition->irRes+", label "+var+", label "+Node->irLabel2));//运行condition,然后判断跳转

        Node->irCode.emplace_back(std::string("\n"));
        Node->irCode.emplace_back(std::string("out"));
        Node->irCode.emplace_back(std::string(var.substr(1)+":"));//手动添加了var
        Node->irCode.emplace_back(std::string("in"));
        Node->irCode.emplace_back(Node->block->irCode);
        // if (Node->block->realType->typeKind!=TypeName::NeverType) {
            Node->irCode.emplace_back(std::string("br label "+Node->irLabel1));
        // }//判断为真,跳转到block中执行,然后返回判断
        //为啥nevertype在return的时候会出错，不能理解()
        Node->irCode.emplace_back(std::string("\n"));
        Node->irCode.emplace_back(std::string("out"));
        Node->irCode.emplace_back(std::string(Node->irLabel2.substr(1)+":"));
        Node->irCode.emplace_back(std::string("in"));//末尾是一个空跳转，也就是收束的部分，继续往后执行
    }
}

void IRgen::generateIr(GroupedExpr *Node, ASTNode *r, ASTNode *LastBlock) {
    Node->irCode.emplace_back(Node->expr->irCode);
    Node->irRes=Node->expr->irRes;
}


void IRgen::generateIr(ContinueExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    Node->irCode.emplace_back(std::string("br label "+Node->LoopPtr->irLabel1));
}

void IRgen::generateIr(BreakExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    if (Node->expr==nullptr) {//如果只是一个break
        Node->irCode.emplace_back(std::string("br label "+Node->LoopPtr->irLabel2));
    }else {
        Node->irCode.emplace_back(Node->expr->irCode);
        if (Node->expr->realType->typeKind!=TypeName::NeverType) {
            if (Node->expr->realType->typeKind!=TypeName::UnitType) {
                Store(Node->irCode,Node->LoopPtr->irRes_p,Node->expr->irRes,Node->expr->realType);
            }
            Node->irCode.emplace_back(std::string("br label "+Node->LoopPtr->irLabel2));
        }
    }
}

void IRgen::generateIr(LetStmt*Node,ASTNode* r,ASTNode* LastBlock) {
    if (Node->expr->realType->typeKind==TypeName::NeverType) {
        Node->irCode.emplace_back(Node->expr->irCode);
        Node->realType=Node->expr->realType;
    }else if (Node->expr->realType->typeKind!=TypeName::UnitType) {
        Node->irCode.emplace_back(Node->expr->irCode);
        LastBlock->irCode.emplace_back("%v"+std::to_string(Node->variableID)+" = alloca "+Type_To_String(Node->type->realType->typePtr));
        Store(Node->irCode,"%v"+std::to_string(Node->variableID),Node->expr->irRes,Node->expr->realType);
    }
}

void IRgen::generateIr(BlockExpr *Node,ASTNode* r,ASTNode* LastBlock){
    for (auto child:Node->get_children()) {
        if (std::holds_alternative<ASTNode *>(child)) {
            auto ch=std::get<ASTNode *>(child);
            if (ch->node_type==TypeName::FnStmt) {
                r->irCode.emplace_back(ch->irCode);
            }else if (ch->node_type==TypeName::StructStmt) {
                r->irCodeStruct.emplace_back(ch->irCode);
            }else {
                Node->irCode.emplace_back(ch->irCode);
            }
            if (ch->realType->typeKind==TypeName::NeverType) {
                Node->realType=ch->realType;
                break;
            }
        }
    }
    if (Node->realType->typeKind==TypeName::UnitType||Node->realType->typeKind==TypeName::NeverType) {
        Node->irRes = "()";
    }else {
        Node->irRes=Node->statements.back().first->irRes;
    }
}

void IRgen::generateIr(UnaryExpr *Node,ASTNode* r,ASTNode* LastBlock){
    if (Node->op=="-") {
        Node->irRes="%v"+std::to_string(++variableNum);
        Node->irCode.emplace_back(Node->right->irCode);
        Node->irCode.emplace_back(Node->irRes+" = sub "+Type_To_String(Node->realType)+" 0, "+Node->right->irRes);
    }else if (Node->op=="!") {
        Node->irRes="%v"+std::to_string(++variableNum);
        Node->irCode.emplace_back(Node->right->irCode);
        if (Node->realType->typeKind==TypeName::BasicType) {
            auto Basic_T=std::dynamic_pointer_cast<BasicType>(Node->realType);
            if (Basic_T->kind==TypeName::Bool) {
                Node->irCode.emplace_back(Node->irRes+" = xor "+Type_To_String(Node->realType)+" 1, "+Node->right->irRes);
            }else {
                Node->irCode.emplace_back(Node->irRes+" = xor "+Type_To_String(Node->realType)+" -1, "+Node->right->irRes);
            }
        }
    }else if (Node->op=="*") {
        if (Node->right->realType->typeKind!=TypeName::ReferenceType) {
            Node->irCode.emplace_back(Node->right->irCode);
        }
        if (!is_BigType(Node->realType)) {
            Node->irRes="%v"+std::to_string(++variableNum);
            if (Node->right->realType->typeKind==TypeName::ReferenceType) {
                Node->irRes_p=Node->right->irRes_p;
            }else {
                Node->irRes_p=Node->right->irRes;
            }
            Load(Node->irCode,Node->irRes,Node->irRes_p,Node->realType);
        }else {
            Node->irRes=Node->irRes_p=Node->right->irRes;
        }
    }else if (Node->op=="&") {
        if (LastBlock!=nullptr) {
            Node->irCode.emplace_back(Node->right->irCode);
            if (!is_BigType(Node->right->realType)) {
                if (Node->right->irRes_p.empty()) {
                    Node->irRes="%v"+std::to_string(++variableNum);
                    LastBlock->irCode.emplace_back(std::string(Node->irRes+" = alloca "+Type_To_String(Node->right->realType)));
                    Store(Node->irCode,Node->irRes,Node->right->irRes,Node->right->realType);
                }else {
                    Node->irRes_p=Node->irRes=Node->right->irRes_p;
                }
            }else {
                Node->irRes_p=Node->irRes=Node->right->irRes_p;
            }
        }
    }
}
void IRgen::generateIr(PathExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    if (Node->segments.size()==1) {
        if (Node->variableID<=6) {
            Node->irRes="@"+Node->segments[0];
        }else if (Node->realType->is_const) {
            Node->irRes=Node->realType->Const->irRes;
        }else{
            if (is_BigType(Node->realType)) {
                Node->irRes=Node->irRes_p="%v"+std::to_string(Node->variableID);//直接使用指针
            }else if (Node->realType->typeKind==TypeName::FunctionType) {
                auto Fn_T=std::dynamic_pointer_cast<FunctionType>(Node->realType);
                Node->irRes="@f"+std::to_string(Fn_T->funcnum);
            }else{//直接创造一个实例出来
                Node->irRes="%v"+std::to_string(++variableNum);
                Node->irRes_p="%v"+std::to_string(Node->variableID);
                Load(Node->irCode,Node->irRes,"%v"+std::to_string(Node->variableID),Node->realType);
            }
        }
    }else {//a::b()

    }


}

void IRgen::generateIr(EnumStmt *Node, ASTNode* r,ASTNode* LastBlock) {

}
void IRgen::generateIr(StructStmt *Node, ASTNode *r, ASTNode *LastBlock) {
    auto T=SemanticCheck::TypeToItem(Node->Struct_Type);
    auto Struct_T=std::dynamic_pointer_cast<StructType>(T);
    Node->irRes="%v"+std::to_string(Struct_T->structID);
    if (Struct_T->fields.empty()) {
        Node->irCode.emplace_back(Node->irRes+" = type { i8 }");
    }else {
        std::string res=Node->irRes+" = type { ";
        for (auto i=0;i<Struct_T->fields.size();i++) {
            if (i) {
                res+=", ";
            }
            res+=Type_To_String(Struct_T->fields[i].type);
        }
        res+=" }";
        Node->irCode.emplace_back(res);
    }
}

void IRgen::generateIr(StructExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    auto Struct_T=std::dynamic_pointer_cast<StructType>(Node->realType);
    Node->irRes=Node->irRes_p="%v"+std::to_string(++variableNum);
    LastBlock->irCode.emplace_back(Node->irRes_p+" = alloca %v"+std::to_string(Struct_T->structID));
    for (auto i=0;i<Struct_T->fields.size();i++) {
        Node->irCode.emplace_back(Node->apps[i].variable->irCode);
        Node->irCode.emplace_back("%v"+std::to_string(++variableNum)+" = getelementptr %v"+
            std::to_string(Struct_T->structID)+", ptr "+Node->irRes_p+", i32 0, i32 "+std::to_string(i));
        Store(Node->irCode,"%v"+std::to_string(variableNum),Node->apps[i].variable->irRes,Struct_T->fields[i].type);
    }
}

void IRgen::generateIr(ConstStmt *Node,ASTNode* r,ASTNode* LastBlock) {

}

void IRgen::generateIr(IfExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    if (Node->else_branch==nullptr) {//只有if
        Node->irCode.emplace_back(Node->condition->irCode);
        Node->irRes="()";
        if (Node->condition->eval.has_value()) {
            auto cond=std::any_cast<bool>(Node->condition->eval);
            if (cond) {
                Node->irCode.emplace_back(Node->then_branch->irCode);
            }
        }else {
            Node->irLabel1="%v"+std::to_string(++variableNum);
            Node->irLabel2="%v"+std::to_string(++variableNum);
            Node->irCode.emplace_back("br i1 "+Node->condition->irRes+", label "+Node->irLabel1+", label "+Node->irLabel2);

            Node->irCode.emplace_back(std::string("\n"));
            Node->irCode.emplace_back(std::string("out"));
            Node->irCode.emplace_back(std::string(Node->irLabel1.substr(1)+":"));
            Node->irCode.emplace_back(std::string("in"));
            Node->irCode.emplace_back(Node->then_branch->irCode);
            // if (Node->then_branch->realType->typeKind!=TypeName::NeverType) {
                Node->irCode.emplace_back("br label "+Node->irLabel2);
            // }
            //直接写吧,不管是否冗余了,我真的不知道应该怎么办了

            Node->irCode.emplace_back(std::string("\n"));
            Node->irCode.emplace_back(std::string("out"));
            Node->irCode.emplace_back(std::string(Node->irLabel2.substr(1)+":"));
            Node->irCode.emplace_back(std::string("in"));
        }
        // auto ve=trimString(IntegrateRes(Node->irCode,0));
        // for (const auto& basic_string:ve) {
        //     std::cerr<<basic_string<<std::endl;
        // }
    }else {
        Node->irCode.emplace_back(Node->condition->irCode);
        if (Node->condition->eval.has_value()) {
            auto cond=std::any_cast<bool>(Node->condition->eval);
            if (cond) {
                Node->irCode.emplace_back(Node->then_branch->irCode);
                Node->irRes=Node->then_branch->irRes;
            }else {
                Node->irCode.emplace_back(Node->else_branch->irCode);
                Node->irRes=Node->else_branch->irRes;
            }
        }else {
            if (Node->realType->typeKind==TypeName::NeverType||Node->realType->typeKind==TypeName::UnitType) {
                Node->irRes="()";
            }else {
                Node->irRes="%v"+std::to_string(++variableNum);
                Node->irRes_p="%v"+std::to_string(++variableNum);//一个临时值
                LastBlock->irCode.emplace_back(std::string(Node->irRes_p+" = alloca "+Type_To_String(Node->realType)));
            }
            Node->irLabel1="%v"+std::to_string(++variableNum);
            Node->irLabel2="%v"+std::to_string(++variableNum);
            auto Label3="%v"+std::to_string(++variableNum);
            Node->irCode.emplace_back(std::string("br i1 "+Node->condition->irRes+", label "+Node->irLabel1+", label "+Node->irLabel2));

            Node->irCode.emplace_back(std::string("\n"));
            Node->irCode.emplace_back(std::string("out"));
            Node->irCode.emplace_back(std::string(Node->irLabel1.substr(1)+":"));
            Node->irCode.emplace_back(std::string("in"));
            Node->irCode.emplace_back(Node->then_branch->irCode);
            if (Node->then_branch->realType->typeKind!=TypeName::NeverType&&Node->then_branch->realType->typeKind!=TypeName::UnitType) {
                Store(Node->irCode,Node->irRes_p,Node->then_branch->irRes,Node->then_branch->realType);
            }
            if (Node->then_branch->realType->typeKind!=TypeName::NeverType) {
                Node->irCode.emplace_back(std::string("br label "+Label3));
            }

            Node->irCode.emplace_back(std::string("\n"));
            Node->irCode.emplace_back(std::string("out"));
            Node->irCode.emplace_back(std::string(Node->irLabel2.substr(1)+":"));
            Node->irCode.emplace_back(std::string("in"));
            Node->irCode.emplace_back(Node->else_branch->irCode);
            if (Node->else_branch->realType->typeKind!=TypeName::NeverType&&Node->else_branch->realType->typeKind!=TypeName::UnitType) {
                Store(Node->irCode,Node->irRes_p,Node->else_branch->irRes,Node->else_branch->realType);
            }
            if (Node->else_branch->realType->typeKind!=TypeName::NeverType) {
                Node->irCode.emplace_back("br label "+Label3);
            }

            Node->irCode.emplace_back(std::string("\n"));
            Node->irCode.emplace_back(std::string("out"));
            Node->irCode.emplace_back(std::string(Label3.substr(1)+":"));
            Node->irCode.emplace_back(std::string("in"));

            if (Node->realType->typeKind!=TypeName::NeverType&&Node->realType->typeKind!=TypeName::UnitType) {
                if (is_BigType(Node->realType)) {
                    Node->irRes=Node->irRes_p;
                }else {
                    Node->irRes="%v"+std::to_string(++variableNum);
                    Load(Node->irCode,Node->irRes,Node->irRes_p,Node->realType);
                }
            }
        }
    }
}

void IRgen::generateIr(RustType *Node,ASTNode* r,ASTNode* LastBlock) {
    Node->irRes=Type_To_String(Node->realType);
}

void IRgen::generateIr(FnStmt *Node,ASTNode* r,ASTNode* LastBlock) {
    int paramNum[Node->parameters.size()+1];
    int return_param=-1;
    if (Node->variableID==1) {
        auto res="define "+Type_To_String(Node->return_type->realType)+" @main(";
        for (auto i=0;i<Node->parameters.size();i++) {
            if (i) {
                res+=", ";
            }
            res+=Type_To_String_ptr(Node->parameters[i].type);
            res+=" ";
            res+="%v"+std::to_string(++variableNum);
            paramNum[i]=variableNum;
        }
        res+=")";
        Node->irRes=res;
    }else if (Node->variableID==2) {
        auto res="define "+Type_To_String(Node->return_type->realType)+" @exit(";
        for (auto i=0;i<Node->parameters.size();i++) {
            if (i) {
                res+=", ";
            }
            res+=Type_To_String_ptr(Node->parameters[i].type);
            res+=" ";
            res+="%v"+std::to_string(++variableNum);
            paramNum[i]=variableNum;
        }
        res+=")";
        Node->irRes=res;
    }else if (Node->variableID==3) {
        auto res="define "+Type_To_String(Node->return_type->realType)+" @printInt(";
        for (auto i=0;i<Node->parameters.size();i++) {
            if (i) {
                res+=", ";
            }
            res+=Type_To_String_ptr(Node->parameters[i].type);
            res+=" ";
            res+="%v"+std::to_string(++variableNum);
            paramNum[i]=variableNum;
        }
        res+=")";
        Node->irRes=res;
    }else if (Node->variableID==4) {
        auto res="define "+Type_To_String(Node->return_type->realType)+" @printlnInt(";
        for (auto i=0;i<Node->parameters.size();i++) {
            if (i) {
                res+=", ";
            }
            res+=Type_To_String_ptr(Node->parameters[i].type);
            res+=" ";
            res+="%v"+std::to_string(++variableNum);
            paramNum[i]=variableNum;
        }
        res+=")";
        Node->irRes=res;
    }else if (Node->variableID==5) {
        auto res="define "+Type_To_String(Node->return_type->realType)+" @getInt(";
        for (auto i=0;i<Node->parameters.size();i++) {
            if (i) {
                res+=", ";
            }
            res+=Type_To_String_ptr(Node->parameters[i].type);
            res+=" ";
            res+="%v"+std::to_string(++variableNum);
            paramNum[i]=variableNum;
        }
        res+=")";
        Node->irRes=res;
    }else {
        auto res="define "+(is_BigType(Node->return_type->realType)?"void":Type_To_String(Node->return_type->realType))+" @f"+std::to_string(Node->variableID)+"(";
        for (auto i=0;i<Node->parameters.size();i++) {
            if (i) {
                res+=", ";
            }
            res+=Type_To_String_ptr(Node->parameters[i].type);
            if ((Node->parameters[i].type->typePtr->typeKind==TypeName::ReferenceType
                &&Node->parameters[i].type->typePtr->typePtr->typeKind==TypeName::ArrayType)||
                (Node->parameters[i].type->typePtr->typeKind==TypeName::ReferenceType
                &&Node->parameters[i].type->typePtr->typePtr->typeKind==TypeName::BasicType)||
                (Node->parameters[i].type->typePtr->typeKind==TypeName::StructType
                    &&Node->parameters[i].type->is_and)) {
                res+=" %v"+std::to_string(Node->parameters[i].varnum);//直接使用需要使用的这个ptr
                paramNum[i]=Node->parameters[i].varnum;
            }else {
                res+=" ";
                res+="%v"+std::to_string(++variableNum);
                paramNum[i]=variableNum;
            }
        }
        if (is_BigType(SemanticCheck::TypeToItem(Node->return_type->realType))) {
            if (!Node->parameters.empty()) {
                res+=", ";
            }
            res+="ptr "+Node->irRes_p;//如果是大类型,就用参数列表来返回
            return_param=variableNum;
        }
        res+=")";
        Node->irRes=res;
    }
    Node->irCode.emplace_back(Node->irRes);
    Node->irCode.emplace_back(std::string("{"));

    Node->irCode.emplace_back(std::string(Node->irLabel1.substr(1)+":"));
    Node->irCode.emplace_back(std::string("in"));
    std::string ans;
    //我需要把函数参数全部都拿回来,也就是处理参数的部分
    //我在scope里查到的是var的variableNum,存储在paramNum[i]中,也就是define(%v(++num)),然后要alloca paramNum[i],store (++num)->paramNum[i]
    for (auto i=0;i<Node->parameters.size();i++) {
        if (Node->parameters[i].type->is_and) {//只有大聪明的结构体在semantic自动解引用了()
            //TODO
            //这有啥需要干的???Reference就直接用那个指针,好极了
            // Node->irCode.emplace_back(std::string("%v"+std::to_string(Node->parameters[i].varnum)+" = alloca ptr"));
            // Node->irCode.emplace_back(std::string("store ptr %v"+std::to_string(paramNum[i])+",ptr %v"+std::to_string(Node->parameters[i].varnum)));
        }else if (Node->parameters[i].type->typePtr->typeKind==TypeName::ReferenceType) {
            auto Ref_T=std::dynamic_pointer_cast<ReferenceType>(Node->parameters[i].type->typePtr);
            auto realT=Ref_T->typePtr;
            if (is_BigType(realT)) {
                if (realT->typeKind==TypeName::StructType) {
                    auto Struct_T=std::dynamic_pointer_cast<StructType>(realT);
                    auto index=Struct_T->structID;
                    Node->irCode.emplace_back(std::string("%v"+std::to_string(Node->parameters[i].varnum)+" = alloca %v"+std::to_string(index)));
                    Store(Node->irCode,"%v"+std::to_string(Node->parameters[i].varnum),"%v"+std::to_string(paramNum[i]),realT);
                }else {
                    //TODO
                    //数组相关,就应该啥也不干!
                    // auto Array_T=std::dynamic_pointer_cast<ArrayType>(realT);
                    // Node->irCode.emplace_back(std::string("%v"+std::to_string(Node->parameters[i].varnum)+" = alloca "+Type_To_String(Array_T)));
                    // Store(Node->irCode,"%v"+std::to_string(Node->parameters[i].varnum),"%v"+std::to_string(paramNum[i]),realT);
                }
            }else {
                //这边其实也是啥也不干
                // Node->irCode.emplace_back(std::string("%v"+std::to_string(Node->parameters[i].varnum)+" = alloca "+Type_To_String(realT)));
                // Store(Node->irCode,"%v"+std::to_string(Node->parameters[i].varnum),"%v"+std::to_string(paramNum[i]),realT);
            }
        }else if (is_BigType(Node->parameters[i].type)) {
            if (Node->parameters[i].type->typePtr->typeKind==TypeName::StructType) {
                auto Struct_T=std::dynamic_pointer_cast<StructType>(Node->parameters[i].type->typePtr);
                auto index=Struct_T->structID;
                Node->irCode.emplace_back(std::string("%v"+std::to_string(Node->parameters[i].varnum)+" = alloca %v"+std::to_string(index)));
                Store(Node->irCode,"%v"+std::to_string(Node->parameters[i].varnum),"%v"+std::to_string(paramNum[i]),Node->parameters[i].type);
            }else {
                //TODO
                //数组相关
                auto Array_T=std::dynamic_pointer_cast<ArrayType>(Node->parameters[i].type->typePtr);
                Node->irCode.emplace_back(std::string("%v"+std::to_string(Node->parameters[i].varnum)+" = alloca "+Type_To_String(Array_T)));
                Store(Node->irCode,"%v"+std::to_string(Node->parameters[i].varnum),"%v"+std::to_string(paramNum[i]),Array_T);
            }
        }else{
            Node->irCode.emplace_back(std::string("%v"+std::to_string(Node->parameters[i].varnum)+" = alloca "+Type_To_String(Node->parameters[i].type)));
            Store(Node->irCode,"%v"+std::to_string(Node->parameters[i].varnum),"%v"+std::to_string(paramNum[i]),Node->parameters[i].type);
        }
    }

    Node->irCode.emplace_back(Node->body->irCode);
    // if (Node->body->realType->typeKind!=TypeName::NeverType) {
        if (Node->return_type->realType->typePtr->typeKind==TypeName::UnitType) {
            Node->irCode.emplace_back(std::string("ret void"));
        }else if (is_BigType(Node->return_type->realType->typePtr)) {
            if (Node->body->realType->typeKind!=TypeName::NeverType) {
                Store(Node->irCode,Node->irRes_p,Node->body->irRes,Node->body->realType);
            }//内部有一个return了我应该怎么办,是直接ret还是怎么办,关键如果return出现在中间的话会很麻烦
            Node->irCode.emplace_back(std::string("ret void"));
        }else {
            if (Node->body->realType->typeKind!=TypeName::NeverType) {
                Node->irCode.emplace_back(std::string("ret "+Type_To_String(Node->return_type->realType)+" "+Node->body->irRes));
            }
        }
    // }
    Node->irCode.emplace_back(std::string("out"));
    Node->irCode.emplace_back(std::string("}"));
    Node->irCode.emplace_back(std::string("\n"));
}


void IRgen::generateIr(ReturnExpr *Node,ASTNode* r,ASTNode* LastBlock){
    if (!Node->expr) {
        Node->irCode.emplace_back(std::string("ret void"));
    }else if (is_BigType(Node->expr->realType)) {
        Node->irCode.emplace_back(Node->expr->irCode);
        Store(Node->irCode,Node->FnPtr->irRes_p,Node->expr->irRes,Node->expr->realType);
    }else {
        Node->irCode.emplace_back(Node->expr->irCode);
        if (Node->expr->realType->typeKind!=TypeName::NeverType) {
            Node->irCode.emplace_back(std::string("ret "+Type_To_String(Node->expr->realType)+" "+Node->expr->irRes));
        }
    }
}

void IRgen::generateIr(AsExpr *Node, ASTNode *r, ASTNode *LastBlock) {
    if (!Node->expr->irCode.empty()) {//如果是Literal的话就不存在irCode,这个时候就不需要加进去,而是后续可以直接被使用
        Node->irCode.emplace_back(Node->expr->irCode);
    }
    auto Type=Node->realType;
    auto Basic_Type=std::dynamic_pointer_cast<BasicType>(Node->realType);
    auto T=Node->expr->realType;
    auto Basic_T=std::dynamic_pointer_cast<BasicType>(T);
    if (Basic_T->kind==TypeName::Char&&(Basic_Type->kind==TypeName::Int||Basic_Type->kind==TypeName::Uint||
        Basic_Type->kind==TypeName::Isize||Basic_Type->kind==TypeName::Usize||
        Basic_Type->kind==TypeName::U32||Basic_Type->kind==TypeName::I32) ) {
        Node->irRes="%v"+std::to_string(++variableNum);
        Node->irCode.emplace_back(std::string(Node->irRes+" = zext i8 "+Node->expr->irRes+" to i32"));
    }else if (Basic_T->kind==TypeName::Bool&&(Basic_Type->kind==TypeName::Int||Basic_Type->kind==TypeName::Uint||
            Basic_Type->kind==TypeName::Isize||Basic_Type->kind==TypeName::Usize||
            Basic_Type->kind==TypeName::U32||Basic_Type->kind==TypeName::I32) ) {
        Node->irRes="%v"+std::to_string(++variableNum);
        Node->irCode.emplace_back(std::string(Node->irRes+" = zext i1 "+Node->expr->irRes+" to i32"));
    }else {
            Node->irRes=Node->expr->irRes;
            Node->irRes_p=Node->expr->irRes_p;
    }
}

void IRgen::generateIr(FieldAccessExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    if (Node->struct_name->get_type()==TypeName::FieldAccessExpr) {
        Node->irCode.emplace_back(Node->struct_name->irCode);
        auto FieldAccess=std::dynamic_pointer_cast<FieldAccessExpr>(Node->struct_name);
        if (Node->field_expr->get_type()==TypeName::PathExpr) {
            auto PP=std::dynamic_pointer_cast<PathExpr>(Node->field_expr);
            Node->irRes_p="%v"+std::to_string(++variableNum);
            auto Struct_T=std::dynamic_pointer_cast<StructType>(FieldAccess->realType);
            int index=0;
            for (auto i=0;i<Struct_T->fields.size();i++) {
                if (Struct_T->fields[i].name==PP->segments[0]) {
                    index=i;
                    break;
                }
            }
            Node->irCode.emplace_back(Node->irRes_p+" = getelementptr %v"+
               std::to_string(Struct_T->structID)+", ptr "+FieldAccess->irRes+", i32 0, i32 "+std::to_string(index));
            if (is_BigType(Node->realType)) {//决定直接用指针还是load值
                Node->irRes=Node->irRes_p;
            }else {
                Node->irRes="%v"+std::to_string(++variableNum);
                Load(Node->irCode,Node->irRes,Node->irRes_p,Node->realType);
            }
        }
    }

    if (Node->struct_name->get_type()==TypeName::ArrayAccessExpr) {//a[b].c
        Node->irCode.emplace_back(Node->struct_name->irCode);
        auto ArrayAccess=std::dynamic_pointer_cast<ArrayAccessExpr>(Node->struct_name);
        if (Node->field_expr->get_type()==TypeName::PathExpr) {
            auto PP=std::dynamic_pointer_cast<PathExpr>(Node->field_expr);
            Node->irRes_p="%v"+std::to_string(++variableNum);
            auto Struct_T=std::dynamic_pointer_cast<StructType>(ArrayAccess->realType);
            int index=0;
            for (auto i=0;i<Struct_T->fields.size();i++) {
                if (Struct_T->fields[i].name==PP->segments[0]) {
                    index=i;
                    break;
                }
            }
            Node->irCode.emplace_back(Node->irRes_p+" = getelementptr %v"+
                std::to_string(Struct_T->structID)+", ptr "+ArrayAccess->irRes+", i32 0, i32 "+std::to_string(index));
            if (is_BigType(Node->realType)) {//决定直接用指针还是load值
                Node->irRes=Node->irRes_p;
            }else if (Node->realType->typeKind==TypeName::FunctionType) {
                // std::cerr<<"hilo"<<std::endl;
                auto Fn_T=std::dynamic_pointer_cast<FunctionType>(Node->realType);
                Node->irRes="@f"+std::to_string(Fn_T->funcnum);
                Node->irCode.clear();
            }else{
                Node->irRes="%v"+std::to_string(++variableNum);
                Load(Node->irCode,Node->irRes,Node->irRes_p,Node->realType);
            }
        }
    }

    if (Node->struct_name->get_type()==TypeName::CallExpr) {//a.b().c()
        // std::cerr<<"hillo"<<std::endl;
        auto CT=std::dynamic_pointer_cast<CallExpr>(Node->struct_name);
        if (Node->field_expr->get_type()==TypeName::PathExpr) {
            auto PP=std::dynamic_pointer_cast<PathExpr>(Node->field_expr);
            Node->irRes_p="%v"+std::to_string(++variableNum);
            auto Struct_T=std::dynamic_pointer_cast<StructType>(CT->realType);
            int index=0;
            for (auto i=0;i<Struct_T->fields.size();i++) {
                if (Struct_T->fields[i].name==PP->segments[0]) {
                    index=i;
                    break;
                }
            }
            Node->irCode.emplace_back(Node->irRes_p+" = getelementptr %v"+
                std::to_string(Struct_T->structID)+", ptr "+CT->irRes+", i32 0, i32 "+std::to_string(index));
            if (is_BigType(Node->realType)) {//决定直接用指针还是load值
                Node->irRes=Node->irRes_p;
            }else if (Node->realType->typeKind==TypeName::FunctionType) {
                // std::cerr<<"hilo"<<std::endl;
                auto Fn_T=std::dynamic_pointer_cast<FunctionType>(Node->realType);
                Node->irRes="@f"+std::to_string(Fn_T->funcnum);
                Node->irCode.clear();
            }else{
                Node->irRes="%v"+std::to_string(++variableNum);
                Load(Node->irCode,Node->irRes,Node->irRes_p,Node->realType);
            }
        }
    }
    //这边也没有写auto deref
    if (Node->struct_name->get_type() == TypeName::PathExpr) {//a.a
        auto Pt=std::dynamic_pointer_cast<PathExpr>(Node->struct_name);
        if (Node->field_expr->get_type() == TypeName::PathExpr) {
            auto PP=std::dynamic_pointer_cast<PathExpr>(Node->field_expr);
            Node->irRes_p="%v"+std::to_string(++variableNum);
            auto T=Pt->realType;
            if (T->typeKind==TypeName::ReferenceType) {
                // Load(Node->irCode,Node->struct_name->irRes,Node->struct_name->irRes_p,T);
                T=T->typePtr;
            }
            auto Struct_T=std::dynamic_pointer_cast<StructType>(T);
            int index=0;
            for (auto i=0;i<Struct_T->fields.size();i++) {
                if (Struct_T->fields[i].name==PP->segments[0]) {
                    index=i;
                    break;
                }
            }
            Node->irCode.emplace_back(Node->irRes_p+" = getelementptr %v"+
                std::to_string(Struct_T->structID)+", ptr %v"+
                std::to_string(Pt->Id)+", i32 0, i32 "+std::to_string(index));
            if (is_BigType(Node->realType)) {//决定直接用指针还是load值
                Node->irRes=Node->irRes_p;
            }else if (Node->realType->typeKind==TypeName::FunctionType) {
                // std::cerr<<"hilo"<<std::endl;
                auto Fn_T=std::dynamic_pointer_cast<FunctionType>(Node->realType);
                Node->irRes="@f"+std::to_string(Fn_T->funcnum);
                Node->irCode.clear();
            }else {
                Node->irRes="%v"+std::to_string(++variableNum);
                Load(Node->irCode,Node->irRes,Node->irRes_p,Node->realType);
            }
        }else {//不然就是函数

        }


    }
}
void IRgen::generateIr(CallExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    if (Node->receiver->get_type() == TypeName::PathExpr) {
        auto Pnode = std::dynamic_pointer_cast<PathExpr>(Node->receiver);
        if (Pnode->segments.size() == 1) {
            if (Pnode->segments[0]!="exit"&&Pnode->segments[0]!="main"&&Pnode->segments[0]!="printInt"&&Pnode->segments[0]!="printlnInt"&&Pnode->segments[0]!="getInt") {
                Node->irCode.emplace_back(Node->receiver->irCode);
            }
            std::string paramRes="";
            for (auto i=0;i<Node->arguments.size();i++) {
                if (i) {
                    paramRes+=", ";
                }
                if (Node->arguments[i]->realType->typeKind==TypeName::ReferenceType) {//注意如果是reference就不是把值取出来,而是直接把对应指针传进去
                    if (Node->arguments[i]->get_type()==TypeName::UnaryExpr) {//好歹特判一下把irCode加进来
                        Node->irCode.emplace_back(Node->arguments[i]->irCode);
                    }
                    paramRes+=Type_To_String_ptr(Node->arguments[i]->realType);//由于函数初始化的方式,此处如果是大类型也应该传ptr
                    paramRes+=" ";
                    paramRes+=Node->arguments[i]->irRes_p;
                }else {
                    Node->irCode.emplace_back(Node->arguments[i]->irCode);
                    paramRes+=Type_To_String_ptr(Node->arguments[i]->realType);//由于函数初始化的方式,此处如果是大类型也应该传ptr
                    paramRes+=" ";
                    paramRes+=Node->arguments[i]->irRes;
                }

            }
            if (is_BigType(Node->realType)) {
                Node->irRes_p = Node->irRes = "%v"+std::to_string(++variableNum);
                if (!paramRes.empty()) {
                    paramRes+=", ";
                }
                paramRes+="ptr "+Node->irRes;
                LastBlock->irCode.emplace_back(std::string(Node->irRes+" = alloca "+Type_To_String(Node->realType)));//如果返回值是struct或者数组,应该用一个ptr来接
                Node->irCode.emplace_back(std::string("call void "+Node->receiver->irRes+"("+paramRes+")"));
            }else {
                if (Node->realType->typeKind!=TypeName::UnitType) {
                    Node->irRes = "%v"+std::to_string(++variableNum);
                    if (is_BigType(Node->realType)) {
                        Node->irRes_p = Node->irRes ;
                    }
                    Node->irCode.emplace_back(std::string(Node->irRes+" = call "+Type_To_String(Node->realType)+" "+Node->receiver->irRes+"("+paramRes+")"));
                }else {
                    Node->irRes="()";
                    Node->irCode.emplace_back(std::string("call void "+Node->receiver->irRes+"("+paramRes+")"));
                }
            }
        } else {//Edge::new(0,0,0,0)
            // std::cerr<<"hillo!"<<std::endl;
            auto Fn_T=std::dynamic_pointer_cast<FunctionType>(Pnode->realType);
            std::string paramRes="";
            for (auto i=0;i<Node->arguments.size();i++) {
                if (i) {
                    paramRes+=", ";
                }
                Node->irCode.emplace_back(Node->arguments[i]->irCode);
                paramRes+=Type_To_String_ptr(Node->arguments[i]->realType);//由于函数初始化的方式,此处如果是大类型也应该传ptr
                paramRes+=" ";
                paramRes+=Node->arguments[i]->irRes;
            }
            if (is_BigType(Node->realType)) {
                Node->irRes_p = Node->irRes = "%v"+std::to_string(++variableNum);
                if (!paramRes.empty()) {
                    paramRes+=", ";
                }
                paramRes+="ptr "+Node->irRes;
                LastBlock->irCode.emplace_back(std::string(Node->irRes+" = alloca "+Type_To_String(Node->realType)));//如果返回值是struct或者数组,应该用一个ptr来接//这边就应该定义一个指向struct的数组呀！！！
                Node->irCode.emplace_back(std::string("call void @f"+std::to_string(Fn_T->funcnum)+"("+paramRes+")"));
            }else {
                if (Node->realType->typeKind!=TypeName::UnitType) {
                    Node->irRes = "%v"+std::to_string(++variableNum);
                    if (is_BigType(Node->realType)) {
                        Node->irRes_p = Node->irRes ;
                    }
                    Node->irCode.emplace_back(std::string(Node->irRes+" = call "+Type_To_String(Node->realType)+" @f"+std::to_string(Fn_T->funcnum)+"("+paramRes+")"));
                }else {
                    Node->irRes="()";
                    Node->irCode.emplace_back(std::string("call void @f"+std::to_string(Fn_T->funcnum)+"("+paramRes+")"));
                }
            }
        }
    }

    else if (Node->receiver->get_type() == TypeName::FieldAccessExpr) {//a.b(),也就是method,需要在第一位就传进去a
        auto FA=std::dynamic_pointer_cast<FieldAccessExpr>(Node->receiver);
        Node->irCode.emplace_back(FA->struct_name->irCode);
        // std::cerr<<"hillo1"<<std::endl;
        std::string paramRes="";
        paramRes+="ptr "+FA->struct_name->irRes;
        for (auto i=0;i<Node->arguments.size();i++) {
            paramRes+=", ";
            if (Node->arguments[i]->realType->typeKind==TypeName::ReferenceType) {//注意如果是reference就不是把值取出来,而是直接把对应指针传进去
                if (Node->arguments[i]->get_type()==TypeName::UnaryExpr) {//好歹特判一下把irCode加进来
                    Node->irCode.emplace_back(Node->arguments[i]->irCode);
                }
                paramRes+=Type_To_String_ptr(Node->arguments[i]->realType);//由于函数初始化的方式,此处如果是大类型也应该传ptr
                paramRes+=" ";
                paramRes+=Node->arguments[i]->irRes_p;
            }else {
                Node->irCode.emplace_back(Node->arguments[i]->irCode);
                paramRes+=Type_To_String_ptr(Node->arguments[i]->realType);//由于函数初始化的方式,此处如果是大类型也应该传ptr
                paramRes+=" ";
                paramRes+=Node->arguments[i]->irRes;
            }

        }
        if (is_BigType(Node->realType)) {
            Node->irRes_p = Node->irRes = "%v"+std::to_string(++variableNum);
            if (!paramRes.empty()) {
                paramRes+=", ";
            }
            paramRes+="ptr "+Node->irRes;
            LastBlock->irCode.emplace_back(std::string(Node->irRes+" = alloca "+Type_To_String_ptr(Node->realType)));//如果返回值是struct或者数组,应该用一个ptr来接
            Node->irCode.emplace_back(std::string("call void "+Node->receiver->irRes+"("+paramRes+")"));
            // std::cerr<<Node->receiver->irRes<<std::endl;
        }else {
            if (Node->realType->typeKind!=TypeName::UnitType) {
                Node->irRes = "%v"+std::to_string(++variableNum);
                if (is_BigType(Node->realType)) {
                    Node->irRes_p = Node->irRes ;
                }
                Node->irCode.emplace_back(std::string(Node->irRes+" = call "+Type_To_String(Node->realType)+" "+Node->receiver->irRes+"("+paramRes+")"));
                // std::cerr<<Node->receiver->irRes<<std::endl;
            }else {
                Node->irRes="()";
                Node->irCode.emplace_back(std::string("call void "+Node->receiver->irRes+"("+paramRes+")"));
                // std::cerr<<Node->receiver->irRes<<std::endl;
            }
        }
    }

    else {

    }
}

std::vector<std::string> IRgen::IntegrateRes(std::vector<std::any> res, int dent) {
    std::vector<std::string> lines;
    for (auto element:res) {
        if (element.type()==typeid(std::string)) {
            auto stri=std::any_cast<std::string>(element);
            if (stri=="in") {
                dent+=2;
            }else if (stri=="out") {
                dent-=2;
            }else if(stri=="\n") {
                lines.emplace_back(std::string(""));
            }else {
                lines.emplace_back(std::string(SetDent(dent)+stri));
            }
        }else {
            auto morelines=IntegrateRes(std::any_cast<std::vector<std::any>>(element),dent);
            for (auto line:morelines) {
                lines.emplace_back(line);
            }
        }
    }
    return lines;
}

std::vector<std::string> IRgen::trimString(const std::vector<std::string> res) {
    std::vector<std::string> ans;
    int lastLabel=0;
    for (auto i=0;i<res.size();i++) {
        if (res[i].empty()) {
            continue;
        }
        if (res[i]=="}") {
            ans.emplace_back("}");
            lastLabel=1+i;
        }else if (res[i].back()==':') {
            lastLabel=i;
        }else {
            while (lastLabel<=i) {
                ans.push_back(res[lastLabel]);
                lastLabel++;
            }
        }
    }
    return ans;
}

std::vector<std::any> loadBuiltin() {
    std::vector<std::any> res;
    res.emplace_back(std::string("target triple = \"x86_64-unknown-linux-gnu\""));
    res.emplace_back(std::string("declare void @llvm.memcpy.p0.p0.i32(ptr, ptr, i32, i1)"));
    res.emplace_back(std::string("@.str = private unnamed_addr constant [3 x i8] c\"%d\\00\", align 1"));
    res.emplace_back(std::string("@.str.1 = private unnamed_addr constant [4 x i8] c\"%d\\0A\\00\", align 1"));
    res.emplace_back(std::string("define dso_local void @printInt(i32 noundef %x) local_unnamed_addr {"));
    res.emplace_back(std::string("entry:"));
    res.emplace_back(std::string("  %call = tail call noundef i32 (ptr, ...) @printf(ptr noundef nonnull @.str, i32 noundef %x)"));
    res.emplace_back(std::string("  ret void"));
    res.emplace_back(std::string("}"));
    res.emplace_back(std::string("declare noundef i32 @printf(ptr noundef, ...) local_unnamed_addr"));
    res.emplace_back(std::string("define dso_local void @printlnInt(i32 noundef %x) local_unnamed_addr {"));
    res.emplace_back(std::string("entry:"));
    res.emplace_back(std::string("  %call = tail call noundef i32 (ptr, ...) @printf(ptr noundef nonnull @.str.1, i32 noundef %x)"));
    res.emplace_back(std::string("  ret void"));
    res.emplace_back(std::string("}"));
    res.emplace_back(std::string("define dso_local i32 @getInt() local_unnamed_addr {"));
    res.emplace_back(std::string("entry:"));
    res.emplace_back(std::string("  %addr = alloca i32"));
    res.emplace_back(std::string("  %call = call noundef i32 (ptr, ...) @scanf(ptr noundef nonnull @.str, ptr noundef nonnull %addr)"));
    res.emplace_back(std::string("  %result = load i32, ptr %addr"));
    res.emplace_back(std::string("  ret i32 %result"));
    res.emplace_back(std::string("}"));
    res.emplace_back(std::string("declare noundef i32 @scanf(ptr noundef, ...) local_unnamed_addr"));
    res.emplace_back(std::string("declare void @exit(i32 noundef) local_unnamed_addr"));
    res.emplace_back(std::string("\n"));
    return res;
}

std::vector<std::string> IRgen::generateIr(ASTNode* root)
{
    pre_process(root, root, nullptr);
    root->IR(*this,root,nullptr);
    int dent = 0;
    auto res = loadBuiltin();
    if (!root->irCodeStruct.empty()) {
        root->irCodeStruct.emplace_back(std::string("\n"));
    }
    for (const auto& code:root->irCodeStruct) {
        res.emplace_back(code);
    }
    for (const auto& code:root->irCode) {
        res.emplace_back(code);
    }
    auto ve=trimString(IntegrateRes(res, dent));
    return ve;
}