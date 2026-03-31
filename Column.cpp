#include "Column.hpp"

static int nextPinId = 100;

Column::Column(std::string name, std::string type)
    : name(std::move(name)), type(std::move(type)),
inputPin(nextPinId++, PinType::Input),
outputPin(nextPinId++, PinType::Output)
{
}

std::string Column::getName() const {
    return name;
}

std::string Column::getType() const {
    return type;
}

void Column::setName(std::string newName) {
    this->name = newName;
}

void Column::setType(std::string newType) {
    this->type = newType;
}

Pin& Column::getInputPin() {
    return inputPin;
}

Pin& Column::getOutputPin() {
    return outputPin;
}