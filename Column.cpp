#include "Column.hpp"

Column::Column(std::string name, std::string type)
    : name(std::move(name)), type(std::move(type)) {

}

std::string Column::getName() const {
    return name;
}

std::string Column::getType() const {
    return type;
}