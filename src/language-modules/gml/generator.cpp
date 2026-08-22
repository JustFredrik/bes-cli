#pragma once
#include "../LanguageGenerator.cpp"

class GmlGenerator : public LanguageGenerator
{
public:
    GmlGenerator() : LanguageGenerator("gml", "GameMaker (v2024.14.3)", ".gml") {}

    GeneratedData generate(const AST &ast) override
    {
        std::ostringstream out;
        out << generate_constructors(ast);
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

    std::string generate_struct_signature(const StructDeclaration &s)
    {
        std::ostringstream out;
        out << "function BES_" << s.name << "(";
        size_t fieldCount = s.fields.size();
        for (int i = 0; i < fieldCount; i++)
        {
            out << s.fields[i].name << (i < fieldCount - 1 ? ", " : "");
        }
        out << ") : __BES_Struct(\"" << s.name << "\"," << s.uid << ") constructor {\n";
        return out.str();
    }

    std::string generate_encode_function(const AST &ast){
        std::ostringstream out;
        out << "function bes_encode(_struct) {\n";
        
        out << "}";
        return out.str();
    }
};