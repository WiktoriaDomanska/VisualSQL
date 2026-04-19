#pragma once
#include <string>
#include "GraphElements.hpp"

class Column {
public:
    Column(std::string name, std::string type, bool isPrimaryKey = false);

    std::string getName() const;
    std::string getType() const;

    void setName(std::string newName);
    void setType(std::string newType);

    Pin& getInputPin();
    Pin& getOutputPin();

    const Pin& getInputPin() const;
    const Pin& getOutputPin() const;

    bool getIsPrimaryKey() const;
    void setIsPrimaryKey(bool isPK);
private:
    std::string name;
    std::string type;
    bool isPrimaryKey;

    Pin inputPin;
    Pin outputPin;
};