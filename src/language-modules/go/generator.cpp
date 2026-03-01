#pragma once
#include "../LanguageGenerator.cpp";

class GoGenerator : public LanguageGenerator {
public:
    GoGenerator() : LanguageGenerator("go", "Go (v1.22.5)", ".go") {}

    std::string generate(const AST& ast) override {
        return "// TODO implemented Go generator";
    }
};