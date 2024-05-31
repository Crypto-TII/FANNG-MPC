/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

#ifndef MYSQL_CONTROLLER_H
#define MYSQL_CONTROLLER_H

#include <Exceptions/Exceptions.h>
#include <Tools/Storage_Config.h>
#include <cppconn/driver.h>
#include <fstream>
#include <string>

class MySQL_Controller {
public:
    shared_ptr<sql::Connection> connection;

    /* Prevent use of these */
    MySQL_Controller(const MySQL_Controller &) = delete;
    void operator=(MySQL_Controller &) = delete;

    MySQL_Controller(const string &database_url,
                     const string &database_user,
                     const string &database_password,
                     const string &database_name);

    ~MySQL_Controller(){
        connection->close();
    };
};

#endif //MYSQL_CONTROLLER_H
