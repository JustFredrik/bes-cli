#pragma once
#include "../LanguageGenerator.cpp";

class GmlGenerator : public LanguageGenerator {
public:
    GmlGenerator() : LanguageGenerator("gml", "GameMaker (v2024.14.3)", ".gml") {}

    std::string generate(const AST& ast) override {
        return "// TODO implemented GML generator";
    }
};