#pragma once
#include "Table.hpp"
#include <vector>
#include <string>

class Schema {
private:
    std::string name;
    std::vector<Table> tables;

public:
    Schema(std::string name);

    void addTable(const Table& table);

    const std::vector<Table>& getTables() const;
};