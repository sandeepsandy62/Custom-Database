#ifndef SCHEMA_H
#define SCHEMA_H

#include <cstdint>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

enum class ColumnType : uint8_t {
    INT = 1,
    TEXT = 2,
    FLOAT = 3,
};

class Column {
private:
    std::string name;
    ColumnType type;

public:
    Column() = default;
    Column(const std::string& name, ColumnType type) : name(name), type(type) {}

    // Serialize Column
    void serialize(std::ofstream& out) const {
        uint8_t nameLength = name.size();
        out.write(reinterpret_cast<const char*>(&nameLength), sizeof(nameLength));
        out.write(name.c_str(), name.size());
        out.write(reinterpret_cast<const char*>(&type), sizeof(type));
    }

    // Deserialize Column
    bool deserialize(std::ifstream& in) {
        uint8_t nameLength;
        if (!(in.read(reinterpret_cast<char*>(&nameLength), sizeof(nameLength)))) return false;
        name.resize(nameLength);
        if (!(in.read(&name[0], nameLength))) return false;
        if (!(in.read(reinterpret_cast<char*>(&type), sizeof(type)))) return false;
        return true;
    }

    const std::string& getName() const { return name; }
    ColumnType getType() const { return type; }

    void print() const {
        std::cout << "Column Name: " << name << ", Type: " << static_cast<int>(type) << "\n";
    }
};

class Schema {
private:
    std::vector<Column> columns;

public:
    Schema() = default;

    void addColumn(const std::string& name, ColumnType type) {
        columns.emplace_back(name, type);
    }

    void serialize(std::ofstream& out) const {
        uint32_t numColumns = columns.size();
        out.write(reinterpret_cast<const char*>(&numColumns), sizeof(numColumns));
        for (const auto& column : columns) {
            column.serialize(out);
        }
    }

    bool deserialize(std::ifstream& in) {
        uint32_t numColumns;
        if (!(in.read(reinterpret_cast<char*>(&numColumns), sizeof(numColumns)))) return false;
        columns.resize(numColumns);
        for (auto& column : columns) {
            if (!column.deserialize(in)) return false;
        }
        return true;
    }

    void print() const {
        for (const auto& column : columns) {
            column.print();
        }
    }

    const std::vector<Column>& getColumns() const {
        return columns;
    }
};

#endif 
