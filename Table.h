// include guard or header guard
#ifndef Table_H
#define Table_H

#include <iostream>
#include <cstdlib>
#include <fstream>
#include "TableHeader.h"

class Table
{
private:
    std::string tableName;
    TableHeader header;

public:
    Table()
    {
        tableName = "";
    }
    Table(std::string tableName)
    {
        this->tableName = "customdatabase/" + tableName + ".tbl";
    };

    uint32_t getSchemaOffset() const
    {
        return header.getSchemaOffset();
    }

    void CreateTable()
    {
        int acknowledge_custom_db_create = system("mkdir -p customdatabase"); // works as if we are executing this command in cmd and create customdatabase folder if wont exists only

        // Log database folder creation sucessfull or not

        if (acknowledge_custom_db_create == 0)
        {
            std ::cout << "---Custom Database Created Successfully---" << std ::endl;
        }
        else
        {
            std ::cout << "Return Code : " << acknowledge_custom_db_create << "\n"
                       << "---Error in Custom Database Creation---" << std ::endl;
        }

        // create a file and open in binary mode
        std ::ofstream file(tableName, std::ios::binary);
        if (!file.is_open())
        {
            /*
            cerr is a standard output stream object used to output errors.
            It is an unbuffered stream, meaning that output is written immediately to the standard error stream (stderr), without any intermediate buffering.
            This ensures that error messages are displayed as soon as they are generated, which is crucial for debugging and error handling.
            */
            std::cerr << "Failed to create table file: " << tableName << "\n";
            return;
        }

        TableHeader header;
        header.serialize(file);

        file.close();

        std::cout << "Table '" << tableName << "' created successfully as " << tableName << "\n";
    }

    void readTableHeader(const std::string &tableName)
    {
        std::string filename = "customdatabase/" + tableName + ".tbl";

        std::ifstream file(filename, std::ios::binary);
        if (!file.is_open())
        {
            std::cerr << "Failed to open table file: " << filename << "\n";
            return;
        }

        if (header.deserialize(file))
        {
            std::cout << "Table Header for '" << tableName << "':\n";
            header.print();
        }

        file.close();
    }
};

#endif
