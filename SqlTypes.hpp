#pragma once
#include <string>

struct SqlTypeDefinition {
    std::string name;
    std::string category;
    bool hasLength;
};