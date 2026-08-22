#pragma once
#include <stdexcept>
#include <filesystem>
#include <unordered_map>
#include <string>
#include <algorithm>
#include "ast.cpp"
#include "token.cpp"
#include "errors.cpp"
#include "utils/hash.cpp"

FieldDeclaration parse_field_declaration(std::vector<Token> &tokens, int &i, std::unordered_map<std::string, bool> &nameLookup, std::unordered_map<int, bool> &uidLookup, int &fieldIdInc, std::unordered_map<std::string, DeclarationPointer> declarationLookup);
FieldDataType parse_field_data_type(std::vector<Token> &tokens, int &i, std::unordered_map<std::string, DeclarationPointer> declarationLookup);
std::vector<UnionMemberDeclaration> parse_union_members(std::vector<Token> &tokens, int &i, std::unordered_map<std::string, DeclarationPointer> declarationLookup);
PrimitiveDataType string_to_primitive_data_type(const std::string_view &s);

struct Alias
{
    std::string_view identifierString;  // Identifier lexeme to match with
    std::string_view replacementString; // lexeme to replace with
    TokenType replacementType;
};

bool match(std::vector<Token> &tokens, int &i, TokenType type)
{
    return !(i >= tokens.size() || tokens[i].type != type);
}

bool match(std::vector<Token> &tokens, int &i, std::vector<TokenType> types)
{
    return !(i >= tokens.size() || std::find(types.begin(), types.end(), tokens[i].type) == types.end());
}

Token &consume(std::vector<Token> &tokens, int &i, TokenType type)
{
    if (i >= tokens.size() || tokens[i].type != type)
    {
        throw UnexpectedTokenError(tokens[i].lexeme, type, tokens[i].line_number, tokens[i].column_number);
    }
    return tokens[i++];
}

Token &consume(std::vector<Token> &tokens, int &i, std::vector<TokenType> types)
{
    if (i >= tokens.size() || std::find(types.begin(), types.end(), tokens[i].type) == types.end())
    {
        throw UnexpectedTokenError(tokens[i].lexeme, types, tokens[i].line_number, tokens[i].column_number);
    }
    return tokens[i++];
}

std::vector<Alias> extract_aliases(std::vector<Token> &tokens)
{
    std::vector<Alias> aliases;
    auto size = tokens.size();
    int i = 0;
    while (i < size)
    {

        if (tokens[i].type == TokenType::Keyword && tokens[i].lexeme == "alias")
        {
            int startIndex = i;

            if (i + 1 >= size || tokens[i + 1].type != TokenType::Identifier)
            {
                throw UnexpectedTokenError(
                    tokens[i + 1].lexeme,
                    TokenType::Identifier,
                    tokens[i + 1].line_number,
                    tokens[i + 1].column_number);
            }
            auto aliasName = tokens[i + 1].lexeme;

            if (i + 2 >= size ||
                (tokens[i + 2].type != TokenType::Number &&
                 tokens[i + 2].type != TokenType::Hexadecimal &&
                 tokens[i + 2].type != TokenType::DataType))
            {

                std::vector<TokenType> expected = {TokenType::Number, TokenType::Hexadecimal, TokenType::DataType};

                throw UnexpectedTokenError(
                    tokens[i + 2].lexeme,
                    expected,
                    tokens[i + 2].line_number,
                    tokens[i + 2].column_number);
            }
            aliases.push_back({aliasName,
                               tokens[i + 2].lexeme,
                               tokens[i + 2].type});

            auto eraseEnd = i + 3;
            if (eraseEnd <= size && tokens[i + 3].type == TokenType::Semicolon)
            {
                eraseEnd++;
            }
            tokens.erase(tokens.begin() + i, tokens.begin() + eraseEnd);
            continue;
        }
        i++;
    }

    return aliases;
}

void replace_aliases(std::vector<Token> &tokens, std::vector<Alias> &aliases)
{
    for (Alias a : aliases)
    {
        for (Token &t : tokens)
        {
            if (t.type == TokenType::Identifier && t.lexeme == a.identifierString)
            {
                t.lexeme = a.replacementString;
                t.type = a.replacementType;
            }
        }
    }
}

