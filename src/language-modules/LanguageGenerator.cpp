#pragma once
#include "../ast.cpp";

class LanguageGenerator {

private:
    std::string id;
    std::string name;
    std::string fileExtension;

public:
    LanguageGenerator(std:: string id, std::string name, std::string fileExtension)
        : id(id), name(name), fileExtension(fileExtension) {}

    virtual ~LanguageGenerator() = default;

    std::string getId() { return id; }
    std::string getName() { return name; }
    std::string getDefaultFileExtension() { return fileExtension; }

    // Generator method
    virtual std::string generate(const AST& ast) = 0;
};