#pragma once
#include <string>

class Column {
public:
    Column(std::string name, std::string type);

    std::string getName() const;
    std::string getType() const;

private:
    std::string name;
    std::string type;
};