void remove_comments(std::vector<Token> &tokens)
{
    int i = 0;
    auto b = tokens.begin();
    while (i < tokens.size())
    {
        while (match(tokens, i, {TokenType::SingleLineComment, TokenType::MultiLineComment}))
        {
            tokens.erase(b + i);
        }
        i++;
    }
}

void pre_process_tokens(std::vector<Token> &tokens)
{
    auto aliases = extract_aliases(tokens);
    replace_aliases(tokens, aliases);
    remove_comments(tokens);
}

void consume_trailing_seperator(std::vector<Token> &tokens, int &i)
{
    auto seperators = {TokenType::Comma, TokenType::Semicolon};
    while (match(tokens, i, seperators))
    {
        consume(tokens, i, seperators);
    }
}

int parse_uid(std::vector<Token> &tokens, int &i, std::unordered_map<int, bool> &uidLookup, std::string_view declarationName)
{
    // Static id
    if (match(tokens, i, TokenType::Annotation))
    {
        consume(tokens, i, TokenType::Annotation);
        auto uidToken = consume(tokens, i, {TokenType::Number, TokenType::Hexadecimal});
        int uid;
        if (uidToken.type == TokenType::Hexadecimal)
        {
            uid = stoi(std::string(uidToken.lexeme), nullptr, 16);
        }
        else
        {
            uid = stoi(std::string(uidToken.lexeme), nullptr, 10);
        }
        if (uidLookup[uid])
        {
            throw DuplicateStaticIdError(uidToken);
        }
        uidLookup[uid] = true;
        return uid;
    }
    // Dynamic id
    uint16_t hashDeclarationName = fnv1a_hash_u16(std::string(declarationName));
    while (uidLookup[int(hashDeclarationName)])
    {
        hashDeclarationName += 1;
    }
    uidLookup[int(hashDeclarationName)] = true;
    return hashDeclarationName;
}

int parse_uid(std::vector<Token> &tokens, int &i, std::unordered_map<int, bool> &uidLookup, int currentUidInc)
{
    // Static id
    if (match(tokens, i, TokenType::Annotation))
    {
        consume(tokens, i, TokenType::Annotation);
        auto uidToken = consume(tokens, i, {TokenType::Number, TokenType::Hexadecimal});
        int uid;
        if (uidToken.type == TokenType::Hexadecimal)
        {
            uid = stoi(std::string(uidToken.lexeme), nullptr, 16);
        }
        else
        {
            uid = stoi(std::string(uidToken.lexeme), nullptr, 10);
        }
        if (uidLookup[uid])
        {
            throw DuplicateStaticIdError(uidToken);
        }
        uidLookup[uid] = true;
        return uid;
    }
    // Dynamic id
    while (uidLookup[currentUidInc])
    {
        currentUidInc += 1;
    }
    uidLookup[currentUidInc] = true;
    return currentUidInc;
}

Token &consume_name_declaration(std::vector<Token> &tokens, int &i, std::unordered_map<std::string, bool> &nameLookup)
{
    if (nameLookup[std::string(tokens[i].lexeme)] == true)
    {
        throw DuplicateNameError(tokens[i]);
    }
    return tokens[i++];
}

std::tuple<ListType, int> consume_list_brackets(std::vector<Token> &tokens, int &i)
{
    ListType type = ListType::FixedLength;
    consume(tokens, i, TokenType::LeftSquareBracket);
    if (match(tokens, i, TokenType::Number))
    {
        auto lengthToken = consume(tokens, i, TokenType::Number);
        consume(tokens, i, TokenType::RightSquareBracket);
        return {ListType::FixedLength, stoi(std::string(lengthToken.lexeme))};
    }
    consume(tokens, i, TokenType::RightSquareBracket);
    return {ListType::DynamicLength, NULL};
}

