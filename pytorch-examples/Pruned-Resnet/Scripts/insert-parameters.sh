#!/bin/bash

# Arguments:
# $1 path of parameters directory
# $2 host ip address
# $3 host port
# $4 database username for connection
# $5 player number
# $6 database name

parameters_path=$1
db_host_url=$2
db_port=$3
db_username=$4
db_password=$5
player=$6
database=$7
starting_channel=$8

if [ -z "$8" ]
  then
    echo "Argument missing. Please specify the following parameters:\n"
    echo "=======Arguments======="
    echo "1 - Path of parameters directory"
    echo "2 - Host ip address"
    echo "3 - Host port"
    echo "4 - Database username for connection"
    echo "5 - Database user password"
    echo "6 - Player number"
    echo "7 - Database name"
    echo "8 - starting channel"
    exit 1
fi

formatted_data_filepath=formatted_data.txt
channel=$starting_channel

for filename in $parameters_path/P0-*; do
 echo channel: $channel "-->" $filename
  for value in `cat $filename`; do
    echo "$channel $value $player" >> $formatted_data_filepath
  done
 channel=$(($channel+1))
done

mysql --local-infile=1 --host=${db_host_url} --port=${db_port} --user=${db_username} --password=${db_password} ${database} -e "
  LOAD DATA LOCAL INFILE 'formatted_data.txt'
  INTO TABLE gfp
  FIELDS TERMINATED BY ' '
  LINES TERMINATED BY '\n'
  (CHANNEL,VALUE,PLAYER);"

rm "$formatted_data_filepath"
