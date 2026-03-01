#include <iostream>
#include <filesystem>
#include <sstream> 
#include <fstream>
#include <string>
#include "token.cpp"
#include "lexer.cpp"
#include "parser.cpp"
#include "generate.cpp"

namespace fs = std::filesystem;

void validate_schema(int argc, char **argv);
void generate(int argc, char **argv);
void init();

int main(int argc, char **argv) {
    init();

    if (std::strcmp(argv[1],"validate") == 0) {
        validate_schema(argc, argv);
    }
    if (std::strcmp(argv[1],"generate") == 0) {
        generate(argc, argv);
    }
}

void init() {
    populate_language_modules();
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

void print_tokens(std::vector<Token> tokens) {
        for (const auto& token : tokens) {
        std::cout << std::setw(18) << token.lexeme << " | " << std::setw(16) << token_type_to_string(token.type) << " (" << token.line_number << ": " << token.column_number << ")\n";
    }
    auto ast = parse(tokens);
    std::cout << ast;
}


void validate_schema(int argc, char **argv) {
    auto file_path = argv[2];
    std::string content = read_file(file_path);
    auto tokens = tokenize(content);
    auto ast = parse(tokens);
    std::cout << "Schema in '" << file_path << "' is valid.";
}

void generate(int argc, char **argv) {
    std::cout << "Code generation is not implemented yet.";
}
