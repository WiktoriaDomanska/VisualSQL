#include "Schema.hpp"

Schema::Schema(std::string name) {
    this->name = name;
}

void Schema::addTable(const Table &table) {
    tables.push_back(table);
}

const std::vector<Table>& Schema::getTables() const {
    return tables;
}