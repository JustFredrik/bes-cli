#include <iostream>
#include <filesystem>
#include <sstream> 
#include <fstream>
#include <string>
#include "token.cpp"
#include "lexer.cpp"
#include "parser.cpp"

namespace fs = std::filesystem;

void validate_schema(int argc, char **argv, int& i);

int main(int argc, char **argv)
{
    for (int i = 0; i < argc; ++i)
    {
        printf("argv[%d]: %s\n", i, argv[i]);
    }
    for (int i = 1; i < argc; ++i)
    {
        validate_schema(argc, argv, i);
    }
}


std::string read_file(const std::string& path) {
    if(!fs::exists(path)) {
        throw std::runtime_error("cannot open file: No such file or directory");
    }

    if(!fs::is_regular_file(path)) {
        throw std::runtime_error("cannot open file: is not a file");
    }

    std::fstream file(path);

    if (!file.is_open()) {
        throw std::runtime_error("cannot open file: unknown error");
    }

    auto size = fs::file_size(path);
    std::string file_content(size, '\0');
    file.read(&file_content[0], size);
    return file_content;
}


void validate_schema(int argc, char **argv, int& i) {
    std::string content = read_file(argv[++i]);
    auto tokens = tokenize(content);
    parse(tokens);
    for (const auto& token : tokens) {
        std::cout << std::setw(18) << token.lexeme << " | " << std::setw(16) << token_type_to_string(token.type) << " (" << token.line_number << ": " << token.column_number << ")\n";
    }
}