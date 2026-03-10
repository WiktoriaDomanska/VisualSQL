#include "Table.hpp"

Table::Table(std::string name) : name(std::move(name)) {

}

void Table::addColumn(const Column& column) {
    columns.push_back(column);
}

std::string Table::getName() const {
    return name;
}

const std::vector<Column>& Table::getColumns() const {
    return columns;
}