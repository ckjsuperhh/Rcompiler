//
// Created by ckjsuperhh6602 on 25-9-24.
//
#include "AST_node.h"
#include <vector>
#include <string>
#include <sstream>
#include "ir.h"

// 辅助函数：获取节点类型的字符串表示
std::string get_node_type_str(TypeName type) {
    static const std::unordered_map<TypeName, std::string> type_map = {
        // 基础类型
        {TypeName::Int, "Int"},
        {TypeName::Float, "Float"},
        {TypeName::Bool, "Bool"},
        {TypeName::Void, "Void"},
        {TypeName::Uint, "Uint"},
        {TypeName::String, "String"},
        {TypeName::Char, "Char"},
        {TypeName::I32, "i32"},
        {TypeName::U32, "u32"},
        {TypeName::Isize, "isize"},
        {TypeName::Usize, "usize"},
        {TypeName::Unit, "unit"},

        // 复合类型
        {TypeName::BasicType, "BasicType"},
        {TypeName::ArrayType, "ArrayType"},
        {TypeName::TupleType, "TupleType"},
        {TypeName::StructType, "StructType"},
        {TypeName::FunctionType, "FunctionType"},
        {TypeName::ErrorType, "ErrorType"},
        {TypeName::IdentifierType, "IdentifierType"},
        {TypeName::selfType, "selfType"},
        {TypeName::SelfType, "SelfType"},
        {TypeName::ArrayAccessType, "ArrayAccessType"},
        {TypeName::RustType, "RustType"},

        // 表达式
        {TypeName::LiteralExpr, "LiteralExpr"},
        {TypeName::VariableExpr, "VariableExpr"},
        {TypeName::BinaryExpr, "BinaryExpr"},
        {TypeName::UnaryExpr, "UnaryExpr"},
        {TypeName::CallExpr, "CallExpr"},
        {TypeName::IfExpr, "IfExpr"},
        {TypeName::WhileExpr, "WhileExpr"},
        {TypeName::LoopExpr, "LoopExpr"},
        {TypeName::RangeExpr, "RangeExpr"},
        {TypeName::ReturnExpr, "ReturnExpr"},
        {TypeName::UnderscoreExpr, "UnderscoreExpr"},
        {TypeName::UnitExpr, "UnitExpr"},
        {TypeName::BlockExpr, "BlockExpr"},
        {TypeName::PathExpr, "PathExpr"},
        {TypeName::AsExpr, "AsExpr"},
        {TypeName::GroupedExpr,"GroupedExpr"},
        {TypeName::ArrayAccessExpr,"ArrayAccessExpr"},
        {TypeName::ArrayInitExpr,"ArrayInitExpr"},
        {TypeName::ArraySimplifiedExpr,"ArraySimplifiedExpr"},

        // 语句
        {TypeName::LetStmt, "LetStmt"},
        {TypeName::ConstStmt, "ConstStmt"},
        {TypeName::AssignmentStmt, "AssignmentStmt"},
        {TypeName::FnStmt, "FnStmt"},
        {TypeName::StructStmt, "StructStmt"},
        {TypeName::EnumStmt, "EnumStmt"},
        {TypeName::TraitStmt, "TraitStmt"},
        {TypeName::TraitImplStmt, "TraitImplStmt"},
        {TypeName::InherentImplStmt, "InherentImplStmt"},

        // 其他
        {TypeName::Program, "Program"},
        {TypeName::Error, "Error"}
    };
    auto it = type_map.find(type);
    return (it != type_map.end()) ? it->second : "Unknown";
}


// 基础 AST 节点的显示函数
std::vector<std::string> ASTNode::showSelf() const {
    std::vector<std::string> res;
    std::stringstream ss;
    ss << get_node_type_str(node_type);
    res.push_back(ss.str());
    return res;
}

// 递归生成树形结构
std::vector<std::string> ASTNode::showTree(int depth = 0, bool is_last = true) const {
    auto res = ASTNode::showSelf();
    auto children = this->get_children(); // 需要每个节点实现获取子节点的方法

    for (size_t i = 0; i < children.size(); ++i) {
        bool last_child = (i == children.size() - 1);
        std::vector<std::string> child_content;
        std::visit([&]<typename T0>(T0&& elem) {
            using T = std::decay_t<T0>;
            if constexpr (std::is_same_v<T, ASTNode*>) {
                // 如果是ASTNode，递归展示其树形结构
                child_content = elem->showTree(depth + 1, last_child);
            } else if constexpr (std::is_same_v<T, std::string>) {
                // 对于字符串类型，直接使用该字符串
                child_content = {elem};
            } else if constexpr (std::is_same_v<T, Type*>) {
                // 处理Type*类型：调用其toString()方法
                if (elem != nullptr) {
                    child_content = {elem->toString()};
                } else {
                    child_content = {"nullptr (Type*)"}; // 处理空指针情况
                }
            } else {
                // 对于其他数值类型（如int），使用std::to_string
                child_content = {std::to_string(elem)};
            }
        }, children[i]);
        res.emplace_back("|");
        for (size_t j = 0; j < child_content.size(); ++j) {
            std::string prefix;
            if (j == 0) {
                prefix = last_child ? "└-" : "├-";
            } else {
                prefix = last_child ? "  " : "| ";
            }
            res.push_back(prefix + child_content[j]);
        }
    }
    return res;
}

