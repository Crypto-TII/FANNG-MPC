/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

//
// Created by Ivan on 14/11/2021.
//

#ifndef C_DATABASE_H
#define C_DATABASE_H

#include <stdlib.h>
#include <string>
#include <array>


#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include "LSSS/Share.h"
#include "Utils/String_Utils.h"
#include "Share_DTO.h"
#include "GFP_DTO.h"
#include "Reg_int_DTO.h"
#include "IO_exceptions.h"

using namespace std;

class Database {
private:
    sql::Driver *driver;
    sql::Connection *connection;

    void save(int player_number, int channel, const string &value, const string &query);
    sql::ResultSet* read(int player_number, int channel, int last_retrieved_index, const string &query);
    void clear_data(const string &tableName, int channel, int player_number);

public:
    Database();

    Database(const string &connection_url, const string &database_user, const string &database_password,
             const string &database_schema);

    void save_gfp(int player_number, int channel, const string &value);

    void save_regint(int player_number, int channel, const string &value);

    void save_cint(int player_number, int channel, const string &value);

    void save_share(int channel, const Share &share);

    Share_DTO read_share(int player_number, int channel, int last_retrieved_index);

    Reg_int_DTO read_reg_int(int player_number, int channel, int last_retrieved_index);

    GFP_DTO read_gfp(int player_number, int channel, int last_retrieved_index);

    GFP_DTO read_cint(int player_number, int channel, int last_retrieved_index);

    void clear_data(int channel, int player_number);

    void close();
};

#endif //C_DATABASE_H
