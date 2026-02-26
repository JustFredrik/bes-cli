#pragma once
#include <filesystem>
#include <string>
#include "token.cpp"

bool both_chars_are_spaces(char left, char right) { return (left == right) && (left == ' '); }

void remove_spaces(std::string& str) {
    std::string::iterator new_end = std::unique(str.begin(), str.end(), both_chars_are_spaces);
    str.erase(new_end, str.end());   
}

bool is_keyword(const std::string_view& str) {
    return (
        str == "struct"|| 
        str == "union" || 
        str == "alias"
    );
}

bool is_dataType(const std::string_view& str) {
        return (
        str == "int8" ||
        str == "int16" ||
        str == "int32" ||
        str == "int64" ||
        str == "uint8" ||
        str == "uint16" ||
        str == "uint32" ||
        str == "uint64" ||
        str == "float32" ||
        str == "float64" ||
        str == "string" ||
        str == "void"
    );
}

bool is_space(char c) {
    switch(c) {
        case ' ':
        case '\t':
            return true;
        default:
            return false;
    }
    return false;
}

bool is_decimal(char c) {
    switch (c) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return true;
        default:
            return false;
    }
    return false;
}

bool is_hex_char(char c) {
    switch (c) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            return true;
        default:
            return false;
    }
    return false;
}

bool is_identifier_char(char c) {
    switch (c) {
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y': 
        case 'Z':
        case '_':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        return true;
        default:
         return false;
    }
    return false;
}

