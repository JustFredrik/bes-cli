#pragma once
#include "../LanguageGenerator.cpp"

class GoGenerator : public LanguageGenerator {
public:
    GoGenerator() : LanguageGenerator("go", "Go (v1.22.5)", ".go") {}

    GeneratedData generate(const AST& ast) override {
        return {
            "// Go code generation is not yet implemented.",
            getId(),
            getDefaultFileExtension()
        };
    }
};