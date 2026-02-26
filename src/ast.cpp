#pragma once
#include <vector>
#include <memory>
#include <string>
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

enum class DeclarationType {
    Struct,
    Union
};

struct StructDeclaration {
    std::string_view name;
    std::vector<FieldDeclaration> fields;
    int uid;
};

struct AnonymousStruct {
    std::vector<FieldDeclaration> fields;
    int structFieldId;
};


struct NamedDeclaredReference {
    std::string_view typeName;
    DeclarationType type;
    int uid;
};


struct UnionDeclaration {
    std::string_view name;
    std::vector<UnionMemberDeclaration> members;
    int uid;
};


struct AnonymousUnion {
    std::vector<UnionMemberDeclaration> members;
    int structFieldId;
};

using FieldDataType = std::variant<
    PrimitiveDataType, 
    AnonymousStruct, 
    AnonymousUnion,
    NamedDeclaredReference
>;

struct UnionType {
    std::string_view name;
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
    std::vector<UnionDeclaration> unionDeclarations;
    std::vector<StructDeclaration> structDeclarations;
};