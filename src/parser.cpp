#pragma once
#include <stdexcept>
#include <filesystem>
#include <unordered_map>
#include <string>
#include <algorithm>
#include "ast.cpp"
#include "token.cpp"
#include "errors.cpp"


uint16_t fnv1a_hash_u16(const std::string& s);


struct Alias {
    std::string_view identifierString; // Identifier lexeme to match with
    std::string_view replacementString; // lexeme to replace with
    TokenType replacementType;
};


std::vector<Alias> extract_aliases(std::vector<Token>& tokens) {
    std::vector<Alias> aliases;
    auto size = tokens.size();
    int i = 0;
    while (i < size) {
        
        if (tokens[i].type == TokenType::Keyword && tokens[i].lexeme == "alias") {
            int startIndex = i; 

            if ( i+1 >= size  || tokens[i+1].type != TokenType::Identifier ) {
                throw ErrorUnexpectedToken(
                    tokens[i+1].lexeme, 
                    TokenType::Identifier, 
                    tokens[i+1].line_number, 
                    tokens[i+1].column_number
                );
            }
            auto aliasName = tokens[i+1].lexeme;
            
            if (i+2 >= size ||
                (tokens[i+2].type != TokenType::Number && 
                tokens[i+2].type != TokenType::Hexadecimal && 
                tokens[i+2].type != TokenType::DataType)) {

                    std::vector<TokenType> expected = {TokenType::Number, TokenType::Hexadecimal, TokenType::DataType};

                    throw ErrorUnexpectedToken(
                        tokens[i+2].lexeme, 
                        expected, 
                        tokens[i+2].line_number, 
                        tokens[i+2].column_number
                    );
              }
            aliases.push_back({
                aliasName,
                tokens[i+2].lexeme,
                tokens[i+2].type
            });

            auto eraseEnd = i+3;
            if (eraseEnd <= size && tokens[i+3].type == TokenType::Semicolon) {
                eraseEnd++;
            }
            tokens.erase(tokens.begin() + i, tokens.begin() + eraseEnd);
            continue;
        }
        i++;
    }

    return aliases;
}


void replace_aliases(std::vector<Token>& tokens, std::vector<Alias>& aliases) {
     for (Alias a : aliases) {
        for(Token& t : tokens) {
            if (t.type == TokenType::Identifier && t.lexeme == a.identifierString) {
                t.lexeme = a.replacementString;
                t.type = a.replacementType;
            }
        }
    }
}


void pre_process_tokens(std::vector<Token>& tokens) {
    auto aliases = extract_aliases(tokens);
    replace_aliases(tokens, aliases);
   
}


Token& consume(std::vector<Token>& tokens, int& i, TokenType type) {
    if (i >= tokens.size() || tokens[i].type != type) {
        throw ErrorUnexpectedToken(tokens[i].lexeme, type, tokens[i].line_number, tokens[i].column_number);
    }
    return tokens[i++];
}


Token& consume(std::vector<Token>& tokens, int& i, std::vector<TokenType>types) {
    if (i >= tokens.size() || std::find(types.begin(), types.end(), tokens[i].type) == types.end()) {
          throw ErrorUnexpectedToken(tokens[i].lexeme, types, tokens[i].line_number, tokens[i].column_number);
    }
    return tokens[i++];
}


int parse_uid(std::vector<Token>& tokens, int& i, std::unordered_map<int, bool>& uidLookup, std::string_view declarationName) {
    // Static id
    if (match(tokens, i, TokenType::Annotation)) {
        consume(tokens, i, TokenType::Annotation);
        auto uidToken = consume(tokens, i, {TokenType::Number, TokenType::Hexadecimal});
        
        return 1;
    }
    // Dynamic id
    uint16_t hashDeclarationName = fnv1a_hash_u16(std::string(declarationName));
    while(uidLookup[int(hashDeclarationName)]) {
        hashDeclarationName += 1;
    }
    return hashDeclarationName;
}


bool match(std::vector<Token>& tokens, int& i, TokenType type) {
    return !(i >= tokens.size() || tokens[i].type != type);
}


Token& consume_name_declaration(std::vector<Token>& tokens, int& i, std::unordered_map<std::string, bool>& nameLookup) {
    if (nameLookup[std::string(tokens[i].lexeme)] = true) {
        throw ErrorDuplicateName(tokens[i]);
    }
    return tokens[i++];
}


