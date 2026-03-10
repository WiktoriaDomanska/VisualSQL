#pragma once
#include <string>
#include <vector>
#include "Column.hpp"

class Table {
public:
    Table(std::string name);

    void addColumn(const Column& column);

    std::string getName() const;

    const std::vector<Column>& getColumns() const;

private:
    std::string name;
    std::vector<Column> columns;
};