std::vector<FieldDeclaration> parse_field_declarations(std::vector<Token> &tokens, int &i, std::unordered_map<std::string, DeclarationPointer> declarationLookup)
{
    std::vector<FieldDeclaration> fields = {};
    std::unordered_map<std::string, bool> nameLookup;
    int fieldIdInc = 0;
    std::unordered_map<int, bool> uidLookup;
    while (i < tokens.size() && tokens[i].type != TokenType::RightCurlyBrace)
    {
        fields.push_back(std::move(parse_field_declaration(tokens, i, nameLookup, uidLookup, fieldIdInc, declarationLookup)));
        consume_trailing_seperator(tokens, i);
    }
    consume(tokens, i, TokenType::RightCurlyBrace);
    return fields;
}

FieldDeclaration parse_field_declaration(
    std::vector<Token> &tokens,
    int &i,
    std::unordered_map<std::string, bool> &nameLookup,
    std::unordered_map<int, bool> &uidLookup,
    int &fieldIdInc,
    std::unordered_map<std::string, DeclarationPointer> declarationLookup)
{
    std::string_view fieldName = consume_name_declaration(tokens, i, nameLookup).lexeme;
    uint16_t uid = parse_uid(tokens, i, uidLookup, fieldIdInc);
    consume(tokens, i, TokenType::Colon);
    FieldDataType dataType = parse_field_data_type(tokens, i, declarationLookup);

    return FieldDeclaration(fieldName, uid, std::move(dataType));
}

FieldDataType parse_anonymous_union(std::vector<Token> &tokens, int &i, std::unordered_map<std::string, DeclarationPointer> declarationLookup)
{
    AnonymousUnion u{
        parse_union_members(tokens, i, declarationLookup),
    };
    return FieldDataType{std::make_unique<AnonymousUnion>(std::move(u))};
}

FieldDataType parse_anonymous_struct(std::vector<Token> &tokens, int &i, std::unordered_map<std::string, DeclarationPointer> declarationLookup)
{
    AnonymousStruct s{
        parse_field_declarations(tokens, i, declarationLookup)};
    return FieldDataType{std::make_unique<AnonymousStruct>(std::move(s))};
}

FieldDataType parse_field_data_type(std::vector<Token> &tokens, int &i, std::unordered_map<std::string, DeclarationPointer> declarationLookup)
{
    Token fieldData = consume(tokens, i, {TokenType::Identifier, TokenType::DataType, TokenType::LessThan, TokenType::LeftCurlyBrace});
    FieldDataType d;
    if (fieldData.type == TokenType::Identifier)
    {
        auto &declarationPointer = declarationLookup[std::string(fieldData.lexeme)];
        if (std::holds_alternative<std::monostate>(declarationPointer))
        {
            throw UndeclaredReferenceError(fieldData);
        }
        else if (std::holds_alternative<StructDeclaration *>(declarationPointer))
        {
            d = FieldDataType{NamedDeclaredReference{
                fieldData.lexeme,
                DeclarationType::Struct,
                declarationPointer}};
        }
        else if (std::holds_alternative<UnionDeclaration *>(declarationPointer))
        {
            d = FieldDataType{NamedDeclaredReference{
                fieldData.lexeme,
                DeclarationType::Union,
                declarationPointer}};
        }
    }
    else if (fieldData.type == TokenType::DataType)
    {
        d = FieldDataType{string_to_primitive_data_type(fieldData.lexeme)}; // TODO output actual dataType instead of defaulting to float32
    }
    else if (fieldData.type == TokenType::LessThan)
    {
        d = parse_anonymous_union(tokens, i, declarationLookup);
    }
    else if (fieldData.type == TokenType::LeftCurlyBrace)
    {
        d = parse_anonymous_struct(tokens, i, declarationLookup);
    }
    if (match(tokens, i, TokenType::LeftSquareBracket))
    {
        auto [listType, listLength] = consume_list_brackets(tokens, i);
        if (listType == ListType::FixedLength)
        {
            FixedLengthList l{
                listLength,
                std::move(d)};
            return FieldDataType{std::make_unique<FixedLengthList>(std::move(l))};
        }
        if (listType == ListType::DynamicLength)
        {
            DynamicLengthList l{
                std::move(d)};
            return FieldDataType{std::make_unique<DynamicLengthList>(std::move(l))};
        }
    }
    return d;
}

