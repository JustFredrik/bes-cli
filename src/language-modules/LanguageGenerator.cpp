#pragma once
#include "../ast.cpp"

struct GeneratedData
{
    std::string data;
    std::string languageId;
    std::string extension;
};

class LanguageGenerator
{

private:
    std::string id;
    std::string name;
    std::string fileExtension;

public:
    LanguageGenerator(std::string id, std::string name, std::string fileExtension)
        : id(id), name(name), fileExtension(fileExtension) {}

    virtual ~LanguageGenerator() = default;

    std::string getId() { return id; }
    std::string getName() { return name; }
    std::string getDefaultFileExtension() { return fileExtension; }

    // Generator method
    virtual GeneratedData generate(const AST &ast) = 0;
};