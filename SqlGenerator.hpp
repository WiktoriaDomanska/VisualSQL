#pragma once
#include <string>
#include <vector>
#include "Schema.hpp"
#include"GraphElements.hpp"

class SqlGenerator {
public:
    static std::string generateSchemaSql(const Schema& schema, const std::vector<Link>& links);
};