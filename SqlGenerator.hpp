#pragma once
#include <string>
#include <vector>
#include <map>
#include "Schema.hpp"
#include "GraphElements.hpp"
#include "SqlTypes.hpp"

struct PinOwner {
    const Table* table = nullptr;
    const Column* column = nullptr;
};

inline PinOwner findPinOwner(const Schema& schema, ax::NodeEditor::PinId id) {
    for (const auto& table : schema.getTables()) {
        for (const auto& col : table.getColumns()) {
            if (col.getInputPin().ID == id || col.getOutputPin().ID == id) {
                return { &table, &col};
            }
        }
    }
    return { nullptr, nullptr};
}

class ISqlGenerator {
public:
    virtual ~ISqlGenerator() = default;

    virtual std::string generateSchemaSql(const Schema& schema,
                                          const std::vector<Link>& links,
                                          const std::map<std::string, std::vector<SqlTypeDefinition>>& types) = 0;
    virtual std::string getEngineName() const = 0;
};