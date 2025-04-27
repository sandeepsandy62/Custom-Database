/*
    constant member function
    reinterpret_cast<const char*>(&version): This is a C++ type cast.
    It treats the memory location of the version variable as if it were a pointer to a constant character (const char*).
    This is necessary because the out.write() function expects a pointer to a character buffer (const char*) as its first argument when writing raw bytes.
    We are essentially telling the compiler to treat the byte representing the version as a character for the purpose of writing it.

    we completed filling 17 bytes
    Padding to complete 4096 bytes
    4096 - 17 = 4079
*/

#ifndef TABLEHEADER_H
#define TABLEHEADER_H

#include <cstdint>
#include <iostream>
#include <fstream>
#include <cstring>

class TableHeader {
private:
    char magic[4] = {'M', 'Y', 'T', 'B'}; // 4 Bytes
    uint8_t version = 1;                  // 1 Byte
    uint32_t pageSize = 4096;              // 4 Bytes
    uint32_t numRecords = 0;               // 4 Bytes
    uint32_t schemaOffset = 4096;          // 4 Bytes

public:
    TableHeader() = default;

    // --- Serialization ---
    void serialize(std::ofstream& out) const {
        out.write(magic, sizeof(magic));
        out.write(reinterpret_cast<const char*>(&version), sizeof(version));
        out.write(reinterpret_cast<const char*>(&pageSize), sizeof(pageSize));
        out.write(reinterpret_cast<const char*>(&numRecords), sizeof(numRecords));
        out.write(reinterpret_cast<const char*>(&schemaOffset), sizeof(schemaOffset));

        // Fill the rest of 4096 bytes with padding
        char padding[4079] = {0};
        out.write(padding, sizeof(padding));
    }

    // --- Deserialization ---
    bool deserialize(std::ifstream& in) {
        in.read(magic, sizeof(magic));
        in.read(reinterpret_cast<char*>(&version), sizeof(version));
        in.read(reinterpret_cast<char*>(&pageSize), sizeof(pageSize));
        in.read(reinterpret_cast<char*>(&numRecords), sizeof(numRecords));
        in.read(reinterpret_cast<char*>(&schemaOffset), sizeof(schemaOffset));

        if (std::strncmp(magic, "MYTB", 4) != 0) {
            std::cerr << "Invalid table file (magic mismatch)\n";
            return false;
        }
        return true;
    }

    // --- Print header ---
    void print() const {
        std::cout << "Magic: " << std::string(magic, 4) << "\n";
        std::cout << "Version: " << static_cast<int>(version) << "\n";
        std::cout << "Page Size: " << pageSize << "\n";
        std::cout << "Number of Records: " << numRecords << "\n";
        std::cout << "Schema Offset: " << schemaOffset << "\n";
    }

    // --- Getters ---
    uint32_t getPageSize() const { return pageSize; }
    uint32_t getNumRecords() const { return numRecords; }
    uint32_t getSchemaOffset() const { return schemaOffset; }
    uint8_t getVersion() const { return version; }

    // --- Setters ---
    void setNumRecords(uint32_t num) { numRecords = num; }
    void setSchemaOffset(uint32_t offset) { schemaOffset = offset; }
};

#endif // TABLEHEADER_H
