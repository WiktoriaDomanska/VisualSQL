#pragma once
#include "SqlGenerator.hpp"
#include <sstream>

class MySqlGenerator : public ISqlGenerator {
public:
    std::string generateSchemaSql(const Schema &schema, const std::vector<Link> &links, const std::map<std::string, std::vector<SqlTypeDefinition>> &types) override;
    std::string getEngineName() const override {return "MySQL";};
};