// 1. BasicType 类实现（基础类型，如 int、bool 等）
[[nodiscard]] std::vector<Element> BasicType::get_children() const  {
    std::vector<Element> children;

    // 转换 TypeName 枚举为可读字符串（匹配枚举定义的基础类型）
    std::string kind_str;
    switch (kind) {
        case TypeName::Int:      kind_str = "Int";      break;
        case TypeName::Float:    kind_str = "Float";    break;
        case TypeName::Bool:     kind_str = "Bool";     break;
        case TypeName::Void:     kind_str = "Void";     break;
        case TypeName::Uint:     kind_str = "Uint";     break;
        case TypeName::String:   kind_str = "String";   break;
        case TypeName::I32:      kind_str = "i32";      break;
        case TypeName::U32:      kind_str = "u32";      break;
        case TypeName::Isize:    kind_str = "isize";    break;
        case TypeName::Usize:    kind_str = "usize";    break;
        case TypeName::Char:     kind_str = "Char";     break;
        case TypeName::Unit:     kind_str = "unit";     break;
        case TypeName::Integer:  kind_str = "Integer";  break;  // 通用整数类型
        default:                 kind_str = "UnknownBasicType";
    }

    // 添加基础类型标识（字符串属性）
    children.emplace_back("basic_type_kind: " + kind_str);
    // 添加可变性和引用标记（Type 基类成员）
    children.emplace_back("is_mutable: " + (is_mutable ? std::string("true") : std::string("false")));
    children.emplace_back("is_reference: " + (is_and ? std::string("true") : std::string("false")));

    return children;
}

// 2. ArrayType 类实现（数组类型，包含元素类型、维度、长度）
[[nodiscard]] std::vector<Element> ArrayType::get_children() const  {
    std::vector<Element> children;

    // 添加数组维度（int 类型属性，构造函数已确保 >0）
    children.emplace_back("array_dimension: " + std::to_string(dimension));
    // 添加可变性和引用标记（Type 基类成员）
    children.emplace_back("is_mutable: " + (is_mutable ? std::string("true") : std::string("false")));
    children.emplace_back("is_reference: " + (is_and ? std::string("true") : std::string("false")));

    // 数组元素类型（Type* 子节点，构造函数已确保非空）
    children.emplace_back("array_element_type:");
    children.emplace_back(typePtr.get());

    // 数组长度表达式（Expr* 子节点，可能为常量表达式如 10，或变量表达式）
    if (length) {
        children.emplace_back("array_length_expr:");
        children.emplace_back(length.get());
    } else {
        children.emplace_back("array_length_expr: dynamic");  // 动态长度数组场景
    }

    return children;
}

// 3. IdentifierType 类实现（标识符类型，如 struct、enum 的名称引用）
[[nodiscard]] std::vector<Element> IdentifierType::get_children() const  {
    std::vector<Element> children;

    // 添加标识符名称（如 "User"、"Color"，对应 struct/enum 名）
    children.emplace_back("identifier_type_name: " + name);
    // 添加可变性和引用标记（Type 基类成员）
    children.emplace_back("is_mutable: " + (is_mutable ? std::string("true") : std::string("false")));
    children.emplace_back("is_reference: " + (is_and ? std::string("true") : std::string("false")));

    return children;
}

// 4. SelfType 类实现（Self 类型，用于 trait/impl 中指代当前类型）
[[nodiscard]] std::vector<Element> SelfType::get_children() const  {
    std::vector<Element> children;

    // 添加 Self 类型标识
    children.emplace_back("self_type: Self");
    // 添加可变性和引用标记（Type 基类成员）
    children.emplace_back("is_mutable: " + (is_mutable ? std::string("true") : std::string("false")));
    children.emplace_back("is_reference: " + (is_and ? std::string("true") : std::string("false")));

    return children;
}

