#pragma once
#include <vector>
#include <string>
#include <map>
#include "ast.cpp"
#include "./language-modules/LanguageGenerator.cpp"
#include "./language-modules/gml/generator.cpp"
#include "./language-modules/go/generator.cpp"

std::map<std::string, std::unique_ptr<LanguageGenerator>> modules;

void populate_language_modules()
{
    modules["go"] = std::make_unique<GoGenerator>();
    modules["gml"] = std::make_unique<GmlGenerator>();
}

std::vector<GeneratedData> generate(const AST &ast, std::vector<std::string> languageFlags)
{
    std::vector<GeneratedData> data;
    for (const auto &flag : languageFlags)
    {
        if (modules.find(flag) != modules.end())
        {
            data.push_back(modules[flag]->generate(ast));
        }
    }
    return data;
}