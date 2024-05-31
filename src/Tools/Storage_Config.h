/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

#ifndef C_STORAGE_CONFIG_H
#define C_STORAGE_CONFIG_H

#include <string>
#include <fstream>
#include <Exceptions/Exceptions.h>
#include <GC/Garbled_Circuit_Exceptions.h>
#include "Tools_Exceptions.h"
#include <boost/algorithm/string.hpp>
#include <iostream>

enum class General_storage_type {
    MySQL_Storage_Type, File_System_Garbled_Circuit_Storage_Type
};

class Storage_Config {
private:

    const string CONFIG_FILE_BASE_NAME = "Data/StorageConfig-P";
    General_storage_type general_storage_type;
    string mysql_url;
    string mysql_user;
    string mysql_password;
    string mysql_database;
    string file_system_storage_directory;
    int mysql_insert_batch_size = 750;

    void set_parameter(const string &config_parameter_name, const string &value);

    void set_general_storage_type(const string &storage_type);

public:
    explicit Storage_Config(unsigned int player_number);

    General_storage_type get_general_storage_type() const {
        return general_storage_type;
    }

    string get_mysql_url() const{
        return mysql_url;
    };

    string get_mysql_user() const{
        return mysql_user;
    };

    string get_mysql_password() const{
        return mysql_password;
    };

    string get_mysql_database() const {
        return mysql_database;
    };

    int get_mysql_insert_batch_size() const {
        return mysql_insert_batch_size;
    };

    string get_file_system_storage_directory() const {
        return file_system_storage_directory;
    };

};

#endif //C_STORAGE_CONFIG_H
