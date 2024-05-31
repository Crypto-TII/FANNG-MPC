/*
Copyright (c) 2024, Technology Innovation Institute, Yas Island, Abu Dhabi, United Arab Emirates.
*/

#include "Storage_Config.h"
#include <iostream>

using namespace std;
using namespace boost::algorithm;

Storage_Config::Storage_Config(unsigned int player_number) {
    string filename = Storage_Config::CONFIG_FILE_BASE_NAME + to_string(player_number) + ".txt";
    ifstream config_file(filename);
    if (config_file.fail()) {
        throw file_error(filename);
    }

    string config_file_line;
    while (getline(config_file, config_file_line)) {
        unsigned long delimiter_position = config_file_line.find('=');
        string parameter_name = config_file_line.substr(0,delimiter_position);
        string parameter_value = config_file_line.substr(delimiter_position+1, config_file_line.size() - 1);
        trim(parameter_name);
        trim(parameter_value);
        set_parameter(parameter_name, parameter_value);
    }

    config_file.close();
}

void Storage_Config::set_parameter(const string &config_parameter_name, const string &value) {
    if (config_parameter_name == "Storage_type") {
        set_general_storage_type(value);
    } else if (config_parameter_name == "MySQL_url") {
        mysql_url = value;
    } else if (config_parameter_name == "MySQL_user") {
        mysql_user = value;
    } else if (config_parameter_name == "MySQL_password") {
        mysql_password = value;
    } else if (config_parameter_name == "MySQL_database") {
        mysql_database = value;
    } else if (config_parameter_name == "MySQL_insert_batch_size") {
        mysql_insert_batch_size = stoi(value);
    } else if (config_parameter_name == "File_system_storage_directory") {
        file_system_storage_directory = value;
    } else {
        throw Unknown_config_parameter(config_parameter_name);
    }
}

void Storage_Config::set_general_storage_type(const string &storage_type) {
    if (storage_type == "FileSystem") {
        general_storage_type = General_storage_type::File_System_Garbled_Circuit_Storage_Type;
    } else if (storage_type == "MySQLDatabase") {
        general_storage_type = General_storage_type::MySQL_Storage_Type;
    } else {
        throw Garbled_Circuit_Storage_Type_Not_Found(storage_type);
    }
}
