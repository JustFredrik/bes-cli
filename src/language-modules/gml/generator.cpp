#pragma once
#include <string>
#include "../LanguageGenerator.cpp"
#include "static_code.cpp"
#include "../../utils/indent.cpp"

class GmlGenerator : public LanguageGenerator
{
public:
    GmlGenerator() : LanguageGenerator("gml", "GameMaker (v2024.14.3)", ".gml") {}

    GeneratedData generate(const AST &ast) override
    {
        std::ostringstream out;
        out << generate_uid_enum(ast);
        out << STATIC_BES_STRUCT_CODE;
        out << generate_constructors(ast);
        out << generate_encode_function(ast);
        std::cout << out.str();
        return GeneratedData{
            out.str(),
            getId(),
            getDefaultFileExtension()};
    }

    std::string generate_constructors(const AST &ast)
    {
        std::ostringstream out;
        for (const auto &s : ast.structDeclarations)
        {
            out << generate_struct_jsdoc(*s);
            out << generate_struct_signature(*s);
            out << generate_struct_field_assignments(*s);
            out << "}\n";
            out << "\n\n";
        }
        return out.str();
    }

    std::string generate_struct_field_assignments(const StructDeclaration &s)
    {
        std::ostringstream out;
        for (const auto &field : s.fields)
        {
            out << "    self." << field.name << " = " << field.name << ";\n";
        }
        return out.str();
    }

    std::string generate_struct_jsdoc(const StructDeclaration &s)
    {
        std::ostringstream out;
        out << "///@function BES_" << s.name << "(";
        size_t fieldCount = s.fields.size();
        for (int i = 0; i < fieldCount; i++)
        {
            out << s.fields[i].name << (i < fieldCount - 1 ? ", " : "");
        }
        out << ")\n";
        for (int i = 0; i < fieldCount; i++)
        {
            out << "///@param {" << field_data_type_to_gml_jsdoc_type(s.fields[i].dataType) << "} " << s.fields[i].name << "\n";
        }
        out << "///@returns {Struct.BES_" << s.name << "}\n";
        return out.str();
    }

    std::string field_data_type_to_gml_jsdoc_type(const FieldDataType &dataType)
    {
        if (const auto *p = std::get_if<PrimitiveDataType>(&dataType))
        {
            return primitive_data_type_to_gml_jsdoc_type(*p);
        }
        else if (const auto *p = std::get_if<std::unique_ptr<FixedLengthList>>(&dataType))
        {
            return "Array<" + field_data_type_to_gml_jsdoc_type((*p)->dataType) + ">";
        }
        else if (const auto *p = std::get_if<std::unique_ptr<DynamicLengthList>>(&dataType))
        {
            return "Array<" + field_data_type_to_gml_jsdoc_type((*p)->dataType) + ">";
        }
        else if (const auto *p = std::get_if<std::unique_ptr<AnonymousStruct>>(&dataType))
        {
            return "Struct";
        }
        else if (const auto *p = std::get_if<std::unique_ptr<AnonymousUnion>>(&dataType))
        {
            return "Any"; // TODO improve union type representation
        }
        else if (const auto *p = std::get_if<NamedDeclaredReference>(&dataType))
        {
            if (p->type == DeclarationType::Struct)
            {
                return "Struct.BES_" + std::string(p->typeName);
            }
            return "Any"; // TODO improved named union type representation
        }
        return "Unknown";
    }

    std::string primitive_data_type_to_gml_jsdoc_type(const PrimitiveDataType &dt)
    {
        switch (dt)
        {
        case PrimitiveDataType::Bool:
            return "Bool";
        case PrimitiveDataType::Int8:
        case PrimitiveDataType::Int16:
        case PrimitiveDataType::Int32:
        case PrimitiveDataType::Uint8:
        case PrimitiveDataType::Uint16:
        case PrimitiveDataType::Uint32:
        case PrimitiveDataType::Uint64:
        case PrimitiveDataType::Float16:
        case PrimitiveDataType::Float32:
        case PrimitiveDataType::Float64:
            return "Real";
        case PrimitiveDataType::String:
            return "string";
        case PrimitiveDataType::Void:
            return "undefined";
        default:
            return "Any";
        }
    }

    std::string primitive_data_type_to_gml_buffer_type(const PrimitiveDataType &dt)
    {
        switch (dt)
        {
        case PrimitiveDataType::Bool:
            return "buffer_bool";
        case PrimitiveDataType::Int8:
            return "buffer_s8";
        case PrimitiveDataType::Int16:
            return "buffer_s16";
        case PrimitiveDataType::Int32:
            return "buffer_s32";
        case PrimitiveDataType::Uint8:
            return "buffer_u8";
        case PrimitiveDataType::Uint16:
            return "buffer_u16";
        case PrimitiveDataType::Uint32:
            return "buffer_u32";
        case PrimitiveDataType::Uint64:
            return "buffer_u64";
        case PrimitiveDataType::Float16:
            return "buffer_f16";
        case PrimitiveDataType::Float32:
            return "buffer_u32";
        case PrimitiveDataType::Float64:
            return "buffer_f64";
        case PrimitiveDataType::String:
            return "buffer_string";
        case PrimitiveDataType::Void:
            return "undefined";
        default:
            return "Any";
        }
    }

