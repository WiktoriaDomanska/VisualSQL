#pragma once
#include "Schema.hpp"
#include <string>

class ProjectSerializer {
    public:
    static bool saveToFile(const Schema& schema, const std::string& filepath);
    static bool loadFromFile(Schema& schema, const std::string& filepath);
};