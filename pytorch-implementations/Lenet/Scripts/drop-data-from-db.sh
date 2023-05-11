#!/bin/bash

# Arguments:
# $1 database name

database=$1

if [ -z "$1" ]
  then
    echo "Argument missing. Please specify the database name where you want to drop the data"
fi

#drop parameters data
mysql --host=${MYSQL_HOST_URL} --port=${MYSQL_PORT} --user=${MYSQL_ROOT_USER} --password=${MYSQL_ROOT_PASSWORD} ${database} -e "DELETE FROM gfp where CHANNEL BETWEEN 100000 AND 100009;"

#drop inputs data
mysql --host=${MYSQL_HOST_URL} --port=${MYSQL_PORT} --user=${MYSQL_ROOT_USER} --password=${MYSQL_ROOT_PASSWORD} ${database} -e "DELETE FROM gfp where CHANNEL BETWEEN 90000 AND 90049;"