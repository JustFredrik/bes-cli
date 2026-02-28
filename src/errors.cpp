#pragma once
#include <stdexcept>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#include "token.cpp"


std::string joinWithCommas(const std::vector<std::string>&elements, const std::string lastSeperator) {
    std::ostringstream oss;
    switch(elements.size()) {
        case 0:
            return "";
        case 1:
            oss << elements[0];
            break;
        case 2:
            oss << elements[0] << " " << lastSeperator << " " << elements[1];
            break;
        default:
            for (size_t i = 0; i < elements.size(); ++i) {
                oss << elements[i];
                if (i == elements.size() - 2) {
                    oss << " " << lastSeperator << " ";
                } else if (i < elements.size() -1) {
                    oss << ", ";
                }

            }
    }
    return oss.str();
}

std::vector<std::string> mapTokenTypeArrayToStrings(std::vector<TokenType> tokenTypes) {
    std::vector<std::string> tokenStrings;
    for (TokenType t : tokenTypes) {
        tokenStrings.push_back(std::string(token_type_to_string(t)));
    }
    return tokenStrings;
}

enum class SyntaxErrorType {
    UnexpectedToken,
    RedeclarationOfIdentifier
};

class UnexpectedTokenError : public std::runtime_error {
public:
    UnexpectedTokenError(const std::string_view got, const TokenType expected, int line, int col) 
        : std::runtime_error("Unexpected token, got '" + std::string(got) + "' expected '" + std::string(token_type_to_string(expected)) + "' ("  + std::to_string(line) + ":" + std::to_string(col) + ")") {}
  
    UnexpectedTokenError(const std::string_view got, const std::vector<TokenType> expected, int line, int col) 
    : std::runtime_error("Unexpected token, got '" + std::string(got) + "' expected '" + joinWithCommas(mapTokenTypeArrayToStrings(expected), "or") + "' ("  + std::to_string(line) + ":" + std::to_string(col) + ")") {}
    
};

class DuplicateNameError : public std::runtime_error {
public:
    DuplicateNameError(const Token& t) 
        : std::runtime_error("Duplicate name in declaration '" + std::string(t.lexeme) + "' ("  + std::to_string(t.line_number) + ":" + std::to_string(t.column_number) + ")") {}
};

class DuplicateStaticIdError : public std::runtime_error {
public:
    DuplicateStaticIdError(const Token& t) 
        : std::runtime_error("Duplicate static id in declaration @'" + std::string(t.lexeme) + "' ("  + std::to_string(t.line_number) + ":" + std::to_string(t.column_number) + ")") {}
};

class UndeclaredReferenceError : public std::runtime_error {
public:
    UndeclaredReferenceError(const Token& t) 
        : std::runtime_error("Trying to reference '" + std::string(t.lexeme) + "' before declaration ("  + std::to_string(t.line_number) + ":" + std::to_string(t.column_number) + ")") {}
};


