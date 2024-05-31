/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

#include "MySQL_Controller.h"

MySQL_Controller::MySQL_Controller(const string &database_url, const string &database_user,
                                   const string &database_password, const string &database_name) {
    sql::Driver *driver;
    sql::ConnectOptionsMap options;
    options["CLIENT_MULTI_STATEMENTS"] = true;
    options["hostName"] = database_url;
    options["userName"] = database_user;
    options["password"] = database_password;
    driver = get_driver_instance();
    connection = shared_ptr<sql::Connection>(driver->connect(options));
    connection->setSchema(database_name);
}