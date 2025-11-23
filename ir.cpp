//
// Created by ckjsuperhh6602 on 2025/11/12.
//
#include "ir.h"
#include<iostream>

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

std::string Op_To_uString(const std::string& Op) {
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
            Basic_T->kind==TypeName::I32||Basic_T->kind==TypeName::Usize||Basic_T->kind==TypeName::Isize) {
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
    return "illegal type";
}

bool is_BigType(std::shared_ptr<Type> type) {
    if (type->typeKind==TypeName::ArrayType||type->typeKind==TypeName::StructType) {
        return true;
    }
    return false;
}

void MemCopy(std::vector<std::any> res,std::string src,std::string dest,std::shared_ptr<Type> type) {
    auto size_p="%c"+std::to_string(++variableNum);
    auto var="%c"+std::to_string(++variableNum);
    res.emplace_back(size_p+" = getelementptr "+Type_To_String(type)+", ptr null, i32 1");
    res.emplace_back(var+" = ptrtoint ptr "+size_p+" to i32");
    res.emplace_back("call void @llvm.memcpy.p0.p0.i32(ptr " +dest+", ptr "+src+", i32 "+var+", i1 false");
}

void Load(std::vector<std::any> res,std::string src,std::string dest,std::shared_ptr<Type> type) {
    if (is_BigType(type)) {
        MemCopy(res,src,dest,type);
    }else {
        res.emplace_back(dest+" = load "+Type_To_String(type)+", ptr "+src);
    }
}

void Store(std::vector<std::any> res,std::string src,std::string dest,std::shared_ptr<Type> type) {
    if (is_BigType(type)) {
        MemCopy(res,src,dest,type);
    }else {
        res.emplace_back("store "+Type_To_String(type)+" "+src+", ptr "+dest);
    }
}

std::string SetDent(int i) {
    std::string res;
    while (i--) {
        res.push_back(' ');
    }
    return res;
}

void pre_process(ASTNode* Node,ASTNode* r,ASTNode* LastBlock) {





    for (auto child:Node->get_children()) {
        if (std::holds_alternative<ASTNode *>(child)) {
            auto ch=std::get<ASTNode *>(child);
            pre_process(ch,r,LastBlock);
            ch->IR(r,LastBlock);
        }
    }
}

void generateIr(LiteralExpr *Node,ASTNode* r,ASTNode* LastBlock) {

}

void generateIr(Program *Node,ASTNode* r,ASTNode* LastBlock) {
    for (auto child:Node->get_children()) {
        if (std::holds_alternative<ASTNode *>(child)) {
            auto ch=std::get<ASTNode *>(child);
            if (ch->realType->typeKind==TypeName::StructType) {
                r->irCodeStruct.emplace_back(&ch->irCode);
            }else {
                r->irCode.emplace_back(&ch->irCode);
            }
        }
    }
}

void generateIr(InherentImplStmt *Node,ASTNode* r,ASTNode* LastBlock) {
    for (auto child:Node->get_children()) {
        if (std::holds_alternative<ASTNode *>(child)) {
            auto ch=std::get<ASTNode *>(child);
            if (ch->realType->typeKind==TypeName::StructType) {
                r->irCodeStruct.emplace_back(&ch->irCode);
            }else {
                r->irCode.emplace_back(&ch->irCode);
            }
        }
    }
}

