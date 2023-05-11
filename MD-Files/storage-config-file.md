# Storage config file

A storage config file is required for each party. They are located in the **Data** directory and must follow this name pattern: `StorageConfig-P{player-number}.txt`.

It must contain the configuration parameters required to set up the desired storage type for Garbling Offline and Input Output (WIP) features.

The supported parameters are:
- **Storage_type**: _MySQLDatabase_ or _FileSystem_  
- **MySQL_url**
- **MySQL_user**
- **MySQL_password**
- **MySQL_database**
- **MySQL_insert_batch_size**
- **File_system_storage_directory**: The directory should exist already. If is not provided or is left empty, files will be written in the project root.   

The values must be written in separate lines. "MySQL_insert_batch_size" value is 750.

## Input/Output (WIP)
Input output storage type is meant to be selected with storage files in the future. 

If the selected Input/Output system is databases, make sure your storage config files have the following properties:
```
MySQL_url=tcp://127.0.0.1:3306
MySQL_user=root
MySQL_password=password
MySQL_database=mpclib-database-test
```


## Garbling Offline
Sample config file using MySQL database storage:
```
Storage_type=MySQLDatabase
MySQL_url=tcp://127.0.0.1:3306
MySQL_user=root
MySQL_password=password
MySQL_database=mpclib-database-test
```

Sample config file using file system storage:
```
Storage_type=FileSystem
File_system_storage_directory=Data
```