// 5. FunctionType 类实现（函数类型，包含参数列表和返回类型）
[[nodiscard]] std::vector<Element> FunctionType::get_children() const  {
    std::vector<Element> children;

    // 添加函数参数列表（Param 包含 string 和 Type*，逐个展示）
    children.emplace_back("func_parameters(count: " + std::to_string(parameters.size()) + "):");
    for (size_t i = 0; i < parameters.size(); ++i) {
        const auto& param = parameters[i];
        // 参数名（如 "a"、"b"）
        children.emplace_back("  param[" + std::to_string(i) + "]_name: " + param.name);
        // 参数类型（Type* 子节点）
        if (param.type) {
            children.emplace_back("  param[" + std::to_string(i) + "]_type:");
            children.emplace_back(param.type.get());
        } else {
            children.emplace_back("  param[" + std::to_string(i) + "]_type: unknown");
        }
    }

    // 添加函数返回类型（Type* 子节点，非空）
    if (return_type) {
        children.emplace_back("func_return_type:");
        children.emplace_back(return_type.get());
    } else {
        children.emplace_back("func_return_type: unit");  // 默认无返回值场景
    }

    // 添加可变性和引用标记（Type 基类成员，函数类型通常无，但保留兼容性）
    children.emplace_back("is_mutable: " + (is_mutable ? std::string("true") : std::string("false")));
    children.emplace_back("is_reference: " + (is_and ? std::string("true") : std::string("false")));

    return children;
}

// 6. ErrorType 类实现（错误类型，用于语法/语义分析错误场景）
[[nodiscard]] std::vector<Element> ErrorType::get_children() const  {
    std::vector<Element> children;

    // 添加错误类型标识，无额外子节点（错误类型无需详细结构）
    children.emplace_back("error_type: syntax/semantic error");
    // 保留 Type 基类成员标记（兼容性）
    children.emplace_back("is_mutable: false");  // 错误类型默认不可变
    children.emplace_back("is_reference: false");// 错误类型默认非引用

    return children;
}

[[nodiscard]] std::vector<Element> AndStrType::get_children() const  {
    std::vector<Element> children;

    // 添加类型标识（特有信息）
    children.emplace_back("and_str_type: &str");

    // 保留 Type 基类成员标记（兼容性）
    children.emplace_back("is_mutable: false");  // &str 类型默认不可变
    children.emplace_back("is_reference: true"); // &str 本质是引用类型

    return children;
}

// LiteralExpr 类的get_children实现
// 包含字符串值value和类型信息realType
[[nodiscard]] std::vector<Element> LiteralExpr::get_children() const  {
    std::vector<Element> children;
    // 添加值信息（作为字符串元素）
    children.emplace_back("value: " + value);
    // 添加类型信息（如果存在）
    if (realType) {
        children.emplace_back(realType.get());
    }
    return children;
}

// ArrayInitExpr 类的get_children实现
// 包含多个元素表达式elements和类型信息realType
[[nodiscard]] std::vector<Element> ArrayInitExpr::get_children() const  {
    std::vector<Element> children;
    // 先添加数组元素数量信息
    children.emplace_back("elements_count: " + std::to_string(elements.size()));
    // 逐个添加数组元素（ASTNode子节点）
    for (const auto& elem : elements) {
        if (elem) {
            children.emplace_back(elem.get());
        } else {
            children.emplace_back("null_element");
        }
    }
    // 添加数组类型信息（如果存在）
    if (realType) {
        children.emplace_back(realType.get());
    }
    return children;
}

// ArraySimplifiedExpr 类的get_children实现
// 包含元素表达式element、长度表达式length和类型信息realType
[[nodiscard]] std::vector<Element> ArraySimplifiedExpr::get_children() const  {
    std::vector<Element> children;
    // 添加元素表达式（ASTNode子节点）
    if (element) {
        children.emplace_back("element:");
        children.emplace_back(element.get());
    } else {
        children.emplace_back("element: null");
    }
    // 添加长度表达式（ASTNode子节点）
    if (length) {
        children.emplace_back("length:");
        children.emplace_back(length.get());
    } else {
        children.emplace_back("length: null");
    }
    // 添加数组类型信息（如果存在）
    if (realType) {
        children.emplace_back("type:");
        children.emplace_back(realType.get());
    }
    return children;
}

// ArrayAccessExpr 类的get_children实现
[[nodiscard]] std::vector<Element> ArrayAccessExpr::get_children() const  {
    std::vector<Element> children;
    // 添加数组表达式
    if (array) {
        children.emplace_back("array:");
        children.emplace_back(array.get());
    } else {
        children.emplace_back("array: null");
    }
    // 添加索引表达式
    if (index) {
        children.emplace_back("index:");
        children.emplace_back(index.get());
    } else {
        children.emplace_back("index: null");
    }
    // 添加类型信息
    if (realType) {
        children.emplace_back("type:");
        children.emplace_back(realType.get());
    }
    return children;
}

// UnitExpr 类的get_children实现（无实际子节点）
[[nodiscard]] std::vector<Element> UnitExpr::get_children() const  {
    return {"()"}; // 单元表达式没有子节点
}

