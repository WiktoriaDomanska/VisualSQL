#pragma once
#include "Schema.hpp"
#include "GraphElements.hpp"
#include <string>
#include <vector>

class ProjectSerializer {
    public:
    static bool saveToFile(const Schema& schema, const std::vector<Link>& links, const std::string& filepath);
    static bool loadFromFile(Schema& schema, std::vector<Link>& links, const std::string& filepath, int& nextTableId);
};