uint16_t fnv1a_hash_u16(const std::string& s) {
    uint32_t hash = 0x811c9dc5;
    const uint32_t prime = 0x01000193;
    for (char c : s) {
        hash ^= static_cast<uint8_t>(c);
        hash *= prime;
    }
    return static_cast<uint16_t>((hash >> 16) ^ (hash & 0xFFFF));
}

std::vector<FieldDeclaration> parse_field_declarations(std::vector<Token>& tokens, int& i) {
    std::vector<FieldDeclaration> fields = {};
    std::unordered_map<std::string, bool> nameLookup;
    std::unordered_map<int, bool> uidLookup;
    while (i < tokens.size() && tokens[i].type != TokenType::RightCurlyBrace) {
        fields.push_back(parse_field_declaration(tokens, i, nameLookup, uidLookup));
    }
    consume(tokens, i, TokenType::RightCurlyBrace);
    return fields;
}

FieldDeclaration parse_field_declaration(
    std::vector<Token>& tokens, 
    int& i, 
    std::unordered_map<std::string, bool>& nameLookup,
    std::unordered_map<int, bool>& uidLookup) 
    {
    std::string_view fieldName = consume_name_declaration(tokens, i, nameLookup).lexeme;
    uint16_t uid = parse_uid(tokens, i, uidLookup, fieldName);
    consume(tokens, i, TokenType::Colon);
    FieldDataType dataType = parse_field_data_type(tokens, i);

    return FieldDeclaration(fieldName, uid, std::move(dataType));
}

// TODO Implement
FieldDataType parse_anonymous_union(std::vector<Token>& tokens, int& i) {
    return FieldDataType{AnonymousUnion{}};
}

// TODO Implement
FieldDataType parse_anonymous_struct(std::vector<Token>& tokens, int& i) {
    return FieldDataType{AnonymousUnion{}};
}

FieldDataType parse_field_data_type(std::vector<Token>& tokens, int& i) {
    Token fieldData = consume(tokens, i, {TokenType::Identifier, TokenType::DataType, TokenType::LessThan, TokenType::LeftCurlyBrace});
    if (fieldData.type == TokenType::Identifier) {
        return FieldDataType{NamedDeclaredReference{
            fieldData.lexeme,
            DeclarationType::Struct,
            i
        }};
    } else if (fieldData.type == TokenType::DataType) {
        return FieldDataType{PrimitiveDataType::Float32};}
    else if (fieldData.type == TokenType::LessThan) {
        return parse_anonymous_union(tokens, i);
    } else if (fieldData.type == TokenType::LeftCurlyBrace) {
        return parse_anonymous_struct(tokens, i);
    } else {
        throw std::runtime_error("Invalid field data type");
    }
}

// TODO implement
UnionMemberDeclaration parse_union_member(std::vector<Token>& tokens, int& i) {
    
}

std::vector<UnionMemberDeclaration> parse_union_members(std::vector<Token>& tokens, int& i) {
    std::vector<UnionMemberDeclaration> members;
    while (i < tokens.size() && tokens[i].type != TokenType::GreaterThan) {
        members.push_back(parse_union_member(tokens, i));
    }
}

void parse(std::vector<Token>& tokens) {
    pre_process_tokens(tokens);

    AST root;
    std::unordered_map<std::string, bool> nameLookup;
    std::unordered_map<int, bool> uidLookup;
    int i = 0;
    auto size = tokens.size();
    while ( i < size) {
        if (tokens[i].type == TokenType::Keyword) {

            if (tokens[i].lexeme == "struct") {
                StructDeclaration s;
                s.name = consume_name_declaration(tokens, i, nameLookup).lexeme;
                s.uid = parse_uid(tokens, i, uidLookup, s.name);
                s.fields = parse_field_declarations(tokens, i);
            
            } else if (tokens[i].lexeme == "union") {
                UnionDeclaration u;
                u.name = consume_name_declaration(tokens, i, nameLookup).lexeme;
                u.uid = parse_uid(tokens, i, uidLookup, u.name);
                u.members = parse_union_members(tokens, i);
            }

        }
    }
}