// BinaryExpr 类的get_children实现
[[nodiscard]] std::vector<Element> BinaryExpr::get_children() const  {
    std::vector<Element> children;
    // 添加左操作数
    if (left) {
        children.emplace_back("left:");
        children.emplace_back(left.get());
    } else {
        children.emplace_back("left: null");
    }
    // 添加运算符
    children.emplace_back("op: " + op);
    // 添加右操作数
    if (right) {
        children.emplace_back("right:");
        children.emplace_back(right.get());
    } else {
        children.emplace_back("right: null");
    }
    // 添加类型信息
    if (realType) {
        children.emplace_back("type:");
        children.emplace_back(realType.get());
    }
    return children;
}

[[nodiscard]] std::vector<Element> UnderscoreExpr::get_children() const {
    std::vector<Element> children;
    children.emplace_back("underscore_expr:_");
    if (realType) {
        children.emplace_back("type:");
        children.emplace_back(realType.get());
    }
    return children;
}

// UnaryExpr 类的get_children实现
[[nodiscard]] std::vector<Element> UnaryExpr::get_children() const  {
    std::vector<Element> children;
    // 添加运算符
    children.emplace_back("op: " + op);
    // 添加操作数
    if (right) {
        children.emplace_back("operand:");
        children.emplace_back(right.get());
    } else {
        children.emplace_back("operand: null");
    }
    // 添加类型信息
    if (realType) {
        children.emplace_back("type:");
        children.emplace_back(realType.get());
    }
    return children;
}

// PathExpr 类的get_children实现
[[nodiscard]] std::vector<Element> PathExpr::get_children() const  {
    std::vector<Element> children;
    // 添加路径片段信息
    children.emplace_back("segments:");
    for (const auto& seg : segments) {
        children.emplace_back("segment: " + seg);
    }
    // 添加类型信息
    if (realType) {
        children.emplace_back("type:");
        children.emplace_back(realType.get());
    }
    return children;
}

// CallExpr 类的get_children实现
[[nodiscard]] std::vector<Element> CallExpr::get_children() const  {
    std::vector<Element> children;
    // 添加接收者（被调用对象）
    if (receiver) {
        children.emplace_back("receiver:");
        children.emplace_back(receiver.get());
    } else {
        children.emplace_back("receiver: null");
    }
    // 添加参数列表
    children.emplace_back("arguments(" + std::to_string(arguments.size()) + "):");
    for (const auto& arg : arguments) {
        if (arg) {
            children.emplace_back(arg.get());
        } else {
            children.emplace_back("null_argument");
        }
    }
    // 添加类型信息
    if (realType) {
        children.emplace_back("type:");
        children.emplace_back(realType.get());
    }
    return children;
}

// FieldAccessExpr 类的get_children实现
[[nodiscard]] std::vector<Element> FieldAccessExpr::get_children() const  {
    std::vector<Element> children;
    // 添加结构体/对象表达式
    if (struct_name) {
        children.emplace_back("struct:");
        children.emplace_back(struct_name.get());
    } else {
        children.emplace_back("struct: null");
    }
    // 添加字段表达式
    if (field_expr) {
        children.emplace_back("field:");
        children.emplace_back(field_expr.get());
    } else {
        children.emplace_back("field: null");
    }
    // 添加类型信息
    if (realType) {
        children.emplace_back("type:");
        children.emplace_back(realType.get());
    }
    return children;
}

// BlockExpr 类的get_children实现
[[nodiscard]] std::vector<Element> BlockExpr::get_children() const  {
    std::vector<Element> children;
    // 添加语句数量信息
    children.emplace_back("statements(" + std::to_string(statements.size()) + "):");
    // 逐个添加语句
    for (size_t i = 0; i < statements.size(); ++i) {
        const auto& stmt_pair = statements[i];
        if (stmt_pair.first) {
            // 添加语句索引标识
            children.emplace_back("statement " + std::to_string(i) + (stmt_pair.second ? " (expr):" : ":"));
            children.emplace_back(stmt_pair.first.get());
        } else {
            children.emplace_back("statement " + std::to_string(i) + ": null");
        }
    }
    // 添加类型信息
    if (realType) {
        children.emplace_back("type:");
        children.emplace_back(realType.get());
    }
    return children;
}

// IfExpr 类的get_children实现
[[nodiscard]] std::vector<Element> IfExpr::get_children() const  {
    std::vector<Element> children;

    // 添加条件表达式
    if (condition) {
        children.emplace_back("condition:");
        children.emplace_back(condition.get());
    } else {
        children.emplace_back("condition: null");
    }

    // 添加then分支（if体）
    if (then_branch) {
        children.emplace_back("then_branch:");
        children.emplace_back(then_branch.get());
    } else {
        children.emplace_back("then_branch: null");
    }

    // 添加else分支（可能为null）
    if (else_branch) {
        children.emplace_back("else_branch:");
        children.emplace_back(else_branch.get());
    } else {
        children.emplace_back("else_branch: none");
    }

    // 添加类型信息
    if (realType) {
        children.emplace_back("type:");
        children.emplace_back(realType.get());
    }

    return children;
}

