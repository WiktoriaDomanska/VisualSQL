#include "MySqlGenerator.hpp"
#include <sstream>

std::string MySqlGenerator::generateSchemaSql(const Schema &schema, const std::vector<Link>& links, const std::map<std::string, std::vector<SqlTypeDefinition>>& types) {
    std::stringstream ss;
    for (const auto& table : schema.getTables()) {
        ss << "CREATE TABLE `" << table.getName() << "` (\n";

        std::vector<std::string> foreignKeys;
        for (const auto& link : links) {
            PinOwner start = findPinOwner(schema, link.StartPinID);
            PinOwner end = findPinOwner(schema, link.EndPinID);

            if (start.table && end.table && start.column && end.column) {
                if (end.table -> getName() == table.getName()) {
                    std::string fk = "    FOREIGN KEY (`" + end.column -> getName() +
                        "`) REFERENCES `" + start.table -> getName() +
                             "`(`" + start.column -> getName() + "`)";
                    foreignKeys.push_back(fk);
                }

            }
        }

        const auto& columns = table.getColumns();
        for (size_t i = 0; i < columns.size(); ++i) {
            ss << "    `" << columns[i].getName() << "` " << columns[i].getType();

            bool requiresLength = false;
            for (const auto& [category, typesList] : types) {
                for (const auto& t : typesList) {
                    if (t.name == columns[i].getType()) {
                        requiresLength = t.hasLength;
                        break;
                    }
                }
            }

            if (requiresLength) {
                ss << "(" << columns[i].getLength() << ")";
            }

            if (columns[i].getIsPrimaryKey()) ss << " PRIMARY KEY";
            if (columns[i].getIsAutoIncrement()) ss << " AUTO_INCREMENT";
            if (columns[i].getIsUnique()) ss << " UNIQUE";
            if (columns[i].getIsNotNull()) ss << " NOT NULL";

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