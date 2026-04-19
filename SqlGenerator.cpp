#include "SqlGenerator.hpp"
#include <sstream>
#include <algorithm>

struct PinOwner {
    const Table* table = nullptr;
    const Column* column = nullptr;
};

PinOwner findPinOwner(const Schema& schema, ax::NodeEditor::PinId id) {
    for (const auto& table : schema.getTables()) {
        for (const auto& col : table.getColumns()) {
            if (col.getInputPin().ID == id || col.getOutputPin().ID == id) {
                return { &table, &col};
            }
        }
    }
    return { nullptr, nullptr};
}

std::string SqlGenerator::generateSchemaSql(const Schema &schema, const std::vector<Link>& links) {
    std::stringstream ss;

    for (const auto& table : schema.getTables()) {
        ss << "CREATE TABLE " << table.getName() << " (\n";

        std::vector<std::string> foreignKeys;
        for (const auto& link : links) {
            PinOwner start = findPinOwner(schema, link.StartPinID);
            PinOwner end = findPinOwner(schema, link.EndPinID);

            if (start.table && end.table && start.column && end.column) {
                if (end.table -> getName() == table.getName()) {
                    std::string fk = "    FOREIGN KEY (" + end.column -> getName() +
                        ") REFERENCES " + start.table -> getName() +
                            "(" + start.column -> getName() + ")";
                    foreignKeys.push_back(fk);
                }
            }
        }

        const auto& columns = table.getColumns();
        for (size_t i = 0; i < columns.size(); ++i) {
            ss << "    " << columns[i].getName() << " " << columns[i].getType();

            if (columns[i].getIsPrimaryKey()) {
                ss << " PRIMARY KEY";
            }

            bool isLastColumn = (i == columns.size() - 1);
            if (!isLastColumn || !foreignKeys.empty()) {
                ss << ",";
            }
            ss << "\n";
        }

        for (size_t i = 0; i < foreignKeys.size(); ++i) {
            ss << foreignKeys[i];

            if (i < foreignKeys.size() - 1) {
                ss << ",";
            }
            ss << "\n";
        }

        ss << ");\n\n";
    }
    return ss.str();
}