// ReturnExpr 类的get_children实现
[[nodiscard]] std::vector<Element> ReturnExpr::get_children() const  {
    std::vector<Element> children;

    // 添加返回值表达式
    if (expr) {
        children.emplace_back("return_value:");
        children.emplace_back(expr.get());
    } else {
        children.emplace_back("return_value: void");
    }

    // 添加类型信息
    if (realType) {
        children.emplace_back("type:");
        children.emplace_back(realType.get());
    }

    return children;
}

// GroupedExpr 类的get_children实现
[[nodiscard]] std::vector<Element> GroupedExpr::get_children() const  {
    std::vector<Element> children;

    // 添加包裹的表达式
    if (expr) {
        children.emplace_back("grouped_expr:");
        children.emplace_back(expr.get());
    } else {
        children.emplace_back("grouped_expr: null");
    }

    // 添加类型信息
    if (realType) {
        children.emplace_back("type:");
        children.emplace_back(realType.get());
    }

    return children;
}

// AssignmentExpr 类的get_children实现
[[nodiscard]] std::vector<Element> AssignmentExpr::get_children() const  {
    std::vector<Element> children;

    // 添加左操作数（赋值目标）
    if (left) {
        children.emplace_back("lhs:");  // lhs = left-hand side
        children.emplace_back(left.get());
    } else {
        children.emplace_back("lhs: null");
    }

    // 添加赋值运算符
    children.emplace_back("op:"+op);

    // 添加右操作数（赋值源）
    if (right) {
        children.emplace_back("rhs:");  // rhs = right-hand side
        children.emplace_back(right.get());
    } else {
        children.emplace_back("rhs: null");
    }

    // 添加类型信息
    if (realType) {
        children.emplace_back("type:");
        children.emplace_back(realType.get());
    }

    return children;
}

// ContinueExpr 类的get_children实现
[[nodiscard]] std::vector<Element> ContinueExpr::get_children() const  {
    // continue语句没有子节点
    return {};
}

// BreakExpr 类的get_children实现
[[nodiscard]] std::vector<Element> BreakExpr::get_children() const  {
    std::vector<Element> children;

    // 添加break带有的表达式（可能为null）
    if (expr) {
        children.emplace_back("break_value:");
        children.emplace_back(expr.get());
    } else {
        children.emplace_back("break_value: none");
    }

    // 添加类型信息
    if (realType) {
        children.emplace_back("type:");
        children.emplace_back(realType.get());
    }

    return children;
}

// StructExpr的get_children实现
[[nodiscard]] std::vector<Element> StructExpr::get_children() const {
    std::vector<Element> children;

    // 添加结构体名称表达式（如标识符或路径）
    if (structname) {
        children.emplace_back("struct_name:");
        children.emplace_back(structname.get());  // 添加Expr类型的子节点
    } else {
        children.emplace_back("struct_name: null");
    }

    // 添加应用列表（application类型的子项）
    children.emplace_back("applications(count: " + std::to_string(apps.size()) + "):");
    for (size_t i = 0; i < apps.size(); ++i) {
        // 假设application包含name和expr成员，这里根据实际定义调整
        if (apps[i].name) {
            children.emplace_back("  app[" + std::to_string(i) + "]_name:");
            children.emplace_back(apps[i].name.get());  // 添加Expr类型的子节点
        } else {
            children.emplace_back("  app[" + std::to_string(i) + "]_name: null");
        }
        if (apps[i].variable) {
            children.emplace_back("  app[" + std::to_string(i) + "]_variable:");
            children.emplace_back(apps[i].variable.get());  // 添加Expr类型的子节点
        } else {
            children.emplace_back("  app[" + std::to_string(i) + "]_variable: null");
        }
    }
    if (realType) {
        children.emplace_back("type:");
        children.emplace_back(realType.get());
    }
    return children;
}

// AsExpr的get_children实现
[[nodiscard]] std::vector<Element> AsExpr::get_children() const {
    std::vector<Element> children;

    // 添加被转换的表达式
    if (expr) {
        children.emplace_back("cast_expression:");
        children.emplace_back(expr.get());  // 添加Expr类型的子节点
    } else {
        children.emplace_back("cast_expression: null");
    }

    // 添加目标类型
    if (type) {
        children.emplace_back("target_type:");
        children.emplace_back(type.get());  // 添加Type类型的子节点
    } else {
        children.emplace_back("target_type: unknown");
    }
    if (realType) {
        children.emplace_back("type:");
        children.emplace_back(realType.get());
    }

    return children;
}

