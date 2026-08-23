#pragma once
#include <string>

const int INDENT_SIZE = 4; 

std::string indent(int n) {
    return std::string(INDENT_SIZE * n, ' ');
}
