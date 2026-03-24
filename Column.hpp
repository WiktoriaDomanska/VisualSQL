#pragma once
#include <string>

class Column {
public:
    Column(std::string name, std::string type);

    std::string getName() const;
    std::string getType() const;

    void setName(std::string newName);
    void setType(std::string newType);

private:
    std::string name;
    std::string type;
};