#include <iostream>
#include "Schema.h" // Include your Schema class
#include "TableHeader.h"
#include "Table.h"

int main()
{
    std::string tableName;
    std::cout << "Enter the table name: ";
    std::cin >> tableName;

    Table table(tableName); // Allocating Table object on stack
    table.CreateTable();
    table.readTableHeader(tableName);

    // Now create and write the Schema
    Schema schema;
    TableHeader tableHeader;

    int numColumns;
    std::cout << "Enter number of columns: ";
    std::cin >> numColumns;
    std::cin.ignore(); // Clear the buffer

    for (int i = 0; i < numColumns; ++i)
    {
        std::string columnName;
        int columnTypeInt;

        std::cout << "Enter name for column " << (i + 1) << ": ";
        std::getline(std::cin, columnName);

        std::cout << "Enter type for column " << (i + 1) << " (1 = INT, 2 = TEXT, 3 = FLOAT): ";
        std::cin >> columnTypeInt;
        std::cin.ignore(); // Clear the buffer

        ColumnType columnType = static_cast<ColumnType>(columnTypeInt);
        schema.addColumn(columnName, columnType);
    }

   std::ofstream outFile("customdatabase/" + tableName + ".tbl", std::ios::binary | std::ios::out);
    if (!outFile.is_open())
    {
        std::cerr << "Failed to open file for writing schema!\n";
        return 1;
    }

    // Move file pointer to the schemaOffset
    outFile.seekp(table.getSchemaOffset(), std::ios::beg);

    // Write the schema into the file
    schema.serialize(outFile);

    // Close the current output file
    outFile.close();

    schema.print();

    std::cout << "Schema added successfully to table '" << tableName << "'!\n";

    return 0;
}