void generateIr(BinaryExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    if (Node->op=="=="||Node->op=="!="||Node->op=="<"||Node->op==">"||Node->op=="<="||Node->op==">=") {
        Node->irCode.emplace_back(&Node->left->irCode);
        Node->irCode.emplace_back(&Node->right->irCode);
        Node->irRes= "%v"+std::to_string(++variableNum);
        if (Node->left->realType->typeKind==TypeName::BasicType&&Node->right->realType->typeKind==TypeName::BasicType) {
            auto Basic_l=std::any_cast<BasicType *>(Node->left->realType);
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

        LastBlock->irCode.emplace_back(Node->irRes_p+" = alloca i1");
        Store(Node->irCode,Node->irRes_p,"0",std::make_shared<BasicType>(TypeName::Bool));//先把目标值设置成0
        Node->irCode.emplace_back(&Node->left->irCode);
        Node->irCode.emplace_back("br i1 "+Node->left->irRes+", label "+Node->irLabel1+", label "+Node->irLabel2);//判断左侧的代码会不会导致短路

        Node->irCode.emplace_back("\n");
        Node->irCode.emplace_back("out");
        Node->irCode.emplace_back(Node->irLabel1.substr(1)+":");
        Node->irCode.emplace_back("in");
        Node->irCode.emplace_back(&Node->right->irCode);
        Store(Node->irCode,Node->irRes_p,Node->right->irRes,std::make_shared<BasicType>(TypeName::Bool));//能够来到此处就说明没有短路,那么结果就只看right的值
        Node->irCode.emplace_back("br label "+Node->irLabel2);

        Node->irCode.emplace_back("\n");
        Node->irCode.emplace_back("out");
        Node->irCode.emplace_back(Node->irLabel2.substr(1)+":");
        Node->irCode.emplace_back("in");
        Load(Node->irCode,Node->irRes,Node->irRes_p,std::make_shared<BasicType>(TypeName::Bool));//最终结果存到irRes中,也保证了irRes_p只被赋值过一遍
    }else if (Node->op=="||"){
        Node->irLabel1="%v"+std::to_string(++variableNum);
        Node->irLabel2="%v"+std::to_string(++variableNum);
        Node->irRes="%v"+std::to_string(++variableNum);
        Node->irRes_p="%v"+std::to_string(++variableNum);//一个临时值

        LastBlock->irCode.emplace_back(Node->irRes_p+" = alloca i1");
        Store(Node->irCode,Node->irRes_p,"1",std::make_shared<BasicType>(TypeName::Bool));//先把目标值设置成0
        Node->irCode.emplace_back(&Node->left->irCode);
        Node->irCode.emplace_back("br i1 "+Node->left->irRes+", label "+Node->irLabel2+", label "+Node->irLabel1);//如果为真,那么就直接并块,否则就跳转去判断下一个

        Node->irCode.emplace_back("\n");
        Node->irCode.emplace_back("out");
        Node->irCode.emplace_back(Node->irLabel1.substr(1)+":");
        Node->irCode.emplace_back("in");
        Node->irCode.emplace_back(&Node->right->irCode);
        Store(Node->irCode,Node->irRes_p,Node->right->irRes,std::make_shared<BasicType>(TypeName::Bool));//能够来到此处就说明没有短路,那么结果就只看right的值
        Node->irCode.emplace_back("br label "+Node->irLabel2);

        Node->irCode.emplace_back("\n");
        Node->irCode.emplace_back("out");
        Node->irCode.emplace_back(Node->irLabel2.substr(1)+":");
        Node->irCode.emplace_back("in");
        Load(Node->irCode,Node->irRes,Node->irRes_p,std::make_shared<BasicType>(TypeName::Bool));//最终结果存到irRes中,也保证了irRes_p只被赋值过一遍
    }else {
        Node->irCode.emplace_back(&Node->left->irCode);
        Node->irCode.emplace_back(&Node->right->irCode);
        Node->irRes= "%v"+std::to_string(++variableNum);
        auto Ty=dynamic_pointer_cast<BasicType>(Node->realType);
        std::string op;
        if (Ty->kind==TypeName::Int||Ty->kind==TypeName::Iint||Ty->kind==TypeName::Float||Ty->kind==TypeName::I32||Ty->kind==TypeName::Isize) {
            op=Op_To_String(Node->op);
        }else {
            op=Op_To_uString(Node->op);
        }
        Node->irCode.emplace_back(Node->irRes+" = "+op+" "+Type_To_String(Node->realType)+" "+Node->left->irRes+", "+Node->right->irRes);
    }
}

void generateIr(AssignmentExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    if (Node->op=="=") {
        Node->irCode.emplace_back(&Node->left->irCode);
        Node->irCode.emplace_back(&Node->right->irCode);
        Node->irRes= "()";
        if (Node->right->realType->typeKind!=TypeName::NeverType&&Node->right->realType->typeKind!=TypeName::UnitType) {
            Store(Node->irCode,Node->left->irRes_p,Node->right->irRes,Node->left->realType);
        }
    }else{
        Node->irCode.emplace_back(&Node->left->irCode);
        Node->irCode.emplace_back(&Node->right->irCode);
        Node->irRes= "()";
        std::string var=std::to_string(++variableNum);
        Node->irCode.emplace_back(var+" = "+Op_To_String(Node->op.substr(0,Node->op.size()-1))+" "+
            Type_To_String(Node->realType)+" "+Node->left->irRes+", "+Node->right->irRes);
        Store(Node->irCode,Node->left->irRes_p,var,Node->right->realType);
    }
}

void generateIr(UnitExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    Node->irRes="()";
}

void generateIr(ArrayInitExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    Node->irRes=Node->irRes_p="%v"+std::to_string(++variableNum);
    LastBlock->irCode.emplace_back(Node->irRes_p+" = alloca "+Type_To_String(Node->realType));
    for (auto i=0;i<Node->elements.size();i++) {//我怀疑这边可能有错,因为elements是一个数组,或许没有被成功遍历到
        Node->irCode.emplace_back(&Node->elements[i]->irCode);
        std::string var=std::to_string(++variableNum);
        Node->irCode.emplace_back(var+" = getelementptr "+
            Type_To_String(Node->realType)+", ptr "+Node->irRes_p+", i32 0, i32 "+std::to_string(i));
        Store(Node->irCode, var,Node->elements[i]->irRes,Node->elements[i]->realType);
        //%v42 = getelementptr [3 x i32], ptr %arr_ptr, i32 0, i32 2,计算第几个位置,便于后续存储
    }
}

void generateIr(ArraySimplifiedExpr*Node,ASTNode* r,ASTNode* LastBlock) {
    Node->irRes=Node->irRes_p="%v"+std::to_string(++variableNum);
    LastBlock->irCode.emplace_back(Node->irRes_p+" = alloca "+Type_To_String(Node->realType));
    Node->irCode.emplace_back(&Node->element->irCode);
    auto len = std::any_cast<long long>(Node->length->eval);
    for (auto i=0;i<len;i++) {//是否会爆栈？需不需要优化？
        std::string var=std::to_string(++variableNum);
        Node->irCode.emplace_back(var+" = getelementptr "+
            Type_To_String(Node->realType)+", ptr "+Node->irRes_p+", i32 0, i32 "+std::to_string(i));
        Store(Node->irCode, var,Node->element->irRes,Node->element->realType);
    }
}

void generateIr(ArrayAccessExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    Node->irCode.emplace_back(&Node->array->irCode);
    Node->irCode.emplace_back(&Node->index->irCode);
    Node->irRes_p=Node->irRes_p="%v"+std::to_string(++variableNum);
    Node->irCode.emplace_back(Node->irRes_p+" = getelementptr "+Type_To_String(Node->array->realType)+", ptr "+
        Node->array->irRes+", i32 0, i32 "+Node->index->irRes);
    if (is_BigType(Node->realType)) {
        Node->irRes=Node->irRes_p;
    }else {
        Node->irRes= "%v"+std::to_string(++variableNum);
        Load(Node->irCode,Node->irRes,Node->irRes_p,Node->realType);
    }
}

void generateIr(LoopExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    if (Node->realType->typeKind==TypeName::UnitType||Node->realType->typeKind==TypeName::NeverType) {
        Node->irRes="()";
    }else {
        LastBlock->irCode.emplace_back(Node->irRes_p+" = alloca "+Type_To_String(Node->realType));
    }
    Node->irCode.emplace_back("br label "+Node->irLabel1);
    Node->irCode.emplace_back("\n");
    Node->irCode.emplace_back("out");
    Node->irCode.emplace_back(Node->irLabel1.substr(1)+":");
    Node->irCode.emplace_back("in");
    Node->irCode.emplace_back(&Node->block->irCode);
    if (Node->block->realType->typeKind!=TypeName::NeverType) {
        Node->irCode.emplace_back("br label "+Node->irLabel1);
    }

    Node->irCode.emplace_back("\n");
    Node->irCode.emplace_back("out");
    Node->irCode.emplace_back(Node->irLabel2.substr(1)+":");
    Node->irCode.emplace_back("in");

    if (Node->realType->typeKind!=TypeName::NeverType||Node->realType->typeKind!=TypeName::UnitType) {
        if (is_BigType(Node->realType)) {
            Node->irRes=Node->irRes_p;
        }else {
            Load(Node->irCode,Node->irRes,Node->irRes_p,Node->realType);
        }
    }
}

void generateIr(WhileExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    if (Node->condition->eval.has_value()) {
        if (const auto B=std::any_cast<bool>(Node->condition->eval); B==1) {//恒为1,无限循环
            Node->irCode.emplace_back("br label "+Node->irLabel1);
            Node->irCode.emplace_back("\n");
            Node->irCode.emplace_back("out");
            Node->irCode.emplace_back(Node->irLabel1.substr(1)+":");
            Node->irCode.emplace_back("in");
            Node->irCode.emplace_back(&Node->block->irCode);//假模假式把后续生成出来
            if (Node->block->realType->typeKind!=TypeName::NeverType) {
                Node->irCode.emplace_back("br label "+Node->irLabel1);
            }
            Node->irCode.emplace_back("\n");
            Node->irCode.emplace_back("out");
            Node->irCode.emplace_back(Node->irLabel2.substr(1)+":");
            Node->irCode.emplace_back("in");//假模假式的退出块
        }//如果是恒假就不生成了
    }else {
        std::string var="%v"+std::to_string(++variableNum);
        Node->irCode.emplace_back("br label "+Node->irLabel1);
        Node->irCode.emplace_back("\n");
        Node->irCode.emplace_back("out");
        Node->irCode.emplace_back(Node->irLabel1.substr(1)+":");
        Node->irCode.emplace_back("in");
        Node->irCode.emplace_back(&Node->condition->irCode);
        Node->irCode.emplace_back("br i1 "+Node->condition->irRes+", label "+var+", label "+Node->irLabel2);//运行condition,然后判断跳转

        Node->irCode.emplace_back("\n");
        Node->irCode.emplace_back("out");
        Node->irCode.emplace_back(var+":");//手动添加了var
        Node->irCode.emplace_back("in");
        Node->irCode.emplace_back(&Node->condition->irCode);
        if (Node->block->realType->typeKind!=TypeName::NeverType) {
            Node->irCode.emplace_back("br label "+Node->irLabel1);
        }//判断为真,跳转到block中执行,然后返回判断

        Node->irCode.emplace_back("\n");
        Node->irCode.emplace_back("out");
        Node->irCode.emplace_back(Node->irLabel2.substr(1)+":");
        Node->irCode.emplace_back("in");//末尾是一个空跳转，也就是收束的部分，继续往后执行
    }
}

void generateIr(GroupedExpr *Node, ASTNode *r, ASTNode *LastBlock) {
    Node->irCode.emplace_back(Node->expr->irCode);
    Node->irRes+=Type_To_String(Node->realType)+" "+Node->expr->irRes;
}


void generateIr(ContinueExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    Node->irCode.emplace_back("br label "+Node->LoopPtr->irLabel1);
}

void generateIr(BreakExpr *Node,ASTNode* r,ASTNode* LastBlock) {
    if (Node->expr==nullptr) {//如果只是一个break
        Node->irCode.emplace_back("br label "+Node->LoopPtr->irLabel2);
    }else {
        Node->irCode.emplace_back(&Node->expr->irCode);
        if (Node->expr->realType->typeKind!=TypeName::NeverType) {
            if (Node->expr->realType->typeKind!=TypeName::UnitType) {
                Store(Node->irCode,Node->LoopPtr->irRes_p,Node->expr->irRes,Node->expr->realType);
            }
            Node->irCode.emplace_back("br label "+Node->LoopPtr->irLabel2);
        }
    }
}

void generateIr(LetStmt*Node,ASTNode* r,ASTNode* LastBlock) {
    if (Node->expr->realType->typeKind==TypeName::NeverType) {
        Node->irCode.emplace_back(Node->expr->irCode);
        Node->realType=Node->expr->realType;
    }else if (Node->expr->realType->typeKind!=TypeName::UnitType) {
        Node->irCode.emplace_back(Node->expr->irCode);
        LastBlock->irCode.emplace_back("%v"+std::to_string(Node->variableID)+" = alloca "+Type_To_String(Node->realType));
        Store(Node->irCode,"%v"+std::to_string(Node->variableID),Node->expr->irRes,Node->expr->realType);
    }
}


void generateIr(BlockExpr *Node,ASTNode* r,ASTNode* LastBlock);

void generateIr(UnaryExpr *Node,ASTNode* r,ASTNode* LastBlock);
void generateIr(PathExpr *Node,ASTNode* r,ASTNode* LastBlock);
void generateIr(FieldAccessExpr *Node,ASTNode* r,ASTNode* LastBlock);
void generateIr(CallExpr *Node,ASTNode* r,ASTNode* LastBlock);

void generateIr(EnumStmt *Node, ASTNode* r,ASTNode* LastBlock);
void generateIr(StructExpr *Node,ASTNode* r,ASTNode* LastBlock);
void generateIr(IfExpr *Node,ASTNode* r,ASTNode* LastBlock);
void generateIr(FnStmt *Node,ASTNode* r,ASTNode* LastBlock);


void generateIr(ReturnExpr *Node,ASTNode* r,ASTNode* LastBlock);
void generateIr(ConstStmt *Node,ASTNode* r,ASTNode* LastBlock);


std::vector<std::string> IntegrateRes(std::vector<std::any> &res, int dent) {
    std::vector<std::string> lines;
    for (auto element:res) {
        if (element.type()==typeid(std::string)) {
            auto stri=std::any_cast<std::string>(element);
            if (stri=="in") {
                dent+=2;
            }else if (stri=="out") {
                dent-=2;
            }else if(stri=="\n") {
                lines.emplace_back("");
            }else {
                lines.emplace_back(SetDent(dent)+stri);
            }
        }else {
            auto morelines=IntegrateRes(*std::any_cast<std::vector<std::any>*>(element),dent);
            for (auto line:morelines) {
                lines.emplace_back(line);
            }
        }
    }
    return lines;
}

std::vector<std::string> trimString(const std::vector<std::string> res) {

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
    res.emplace_back(std::string("empty line"));
    return res;
}