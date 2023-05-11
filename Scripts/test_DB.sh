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

$ROOT/compile.sh -O3 Programs/test_matrix_triple_lib || exit 1
./Scripts/run-online.sh Programs/test_matrix_triple_lib
printf "\n\n\n\n\n\n\n\n\n\n"