// LoopExpr 类的get_children实现
[[nodiscard]] std::vector<Element> LoopExpr::get_children() const  {
    std::vector<Element> children;

    // 添加循环体
    if (block) {
        children.emplace_back("loop_body:");
        children.emplace_back(block.get());
    } else {
        children.emplace_back("loop_body: null");
    }

    // 添加类型信息
    if (realType) {
        children.emplace_back("type:");
        children.emplace_back(realType.get());
    }

    return children;
}

// WhileExpr 类的get_children实现
[[nodiscard]] std::vector<Element> WhileExpr::get_children() const  {
    std::vector<Element> children;

    // 添加循环条件
    if (condition) {
        children.emplace_back("condition:");
        children.emplace_back(condition.get());
    } else {
        children.emplace_back("condition: null");
    }

    // 添加循环体
    if (block) {
        children.emplace_back("loop_body:");
        children.emplace_back(block.get());
    } else {
        children.emplace_back("loop_body: null");
    }

    // 添加类型信息
    if (realType) {
        children.emplace_back("type:");
        children.emplace_back(realType.get());
    }

    return children;
}

// 1. ConstStmt 类实现
[[nodiscard]] std::vector<Element> ConstStmt::get_children() const  {
    std::vector<Element> children;

    // 常量标识符（字符串属性）
    children.emplace_back("const_id: " + identifier);

    // 常量类型（Type* 子节点）
    if (type) {
        children.emplace_back("const_type:");
        children.emplace_back(type.get());
    } else {
        children.emplace_back("const_type: null");
    }

    // 常量初始化表达式（Expr* 子节点）
    if (expr) {
        children.emplace_back("const_init_expr:");
        children.emplace_back(expr.get());
    } else {
        children.emplace_back("const_init_expr: null");
    }

    return children;
}

// 2. FnStmt 类实现
[[nodiscard]] std::vector<Element> FnStmt::get_children() const  {
    std::vector<Element> children;

    // 函数名（字符串属性）
    children.emplace_back("fn_name: " + name);

    // 函数参数列表（Param 包含 string 和 Type*，需拆分展示）
    children.emplace_back("fn_params(count: " + std::to_string(parameters.size()) + "):");
    for (size_t i = 0; i < parameters.size(); ++i) {
        const auto& param = parameters[i];
        // 参数名
        children.emplace_back("  param[" + std::to_string(i) + "]_id: " + param.name);
        // 参数类型（Type* 子节点）
        if (param.type) {
            children.emplace_back("  param[" + std::to_string(i) + "]_type:");
            children.emplace_back(param.type.get());
        } else {
            children.emplace_back("  param[" + std::to_string(i) + "]_type: null");
        }
    }

    // 函数返回类型（Type* 子节点）
    if (return_type) {
        children.emplace_back("fn_return_type:");
        children.emplace_back(return_type.get());
    } else {
        children.emplace_back("fn_return_type: void");
    }

    // 函数体（Expr* 子节点，通常是 BlockExpr）
    if (body) {
        children.emplace_back("fn_body:");
        children.emplace_back(body.get());
    } else {
        children.emplace_back("fn_body: null");
    }

    return children;
}

// 3. StructStmt 类实现
[[nodiscard]] std::vector<Element> StructStmt::get_children() const  {
    std::vector<Element> children;

    // 结构体名（字符串属性）
    children.emplace_back("struct_name: " + name);

    // 结构体字段列表（Field 包含 string 和 Type*）
    children.emplace_back("struct_fields(count: " + std::to_string(fields.size()) + "):");
    for (size_t i = 0; i < fields.size(); ++i) {
        const auto& field = fields[i];
        // 字段名
        children.emplace_back("  field[" + std::to_string(i) + "]_id: " + field.name);
        // 字段类型（Type* 子节点）
        if (field.type) {
            children.emplace_back("  field[" + std::to_string(i) + "]_type:");
            children.emplace_back(field.type.get());
        } else {
            children.emplace_back("  field[" + std::to_string(i) + "]_type: null");
        }
    }

    return children;
}

// 4. EnumStmt 类实现
[[nodiscard]] std::vector<Element> EnumStmt::get_children() const  {
    std::vector<Element> children;

    // 枚举名（字符串属性）
    children.emplace_back("enum_name: " + enum_name);

    // 枚举成员列表（纯字符串属性）
    children.emplace_back("enum_members(count: " + std::to_string(ids.size()) + "):");
    for (size_t i = 0; i < ids.size(); ++i) {
        children.emplace_back("  member[" + std::to_string(i) + "]: " + ids[i]);
    }

    return children;
}

