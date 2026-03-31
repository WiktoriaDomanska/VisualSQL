#pragma once
#include <string>
#include "GraphElements.hpp"

class Column {
public:
    Column(std::string name, std::string type);

    std::string getName() const;
    std::string getType() const;

    void setName(std::string newName);
    void setType(std::string newType);

    Pin& getInputPin();
    Pin& getOutputPin();

private:
    std::string name;
    std::string type;

    Pin inputPin;
    Pin outputPin;
};