UnionMemberDeclaration parse_union_member(std::vector<Token> &tokens, int &i, std::unordered_map<int, bool> &uidLookup, int &uidInc, std::unordered_map<std::string, DeclarationPointer> declarationLookup)
{
    FieldDataType dataType = parse_field_data_type(tokens, i, declarationLookup);
    int uid = parse_uid(tokens, i, uidLookup, uidInc);
    uidInc = uid + 1;
    return UnionMemberDeclaration{std::move(dataType), uid};
}

std::vector<UnionMemberDeclaration> parse_union_members(std::vector<Token> &tokens, int &i, std::unordered_map<std::string, DeclarationPointer> declarationLookup)
{
    std::vector<UnionMemberDeclaration> members;
    int uidInc = 0;
    std::unordered_map<int, bool> uidLookup = {};
    while (i < tokens.size() && tokens[i].type != TokenType::GreaterThan)
    {
        members.push_back(parse_union_member(tokens, i, uidLookup, uidInc, declarationLookup));
        consume_trailing_seperator(tokens, i);
    }
    consume(tokens, i, TokenType::GreaterThan);
    return members;
}

AST parse(std::vector<Token> &tokens)
{
    pre_process_tokens(tokens);

    AST root;
    std::unordered_map<std::string, bool> nameLookup; // globally scoped unique names
    std::unordered_map<int, bool> uidLookup;          // globally scoped uids

    int i = 0;
    auto size = tokens.size();
    Token t;
    while (i < size)
    {
        t = consume(tokens, i, {TokenType::Keyword, TokenType::EndOfFile});

        if (t.lexeme == "struct")
        {
            StructDeclaration s;
            s.name = consume_name_declaration(tokens, i, nameLookup).lexeme;
            s.uid = parse_uid(tokens, i, uidLookup, s.name);
            consume(tokens, i, TokenType::LeftCurlyBrace);
            s.fields = parse_field_declarations(tokens, i, root.declarationLookup);
            root.declarationLookup[std::string(s.name)] = &s;
            root.structDeclarations.push_back(std::move(s));
        }
        else if (t.lexeme == "union")
        {
            UnionDeclaration u;
            u.name = consume_name_declaration(tokens, i, nameLookup).lexeme;
            u.uid = parse_uid(tokens, i, uidLookup, u.name);
            consume(tokens, i, TokenType::LessThan);
            u.members = parse_union_members(tokens, i, root.declarationLookup);
            root.declarationLookup[std::string(u.name)] = &u;
            root.unionDeclarations.push_back(std::move(u));
        }
        else if (t.type == TokenType::EndOfFile)
        {
            return root;
        }
        else
        {
            throw std::runtime_error("Unknown keyword: " + std::string(tokens[i].lexeme));
        }
    }
    return root;
}

PrimitiveDataType string_to_primitive_data_type(const std::string_view &s)
{
    if (s == "bool")
    {
        return PrimitiveDataType::Bool;
    }
    else if (s == "int8")
    {
        return PrimitiveDataType::Int8;
    }
    else if (s == "int16")
    {
        return PrimitiveDataType::Int16;
    }
    else if (s == "int32")
    {
        return PrimitiveDataType::Int32;
    }
    else if (s == "uint8")
    {
        return PrimitiveDataType::Uint8;
    }
    else if (s == "uint16")
    {
        return PrimitiveDataType::Uint16;
    }
    else if (s == "uint32")
    {
        return PrimitiveDataType::Uint32;
    }
    else if (s == "uint64")
    {
        return PrimitiveDataType::Uint64;
    }
    else if (s == "float16")
    {
        return PrimitiveDataType::Float16;
    }
    else if (s == "float32")
    {
        return PrimitiveDataType::Float32;
    }
    else if (s == "float64")
    {
        return PrimitiveDataType::Float64;
    }
    else if (s == "string")
    {
        return PrimitiveDataType::String;
    }
    else if (s == "void")
    {
        return PrimitiveDataType::Void;
    }
    throw std::runtime_error("Unknown data type: " + std::string(s));
}