// 6. InherentImplStmt 类实现
[[nodiscard]] std::vector<Element> InherentImplStmt::get_children() const  {
    std::vector<Element> children;

    // 关联的结构体/枚举名（字符串属性）
    children.emplace_back("impl_for: " + name);

    // 固有实现中的函数列表（同 TraitStmt 的函数格式）
    children.emplace_back("impl_fns(count: " + std::to_string(fns.size()) + "):");
    for (size_t i = 0; i < fns.size(); ++i) {
        const auto& fn_pair = fns[i];
        children.emplace_back("  fn[" + std::to_string(i) + "] (is_default: " + (fn_pair.second ? "true" : "false") + "):");
        if (fn_pair.first) {
            children.emplace_back(fn_pair.first.get());
        } else {
            children.emplace_back("  fn[" + std::to_string(i) + "_body: null");
        }
    }

    // 固有实现中的常量列表（同 TraitStmt 的常量格式）
    children.emplace_back("impl_consts(count: " + std::to_string(cons.size()) + "):");
    for (size_t i = 0; i < cons.size(); ++i) {
        const auto& const_pair = cons[i];
        children.emplace_back("  const[" + std::to_string(i) + "] (is_default: " + (const_pair.second ? "true" : "false") + "):");
        if (const_pair.first) {
            children.emplace_back(const_pair.first.get());
        } else {
            children.emplace_back("  const[" + std::to_string(i) + "_body: null");
        }
    }

    return children;
}

// // 7. TraitImplStmt 类实现
// [[nodiscard]] std::vector<Element> TraitImplStmt::get_children() const  {
//     std::vector<Element> children;
//
//     // 实现的 Trait 名和关联的结构体名（字符串属性）
//     children.emplace_back("impl_trait: " + trait_name);
//     children.emplace_back("impl_for_struct: " + struct_name);
//
//     // 实现的函数列表（同 TraitStmt）
//     children.emplace_back("impl_fns(count: " + std::to_string(fns.size()) + "):");
//     for (size_t i = 0; i < fns.size(); ++i) {
//         const auto& fn_pair = fns[i];
//         children.emplace_back("  fn[" + std::to_string(i) + "] (is_default: " + (fn_pair.second ? "true" : "false") + "):");
//         if (fn_pair.first) {
//             children.emplace_back(fn_pair.first.get());
//         } else {
//             children.emplace_back("  fn[" + std::to_string(i) + "_body: null");
//         }
//     }
//
//     // 实现的常量列表（同 TraitStmt）
//     children.emplace_back("impl_consts(count: " + std::to_string(cons.size()) + "):");
//     for (size_t i = 0; i < cons.size(); ++i) {
//         const auto& const_pair = cons[i];
//         children.emplace_back("  const[" + std::to_string(i) + "] (is_default: " + (const_pair.second ? "true" : "false") + "):");
//         if (const_pair.first) {
//             children.emplace_back(const_pair.first.get());
//         } else {
//             children.emplace_back("  const[" + std::to_string(i) + "_body: null");
//         }
//     }
//
//     return children;
// }

// 8. LetStmt 类实现
[[nodiscard]] std::vector<Element> LetStmt::get_children() const  {
    std::vector<Element> children;

    // 变量标识符 + 可变性标记（字符串属性）
    children.emplace_back("let_id: " + identifier + " (mut: " + (is_mutable ? "true" : "false") + ")");

    // 变量类型（Type* 子节点）
    if (type) {
        children.emplace_back("let_type:");
        children.emplace_back(type.get());
    } else {
        children.emplace_back("let_type: inferred");  // 类型推断场景
    }

    // 变量初始化表达式（Expr* 子节点）
    if (expr) {
        children.emplace_back("let_init_expr:");
        children.emplace_back(expr.get());
    } else {
        children.emplace_back("let_init_expr: uninitialized");  // 未初始化场景
    }

    return children;
}

// 9. Program 类实现
[[nodiscard]] std::vector<Element> Program::get_children() const {
    std::vector<Element> children;
    std::unordered_map<TypeName,std::string> t={
        {TypeName::FnStmt,"fn"},
        {TypeName::EnumStmt,"enum"},
        {TypeName::InherentImplStmt,"impl_for"},
        {TypeName::ConstStmt,"const"},
        {TypeName::StructStmt,"struct_declaration"}
};
    if (!statements.empty()) {
        children.emplace_back("program(count: " + std::to_string(statements.size()) + "):");
        for (const auto& im : statements) {
            if (im) {
                children.emplace_back(t[im->get_type()]+":");
                children.emplace_back(im.get());
            }
        }
    }

    return children;
}

// 1. BasicType::accept
void BasicType::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f,std::pair<SymbolTable*,ASTNode*> scope) {
    visitor.TypeCheck(this, F, l, f, scope);
}