std::vector<Token> tokenize(const std::string& input) {
    std::vector<Token> tokens;
    int pos = 0, line_number = 1, col_acc = 0, look_ahead_pos = 0, input_length = input.length();
    char current_char = input[pos];
    while (pos < input_length) {
        switch(input[pos]) {
            case NULL:
                tokens.push_back({TokenType::EndOfFile, line_number, pos - col_acc, std::string_view(&input[pos], 1)});
                return tokens;
            case ' ':
            case '\t':
            case '\r':
            pos++;
            break;
            case '\n':
                col_acc=pos;
                line_number++;
                pos++;
                break;
            case '{':
                tokens.push_back({TokenType::LeftCurlyBrace, line_number, pos - col_acc, std::string_view(&input[pos], 1)});
                pos++;
                break;
            case '}':
                tokens.push_back({TokenType::RightCurlyBrace, line_number, pos - col_acc, std::string_view(&input[pos], 1)});
                pos++;
                break;
            case '[':
                tokens.push_back({TokenType::LeftSquareBracket, line_number, pos - col_acc, std::string_view(&input[pos], 1)});
                pos++;
                break;
            case ']':
                tokens.push_back({TokenType::RightSquareBracket, line_number, pos - col_acc, std::string_view(&input[pos], 1)});
                pos++;
                break;
            case '<':
                tokens.push_back({TokenType::LessThan, line_number, pos - col_acc, std::string_view(&input[pos], 1)});
                pos++;
                break;
            case '>':
                tokens.push_back({TokenType::GreaterThan, line_number, pos - col_acc, std::string_view(&input[pos], 1)});
                pos++;
                break;
            case ':':
                tokens.push_back({TokenType::Colon, line_number, pos - col_acc, std::string_view(&input[pos], 1)});
                pos++;
                break;
            case ',':
                tokens.push_back({TokenType::Comma, line_number, pos - col_acc, std::string_view(&input[pos], 1)});
                pos++;
                break;
            case ';':
                tokens.push_back({TokenType::Semicolon, line_number, pos - col_acc, std::string_view(&input[pos], 1)});
                pos++;
                break;
            case '/':
                if (pos+1 <= input_length && input[pos+1] == '/') {
                    pos += 2;
                    look_ahead_pos = pos;
                    while(look_ahead_pos <= input_length && is_space(input[look_ahead_pos])) { look_ahead_pos++; } 
                    while(look_ahead_pos <= input_length && input[look_ahead_pos] != '\r' && input[look_ahead_pos] != '\n') { look_ahead_pos++; }
                    tokens.push_back({
                        TokenType::SingleLineComment,
                        line_number,
                        pos-col_acc,
                        std::string_view(&input[pos], look_ahead_pos - pos)
                    });
                    pos = look_ahead_pos++;
                    break;
                }
                
                if (pos+1 <= input_length && input[pos+1] == '*') {
                    pos += 2;
                    look_ahead_pos = pos;
                    while(look_ahead_pos+1 <= input_length && input[look_ahead_pos] != '*' && input[look_ahead_pos+1] != '/') { 
                        if(input[look_ahead_pos] == '\n') { col_acc=pos; line_number++;}
                        look_ahead_pos++; 
                    }
                    tokens.push_back({
                        TokenType::MultiLineComment,
                        line_number,
                        pos-col_acc,
                        std::string_view(&input[pos], look_ahead_pos - pos)
                    });
                    pos = look_ahead_pos+=2;
                    break;
                }
                tokens.push_back({
                    TokenType::Unknown,
                    line_number,
                    pos-col_acc,
                    std::string_view(&input[pos], 1)
                });
                pos++;
                break;
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'h':
            case 'i':
            case 'j':
            case 'k':
            case 'l':
            case 'm':
            case 'n':
            case 'o':
            case 'p':
            case 'q':
            case 'r':
            case 's':
            case 't':
            case 'u':
            case 'v':
            case 'w':
            case 'x':
            case 'y':
            case 'z':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'I':
            case 'J':
            case 'K':
            case 'L':
            case 'M':
            case 'N':
            case 'O':
            case 'P':
            case 'Q':
            case 'R':
            case 'S':
            case 'T':
            case 'U':
            case 'V':
            case 'W':
            case 'X':
            case 'Y':
            case 'Z':
                look_ahead_pos = pos + 1;
                while (look_ahead_pos < input_length && is_identifier_char(input[look_ahead_pos])) {
                    look_ahead_pos++;
                }
                tokens.push_back({
                    is_keyword(std::string_view(&input[pos], look_ahead_pos - pos)) ? TokenType::Keyword : 
                    is_dataType(std::string_view(&input[pos], look_ahead_pos - pos)) ? TokenType::DataType : TokenType::Identifier, 
                    line_number, 
                    pos - col_acc, 
                    std::string_view(&input[pos], look_ahead_pos - pos)
                });
                pos = look_ahead_pos++;
                break;
            case '0':
                if (input_length >= pos+2 && input[pos+1] == 'x' || input[pos+1] == 'X') { // is Hex 
                    if(!is_decimal(input[pos+2])) { // Empty Hex 0x[NonDigit] return as Unknown 
                        tokens.push_back({
                            TokenType::Unknown,
                            line_number,
                            pos - col_acc,
                            std::string_view(&input[pos], look_ahead_pos - pos)
                        });
                    }
                    look_ahead_pos = pos + 2;
                    while (look_ahead_pos < input_length && is_hex_char(input[look_ahead_pos])) {
                        look_ahead_pos++;
                    }
                    tokens.push_back({
                        TokenType::Hexadecimal,
                        line_number,
                        pos - col_acc,
                        std::string_view(&input[pos], look_ahead_pos - pos)
                    });
                    pos = look_ahead_pos++;
                    break;                    
                }
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                look_ahead_pos = pos + 1;
                while (look_ahead_pos < input_length && is_decimal(input[look_ahead_pos])) {
                    look_ahead_pos++;
                }
                tokens.push_back({
                    TokenType::Number,
                    line_number,
                    pos - col_acc,
                    std::string_view(&input[pos], look_ahead_pos - pos)
                });
                pos = look_ahead_pos++;
                break; 
            case '@':
                tokens.push_back({TokenType::Annotation, line_number, pos - col_acc, std::string_view(&input[pos], 1)});
                pos++;
                break;
            default:
                tokens.push_back({TokenType::Unknown, line_number, pos - col_acc, std::string_view(&input[pos], 1)});
                pos++;
                break;
        }
    }
    tokens.push_back({TokenType::EndOfFile, line_number, pos - col_acc, std::string_view(&input[pos], 1)});
    return tokens;
}