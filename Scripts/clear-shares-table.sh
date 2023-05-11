#!/bin/bash

db_host_url=$1
db_port=$2
db_username=$3
db_password=$4
database=$5

if [ -z "$5" ]
  then
    echo "Argument missing. Please specify the following parameters:\n"
    echo "=======Arguments======="
    echo "1 - Host ip address"
    echo "2 - Host port"
    echo "3 - Database username for connection"
    echo "4 - Database password"
    echo "5 - Database name"
    exit 1
fi

mysql -h "$db_host_url" -P "$db_port" -u "$db_username" --password="$db_password" "$database" -e"
  DELETE FROM share;
  " -s