// 2. ArrayType::accept
void ArrayType::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f,std::pair<SymbolTable*,ASTNode*> scope) {
    visitor.TypeCheck(this, F, l, f, scope);
}

// 3. IdentifierType::accept
void IdentifierType::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f,std::pair<SymbolTable*,ASTNode*> scope) {
    visitor.TypeCheck(this, F, l, f, scope);
}

// 4. SelfType::accept
void SelfType::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f,std::pair<SymbolTable*,ASTNode*> scope) {
    visitor.TypeCheck(this, F, l, f, scope);
}

// 5. FunctionType::accept
void FunctionType::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f,std::pair<SymbolTable*,ASTNode*> scope) {
    visitor.TypeCheck(this, F, l, f, scope);
}

// 6. ErrorType::accept
void ErrorType::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f,std::pair<SymbolTable*,ASTNode*> scope) {
    throw std::logic_error("ErrorType");
}

// 7. UnitType::accept
void UnitType::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f,std::pair<SymbolTable*,ASTNode*> scope) {
    // 原代码为空实现，保持不变
}

// 8. NeverType::accept
void NeverType::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f,std::pair<SymbolTable*,ASTNode*> scope) {
    // 原代码为空实现，保持不变
}

// 9. EnumType::accept
void EnumType::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f,std::pair<SymbolTable*,ASTNode*> scope) {
    // 原代码为空实现，保持不变
}

// 10. StructType::accept
void StructType::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f,std::pair<SymbolTable*,ASTNode*> scope) {
    // 原代码为空实现，保持不变
}

void AndStrType::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f,std::pair<SymbolTable*,ASTNode*> scope) {
    // 原代码为空实现，保持不变
}

// 11. TypeType::accept（原代码为空实现，补充类外定义）
void TypeType::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f,std::pair<SymbolTable*,ASTNode*> scope) {
    // 原代码为空实现，保持不变
}

void ReferenceType::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f,std::pair<SymbolTable*,ASTNode*> scope) {
    visitor.TypeCheck(this, F, l, f,scope);
}

// 1. RustType::accept
void RustType::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 2. LiteralExpr::accept
void LiteralExpr::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 3. ArrayInitExpr::accept
void ArrayInitExpr::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 4. ArraySimplifiedExpr::accept
void ArraySimplifiedExpr::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 5. ArrayAccessExpr::accept
void ArrayAccessExpr::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 6. UnitExpr::accept
void UnitExpr::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 7. BinaryExpr::accept
void BinaryExpr::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

void UnderscoreExpr::accept(SemanticCheck &visitor, ASTNode *F, ASTNode *l, ASTNode *f) {

}


// 8. UnaryExpr::accept
void UnaryExpr::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 9. PathExpr::accept
void PathExpr::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 10. CallExpr::accept
void CallExpr::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 11. FieldAccessExpr::accept
void FieldAccessExpr::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 12. BlockExpr::accept
void BlockExpr::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 13. IfExpr::accept
void IfExpr::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 14. ReturnExpr::accept
void ReturnExpr::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 15. GroupedExpr::accept
void GroupedExpr::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 16. AssignmentExpr::accept
void AssignmentExpr::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 17. StructExpr::accept
void StructExpr::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 18. AsExpr::accept
void AsExpr::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 19. ContinueExpr::accept
void ContinueExpr::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 20. BreakExpr::accept
void BreakExpr::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 21. LoopExpr::accept
void LoopExpr::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 22. WhileExpr::accept
void WhileExpr::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 23. ConstStmt::accept
void ConstStmt::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 24. FnStmt::accept
void FnStmt::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 25. StructStmt::accept
void StructStmt::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 26. EnumStmt::accept
void EnumStmt::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 27. InherentImplStmt::accept
void InherentImplStmt::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 28. LetStmt::accept
void LetStmt::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

// 29. Program::accept
void Program::accept(SemanticCheck& visitor, ASTNode* F, ASTNode* l, ASTNode* f) {
    return visitor.visit(this, F, l, f);
}

void LiteralExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void ArrayAccessExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void ArrayInitExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void ArraySimplifiedExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void UnitExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void BinaryExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void FnStmt::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void EnumStmt::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void ConstStmt::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void WhileExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void LoopExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void BreakExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void ContinueExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void StructExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void StructStmt::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void AssignmentExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void AsExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void GroupedExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void ReturnExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void BlockExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void IfExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void FieldAccessExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void PathExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void CallExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void UnderscoreExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
}

void UnaryExpr::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void RustType::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void InherentImplStmt::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void LetStmt::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}

void Program::IR(IRgen& visitor,ASTNode*r,ASTNode*l) {
    return visitor.generateIr(this,r,l);
}


