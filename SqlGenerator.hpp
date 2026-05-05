#pragma once
#include <string>
#include <vector>
#include <map>
#include "Schema.hpp"
#include"GraphElements.hpp"
#include "SqlTypes.hpp"

class SqlGenerator {
public:
    static std::string generateSchemaSql(const Schema& schema, const std::vector<Link>& links, const std::map<std::string, std::vector<SqlTypeDefinition>>& types);
};