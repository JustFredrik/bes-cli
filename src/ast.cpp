#pragma once
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <sstream>
#include <variant>
#include <unordered_map>

enum class PrimitiveDataType {
    Int8,
    Int16,
    Int32,
    Int64, 
    Uint8,
    Uint16,
    Uint32,
    Uint64,
    Float32,
    Float64,
    String,
    Void,
};

enum class ListType {
    FixedLength,
    DynamicLength,
};

enum class DeclarationType {
    Struct,
    Union
};

struct FixedLengthList;
struct DynamicLengthList;
struct StructDeclaration;
struct AnonymousStruct;
struct NamedDeclaredReference;
struct UnionDeclaration;
struct AnonymousUnion;
struct FieldDeclaration;
struct UnionMemberDeclaration;
struct AST;

using DeclarationPointer = std::variant<
    std::monostate, // Invalid default value for when accessing map
    StructDeclaration*,
    UnionDeclaration*
>;

struct NamedDeclaredReference {
    std::string_view typeName;
    DeclarationType type;
    DeclarationPointer decPointer;
    int uid;
};

using FieldDataType = std::variant<
    PrimitiveDataType, 
    NamedDeclaredReference,
    std::unique_ptr<AnonymousStruct>, 
    std::unique_ptr<AnonymousUnion>,
    std::unique_ptr<FixedLengthList>,
    std::unique_ptr<DynamicLengthList>
>;

std::ostream& operator<<(std::ostream& os, const FixedLengthList& dt);
std::ostream& operator<<(std::ostream& os, const DynamicLengthList& dt);
std::ostream& operator<<(std::ostream& os, const PrimitiveDataType& dt);
std::ostream& operator<<(std::ostream& os, const DeclarationType& t);
std::ostream& operator<<(std::ostream& os, const NamedDeclaredReference& ref);
std::ostream& operator<<(std::ostream& os, const FieldDataType& dataType);
std::ostream& operator<<(std::ostream& os, const FieldDeclaration& field);
std::ostream& operator<<(std::ostream& os, const StructDeclaration& s);
std::ostream& operator<<(std::ostream& os, const AnonymousStruct& s);
std::ostream& operator<<(std::ostream& os, const UnionMemberDeclaration& member);
std::ostream& operator<<(std::ostream& os, const AnonymousUnion& u);
std::ostream& operator<<(std::ostream& os, const UnionDeclaration& u);

struct FixedLengthList {
    int length;
    FieldDataType dataType;
};

struct DynamicLengthList {
    FieldDataType dataType;
};

struct StructDeclaration {
    std::string_view name;
    std::vector<FieldDeclaration> fields;
    int uid;
};

struct AnonymousStruct {
    std::vector<FieldDeclaration> fields;
};

struct UnionDeclaration {
    std::string_view name;
    std::vector<UnionMemberDeclaration> members;
    int uid;
};

struct AnonymousUnion {
    std::vector<UnionMemberDeclaration> members;
};

struct FieldDeclaration {
    std::string_view name;
    FieldDataType dataType;
    int structFieldId;

    FieldDeclaration(std::string_view _name,  int _structFieldId, FieldDataType _dataType)
    : name(_name), structFieldId(_structFieldId), dataType(std::move(_dataType)) {}
};

struct UnionMemberDeclaration {
    FieldDataType dataType;
    int unionMemberId;
};

struct AST {
    std::unordered_map<std::string, DeclarationPointer> declarationLookup;
    std::vector<UnionDeclaration> unionDeclarations;
    std::vector<StructDeclaration> structDeclarations;
};


std::ostream& operator<<(std::ostream& os, const FixedLengthList& l) {
    os << l.dataType << "[" << l.length << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const DynamicLengthList& l) {
    os << l.dataType << "[]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const PrimitiveDataType& dt) {
    switch(dt) {
        case PrimitiveDataType::Int8:   os << "int8";   return os;
        case PrimitiveDataType::Int16:  os << "int16";  return os;
        case PrimitiveDataType::Int32:  os << "int32";  return os;
        case PrimitiveDataType::Int64:  os << "int64";  return os;
        case PrimitiveDataType::Uint8:  os << "uint8";  return os;
        case PrimitiveDataType::Uint16: os << "uint16"; return os;
        case PrimitiveDataType::Uint32: os << "uint32"; return os;
        case PrimitiveDataType::Uint64: os << "uint64"; return os;
        case PrimitiveDataType::Float32:os << "float32";return os;
        case PrimitiveDataType::Float64:os << "float64";return os;
        case PrimitiveDataType::String: os << "string"; return os;
        case PrimitiveDataType::Void:   os <<  "void";  return os;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const DeclarationType& t) {
    if (t == DeclarationType::Struct) {
        os << "Struct"; return os;
    }
    os << "Union"; 
    return os;
}

std::ostream& operator<<(std::ostream& os, const StructDeclaration& s) {
    os << "struct " << s.name << " @" << s.uid << " {\n";
    for(const auto& f : s.fields) {
        os << "    " << f << "\n";
    }
    os << "}\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const AnonymousStruct& s) {
    os << " {\n";
    for(const auto& f : s.fields) {
        os << "    " << f << "\n";
    }
    os << "}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const NamedDeclaredReference& ref) {
    os << ref.typeName;;
    return os;
}

std::ostream& operator<<(std::ostream& os, const FieldDataType& dataType) {
    if(const auto* p = std::get_if<PrimitiveDataType>(&dataType)) {
        os << *p;
    } else if (const auto* p = std::get_if<std::unique_ptr<FixedLengthList>>(&dataType)) {
        os << **p;
    } else if (const auto* p = std::get_if<std::unique_ptr<DynamicLengthList>>(&dataType)) {
        os << **p;
    } else if (const auto* p = std::get_if<std::unique_ptr<AnonymousStruct>>(&dataType)) {
        os << **p;
    } else if (const auto* p = std::get_if<std::unique_ptr<AnonymousUnion>>(&dataType)) {
        os << **p;
    } else if (const auto* p = std::get_if<NamedDeclaredReference>(&dataType)) {
        os << *p;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const UnionMemberDeclaration& member) {
    os << member.dataType << " @" << member.unionMemberId;
    return os;
}

std::ostream& operator<<(std::ostream& os, const AnonymousUnion& u) {
    os << " <\n";
    for(const auto& m : u.members) {
        os << "    " << m << ";\n";
    }
    os << ">";
    return os;
}

std::ostream& operator<<(std::ostream& os, const UnionDeclaration& u) {
    os << "union " << u.name << " @" << u.uid << " <\n";
    for(const auto& m : u.members) {
        os << "    " << m << "\n";
    }
    os << ">\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const AST& ast) {
    for(const auto& s : ast.structDeclarations) {
        os << s << "\n";
    }
    for(const auto& u : ast.unionDeclarations) {
        os << u << "\n";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const FieldDeclaration& field) {
    os << field.name << " @" << field.structFieldId <<  " : " << field.dataType;
    return os;
}