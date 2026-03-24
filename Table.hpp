#pragma once
#include <string>
#include <vector>
#include "Column.hpp"

class Table {
public:
    Table(std::string name);

    void setName(std::string newName);
    void addColumn(const Column& column);

    std::string getName() const;

    const std::vector<Column>& getColumns() const;
    std::vector<Column>& getColumnsMutable();

private:
    std::string name;
    std::vector<Column> columns;
};