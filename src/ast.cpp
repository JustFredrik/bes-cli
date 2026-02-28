#pragma once
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <sstream>
#include <variant>
#include <unordered_map>

struct FieldDeclaration;
struct UnionDeclaration;
struct UnionMemberDeclaration;

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

std::ostream& operator<<(std::ostream& os, const PrimitiveDataType& dt) {
    switch(dt) {
        case PrimitiveDataType::Int8:   os << "Int8";   return os;
        case PrimitiveDataType::Int16:  os << "Int16";  return os;
        case PrimitiveDataType::Int32:  os << "Int32";  return os;
        case PrimitiveDataType::Int64:  os << "Int64";  return os;
        case PrimitiveDataType::Uint8:  os << "Uint8";  return os;
        case PrimitiveDataType::Uint16: os << "Uint16"; return os;
        case PrimitiveDataType::Uint32: os << "Uint32"; return os;
        case PrimitiveDataType::Uint64: os << "Uint64"; return os;
        case PrimitiveDataType::Float32:os << "Float32";return os;
        case PrimitiveDataType::Float64:os << "Float64";return os;
        case PrimitiveDataType::String: os << "String"; return os;
        case PrimitiveDataType::Void:   os <<  "Void";  return os;
    }
}

enum class DeclarationType {
    Struct,
    Union
};

std::ostream& operator<<(std::ostream& os, const DeclarationType& t) {
    if (t == DeclarationType::Struct) {
        os << "Struct"; return os;
    }
    os << "Union"; return os;
}

struct StructDeclaration {
    std::string_view name;
    std::vector<FieldDeclaration> fields;
    int uid;
};

struct AnonymousStruct {
    std::vector<FieldDeclaration> fields;
};


using DeclarationPointer = std::variant<
    StructDeclaration*,
    UnionDeclaration*
>;


struct NamedDeclaredReference {
    std::string_view typeName;
    DeclarationType type;
    DeclarationPointer decPointer;
    int uid;
};


struct UnionDeclaration {
    std::string_view name;
    std::vector<UnionMemberDeclaration> members;
    int uid;
};


struct AnonymousUnion {
    std::vector<UnionMemberDeclaration> members;
};

using FieldDataType = std::variant<
    PrimitiveDataType, 
    AnonymousStruct, 
    AnonymousUnion,
    NamedDeclaredReference
>;
std::ostream& operator<<(std::ostream& os, const FieldDataType& dataType) {
    if(const auto* p = std::get_if<PrimitiveDataType>(&dataType)) {
        os << *p;
    } else if (const auto* p = std::get_if<AnonymousStruct>(&dataType)) {
        os << *p;
    } else if (const auto* p = std::get_if<AnonymousUnion>(&dataType)) {
        os << *p;
    } else if (const auto* p = std::get_if<NamedDeclaredReference>(&dataType)) {
        os << *p;
    }
    return os;
}

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
    std::vector<UnionDeclaration> unionDeclarations;
    std::vector<StructDeclaration> structDeclarations;
};