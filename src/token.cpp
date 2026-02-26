#pragma once
#include <string>

enum class TokenType {
    Keyword,
    DataType,
    Identifier,
    Number,
    Hexadecimal,
    AtLabel,
    LeftCurlyBrace,
    RightCurlyBrace,
    LeftSquareBracket,
    RightSquareBracket,
    GreaterThan,
    LessThan,
    Colon,
    Comma,
    Semicolon,
    SingleLineComment,
    MultiLineComment,
    Annotation,
    EndOfFile,
    Unknown,
};

constexpr std::string_view token_type_to_string(TokenType type) {
    switch (type) {
        case TokenType::Keyword: return "Keyword";
        case TokenType::DataType: return "DataType";
        case TokenType::Identifier: return "Identifier";
        case TokenType::Number: return "Number";
        case TokenType::Hexadecimal: return "Hexadecimal";
        case TokenType::AtLabel: return "AtLabel";
        case TokenType::LeftCurlyBrace: return "LeftCurlyBrace";
        case TokenType::RightCurlyBrace: return "RightCurlyBrace";
        case TokenType::LeftSquareBracket: return "LeftSquareBracket";
        case TokenType::RightSquareBracket: return "RightSquareBracket";
        case TokenType::GreaterThan: return "GreaterThan";
        case TokenType::LessThan: return "LessThan";
        case TokenType::Colon: return "Colon";
        case TokenType::Comma: return "Comma";
        case TokenType::Semicolon: return "Semicolon";
        case TokenType::SingleLineComment: return "SingleLineComment";
        case TokenType::MultiLineComment: return "MultiLineComment";
        case TokenType::Annotation: return "Annotation";
        case TokenType::EndOfFile: return "EndOfFile";
        default: return "Unknown";
    }
}

struct Token {
    TokenType type;
    int line_number;
    int column_number;
    std::string_view lexeme;
};