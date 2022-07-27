// TestSQLite branch

#include <string>
#include <iostream>
#include <format>
#include "DataStructures.h"
#include "ConfigConstants.h"
#include "Serv.h"

#include "sqlite3.h"



int main() {

    sqlite3* DB_ptr;
    sqlite3_stmt* stmt_ptr;
    sqlite3_open(DBTestName.c_str(), &DB_ptr);
    char* err_text_buffer;
    int response = sqlite3_exec(DB_ptr, "CREATE TABLE IF NOT EXISTS TestTable(x INT, y TEXT,z DOUBLE);", nullptr, nullptr,&err_text_buffer);
    std::cout << std::format("\ncode of response {}\n", response);

    //switch (response)
    //{
    //case: SQLITE_OK

    //    default:
    //        break;
    //}
    return 0;
}