    std::string generate_uid_enum(const AST &ast){
        std::ostringstream out;
        out << "enum BES_UID {\n";
        for(const auto& s : ast.structDeclarations) {
            out << indent(1) + std::string(s->name) + " = " + std::to_string(s->uid) + ": \n";         
        }
        out << "}\n";
        return out.str();
    }

    std::string generate_struct_signature(const StructDeclaration &s)
    {
        std::ostringstream out;
        out << "function BES_" << s.name << "(";
        size_t fieldCount = s.fields.size();
        for (int i = 0; i < fieldCount; i++)
        {
            out << s.fields[i].name << (i < fieldCount - 1 ? ", " : "");
        }
        out << ") : __BES_Struct(\"" << s.name << "\", " << "BES_UID." << s.name << ") constructor {\n";
        return out.str();
    }

    std::string generate_dataType_encoding(const PrimitiveDataType& d, const std::string& accessor, int ind) {
        return indent(ind) + "buffer_write(_buff, " + primitive_data_type_to_gml_buffer_type(d) + ", " + std::string(accessor) +");\n";
    }

    std::string generate_dataType_encoding(const FixedLengthList& fl, const std::string& accessor, int ind) {
        return indent(ind) + "// TODO Implement fixed length list encoding \n";
    }

    std::string generate_dataType_encoding(const DynamicLengthList& fl, const std::string& accessor, int ind) {
        return indent(ind) + "// TODO Implement dynamic length list encoding \n";
    }

    std::string generate_dataType_encoding(const AnonymousStruct& s, const std::string& accessor, std::string_view fieldName, int ind) {
        std::ostringstream out;
        out << indent(ind) << "// " << "anonymous struct: " << std::string(accessor) << "\n";
        for (const auto& f : s.fields) {
            out << generate_field_encoding(f, accessor + "." + std::string(f.name), ind + 1);
        }
        return out.str();
    }

    std::string generate_dataType_encoding(const AnonymousUnion& u, const std::string& accessor, int ind) {
        return indent(ind) + "// TODO Implement anonymous union encoding \n";
    }

    std::string generate_dataType_encoding(const NamedDeclaredReference& ref, const std::string& accessor, int ind) {
        
        std::ostringstream out;
        out << indent(ind) << "// " << "Named declared reference: ";
        if (const auto* p = std::get_if<std::shared_ptr<StructDeclaration>>(&ref.decPointer)) {
            out << (**p).name << "\n";
            for (const auto& f : (**p).fields) {
                out << generate_field_encoding(f, accessor + "." + std::string(f.name), ind + 1);
            }
        } else if (const auto* p = std::get_if<std::shared_ptr<UnionDeclaration>>(&ref.decPointer)) {
            out << "// TODO implement\n";
        }
        return out.str();    
    }

    std::string generate_field_encoding(const FieldDeclaration& f, std::string_view accessor, int ind) {        
        if(const auto* p = std::get_if<PrimitiveDataType>(&f.dataType)) {
            return generate_dataType_encoding(*p, std::string(accessor), ind);
        } else if(const auto* p = std::get_if<std::unique_ptr<FixedLengthList>>(&f.dataType)) {
            return generate_dataType_encoding(**p, std::string(accessor), ind);
        } else if(const auto* p = std::get_if<std::unique_ptr<DynamicLengthList>>(&f.dataType)) {
            return generate_dataType_encoding(**p, std::string(accessor), ind);
        } else if(const auto* p = std::get_if<std::unique_ptr<AnonymousStruct>>(&f.dataType)) {
            return generate_dataType_encoding(**p, std::string(accessor), f.name, ind);
        } else if(const auto* p = std::get_if<std::unique_ptr<AnonymousUnion>>(&f.dataType)) {
            return generate_dataType_encoding(**p, std::string(accessor), ind);
        } else if(const auto* p = std::get_if<NamedDeclaredReference>(&f.dataType)) {
            return generate_dataType_encoding(*p, std::string(accessor), ind);
        }
        return indent(ind) + "// Not implmented data type\n";
    }

    std::string generate_encode_function(const AST &ast){
        std::ostringstream out;
        out << "function bes_encode(_struct, _buff, _skip_header) {\n";
        out << "if (!_skip_header) {" << " buffer_write(_buff, buffer_u16, _struct.__bes_uid) " << "}\n";
        out << indent(1) + "switch(_struct.__bes_uid) {\n";
        for(const auto& s : ast.structDeclarations) {
            out << indent(2) + "case " + "BES_UID." + std::string(s->name) + ": \n";
            for (const auto& f : s->fields) {
                out << generate_field_encoding(f, "_struct." + std::string(f.name), 3);
            }
            out << indent(3) + "return 1;\n";               
        }
        out << indent(1) <<"}\n}";
        return out.str();
    }

    std::string generate_decode_function(const AST &ast){
        std::ostringstream out;
        out << "function bes_encode(_struct) {\n";
        out << "\n";
        out << "}";
        return out.str();
    }
};