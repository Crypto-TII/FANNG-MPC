#!/bin/bash

# Arguments:
# $1 database name

database=$1

if [ -z "$1" ]
  then
    echo "Argument missing. Please specify the database name where you want to insert the data"
    exit 1
fi

./Lenet/Scripts/insert-parameters.sh ./LenetData/Parameters/Default-parameters $MYSQL_HOST_URL $MYSQL_PORT $MYSQL_ROOT_USER $MYSQL_ROOT_PASSWORD 0 $database
./Lenet/Scripts/insert-gfp-from-file.sh ./LenetData/Inputs/Execution_0/P0-feature_extractor_0.input.txt $MYSQL_HOST_URL $MYSQL_PORT $MYSQL_ROOT_USER $MYSQL_ROOT_PASSWORD 0 $database 90000
./Lenet/Scripts/insert-gfp-from-file.sh ./LenetData/Inputs/Execution_1/P0-feature_extractor_0.input.txt $MYSQL_HOST_URL $MYSQL_PORT $MYSQL_ROOT_USER $MYSQL_ROOT_PASSWORD 0 $database 90001
./Lenet/Scripts/insert-gfp-from-file.sh ./LenetData/Inputs/Execution_2/P0-feature_extractor_0.input.txt $MYSQL_HOST_URL $MYSQL_PORT $MYSQL_ROOT_USER $MYSQL_ROOT_PASSWORD 0 $database 90002
