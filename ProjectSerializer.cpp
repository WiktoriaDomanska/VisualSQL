#include "ProjectSerializer.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;

bool ProjectSerializer::saveToFile(const Schema &schema, const std::vector<Link>& links, const std::string &filepath) {
    json j;
    j["tables"] = json::array();

    for (const auto& table : schema.getTables()) {
        json jTable;
        jTable["name"] = table.getName();
        jTable["columns"] = json::array();

        for (const auto& col : table.getColumns()) {
            json jCol;
            jCol["name"] = col.getName();
            jCol["type"] = col.getType();
            jCol["isPrimaryKey"] = col.getIsPrimaryKey();

            jTable["columns"].push_back(jCol);
        }

        j["tables"].push_back(jTable);
    }

    j["links"] = json::array();
    for (const auto& link : links) {
        json jLink;

        jLink["id"] = (unsigned long long)link.ID.Get();
        jLink["startPin"] = (unsigned long long)link.StartPinID.Get();
        jLink["endPin"] = (unsigned long long)link.EndPinID.Get();
        j["links"].push_back(jLink);
    }

    std::ofstream file(filepath);
    if (file.is_open()) {
        file << j.dump(4);
        file.close();
        return true;
    } else {
        std::cerr << "Blad: Nie udalo sie otworzyc pliku do zapisu." << filepath << std::endl;
        return false;
    }
}

bool ProjectSerializer::loadFromFile(Schema &schema, std::vector<Link>& links, const std::string& filepath, int& nextTableId) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Blad: Nie udalo sie otworzyc pliku" << filepath << std::endl;
        return false;
    }

    json j;
    file >> j;
    file.close();

    schema.getTablesMutable().clear();
    links.clear();

    if (j.contains("tables") && j["tables"].is_array()) {

        for (const auto& jTable : j["tables"]) {
            Table table(nextTableId++, jTable["name"].get<std::string>());

            if (jTable.contains("columns") && jTable["columns"].is_array()) {
                for (const auto& jCol : jTable["columns"]) {
                    Column col(
                        jCol["name"].get<std::string>(),
                        jCol["type"].get<std::string>(),
                        jCol["isPrimaryKey"].get<bool>()
                        );

                    table.addColumn(col);
                }
            }

            schema.addTable(table);
        }
    }

    if (j.contains("links") && j["links"].is_array()) {
        for (const auto& jLink : j["links"]) {
            int id = jLink["id"].get<int>();
            int start = jLink["startPin"].get<int>();
            int end = jLink["endPin"].get<int>();

            links.push_back(Link(id, start, end));
        }
    }

    return true;
}