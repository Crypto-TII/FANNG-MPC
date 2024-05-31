#!/bin/bash
# Portably find the directory containing this script.
HERE=$(cd `dirname $0`; pwd)
# Assuming it lives in Scripts/, set ROOT to the root directory of the
# project.
ROOT=$HERE/..

# Run test_matrix_lib in a non interactive way
$ROOT/compile.sh -O3 Programs/test_matrix_lib || exit 1
./Scripts/run-online-interactive-with-answers.sh "o i" Programs/test_matrix_lib
$ROOT/compile.sh -O3 Programs/test_IO_floats_and_negatives || exit 1
./Scripts/run-online-interactive-with-answers.sh "o i o i o i o i" Programs/test_IO_floats_and_negatives

#Run test_matrix_triple_lib without IO
sed -i 's/#define return_share_zero 1/#define return_share_zero 0/' src/config.h

$ROOT/compile.sh -O3 Programs/test_matrix_triple_lib || exit 1
./Scripts/run-online.sh Programs/test_matrix_triple_lib

sed -i 's/#define return_share_zero 0/#define return_share_zero 1/' src/config.h

# Run test_bounded_randomness_lib using full DB functionality.
sed -i 's/#define ignore_share_db_count 1/#define ignore_share_db_count 0/' src/config.h
sed -i 's/#define return_share_zero 1/#define return_share_zero 0/' src/config.h
sed -i 's/MySQL_insert_batch_size=100/MySQL_insert_batch_size=5000/' Data/StorageConfig-P*


$ROOT/compile.sh -O1 Programs/test_bounded_randomness_lib || exit 1
./Scripts/run-online.sh Programs/test_bounded_randomness_lib

sed -i 's/#define ignore_share_db_count 0/#define ignore_share_db_count 1/' src/config.h
sed -i 's/#define return_share_zero 0/#define return_share_zero 1/' src/config.h
sed -i 's/MySQL_insert_batch_size=5000/MySQL_insert_batch_size=100/' Data/StorageConfig-P*

# Run test_triple_lib using full DB functionality.
sed -i 's/#define ignore_triple_db_count 1/#define ignore_triple_db_count 0/' src/config.h
sed -i 's/#define return_triple_zero 1/#define return_triple_zero 0/' src/config.h
sed -i 's/MySQL_insert_batch_size=100/MySQL_insert_batch_size=5000/' Data/StorageConfig-P*

$ROOT/compile.sh -O1 Programs/test_triple_lib || exit 1
./Scripts/run-online.sh Programs/test_triple_lib

sed -i 's/#define ignore_triple_db_count 0/#define ignore_triple_db_count 1/' src/config.h
sed -i 's/#define return_triple_zero 0/#define return_triple_zero 1/' src/config.h
sed -i 's/MySQL_insert_batch_size=5000/MySQL_insert_batch_size=100/' Data/StorageConfig-P*


# Run test_dabit_lib using full DB functionality.
sed -i 's/#define ignore_dabit_db_count 1/#define ignore_dabit_db_count 0/' src/config.h
sed -i 's/#define return_dabit_zero 1/#define return_dabit_zero 0/' src/config.h
sed -i 's/MySQL_insert_batch_size=100/MySQL_insert_batch_size=1000/' Data/StorageConfig-P*

$ROOT/compile.sh -O1 Programs/test_dabit_lib || exit 1
./Scripts/run-online.sh Programs/test_dabit_lib

sed -i 's/#define ignore_dabit_db_count 0/#define ignore_dabit_db_count 1/' src/config.h
sed -i 's/#define return_dabit_zero 0/#define return_dabit_zero 1/' src/config.h
sed -i 's/MySQL_insert_batch_size=1000/MySQL_insert_batch_size=100/' Data/StorageConfig-P*

make clean
make progs -j

printf "\n\n\n\n\n\n\n\n\n\n"
