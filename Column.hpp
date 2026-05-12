#pragma once
#include <string>
#include "GraphElements.hpp"

class Column {
public:
    Column(std::string name, std::string type, bool isPrimaryKey = false);

    std::string getName() const;
    std::string getType() const;

    int getLength() const;
    void setLength(int newLength);

    void setName(std::string newName);
    void setType(std::string newType);

    Pin& getInputPin();
    Pin& getOutputPin();

    const Pin& getInputPin() const;
    const Pin& getOutputPin() const;

    bool getIsPrimaryKey() const;
    void setIsPrimaryKey(bool isPK);

    bool getIsNotNull() const;
    void setIsNotNull(bool isNN);

    bool getIsUnique() const;
    void setIsUnique(bool isUQ);

    bool getIsAutoIncrement() const;
    void setIsAutoIncrement(bool isAI);

private:
    std::string name;
    std::string type;
    int length;
    bool isPrimaryKey;
    bool isNotNull = false;
    bool isUnique = false;
    bool isAutoIncrement = false;

    Pin inputPin;
    Pin outputPin;
};