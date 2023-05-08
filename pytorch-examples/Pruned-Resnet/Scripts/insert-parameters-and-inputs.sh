#!/bin/bash

# Arguments:
# $1 database name

database=$1

if [ -z "$1" ]
  then
    echo "Argument missing. Please specify the database name where you want to insert the data"
    exit 1
fi

./Scripts/insert-parameters.sh ./Pruned_Resnet_CNN/inference/PrunedResnetData/all_parameters/bias $MYSQL_HOST_URL $MYSQL_PORT $MYSQL_ROOT_USER $MYSQL_ROOT_PASSWORD 0 $database 100020
./Scripts/insert-parameters.sh ./Pruned_Resnet_CNN/inference/PrunedResnetData/all_parameters/new_beta $MYSQL_HOST_URL $MYSQL_PORT $MYSQL_ROOT_USER $MYSQL_ROOT_PASSWORD 0 $database 100028
./Scripts/insert-parameters.sh ./Pruned_Resnet_CNN/inference/PrunedResnetData/all_parameters/new_gamma $MYSQL_HOST_URL $MYSQL_PORT $MYSQL_ROOT_USER $MYSQL_ROOT_PASSWORD 0 $database 100034
./Scripts/insert-parameters.sh ./Pruned_Resnet_CNN/inference/PrunedResnetData/all_parameters/weights $MYSQL_HOST_URL $MYSQL_PORT $MYSQL_ROOT_USER $MYSQL_ROOT_PASSWORD 0 $database 100040

./Scripts/insert-gfp-from-file.sh ./Pruned_Resnet_CNN/inference/PrunedResnetData/Inputs/Execution_0/P0-Conv_layer_1.input.txt $MYSQL_HOST_URL $MYSQL_PORT $MYSQL_ROOT_USER $MYSQL_ROOT_PASSWORD 0 $database 90050
