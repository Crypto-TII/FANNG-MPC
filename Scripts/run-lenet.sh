#!/bin/bash
number_of_arguments="$#"
mpclib_path=$1
lenet_path_relative_to_mpclib_path=$2

if [[ $number_of_arguments -lt 2 ]]
then
  echo "#######################################################################################"
  echo "###                                                                                 ###"
  echo "### The necessary number of arguments are 2:                                        ###"
  echo "### FANNG-MPC folder path and Lenet folder path relative to FANNG-MPC folder        ###"
  echo "### EXAMPLE:                                                                        ###"
  echo "### ./run-lenet.sh FANNG-MPC ../test_obliv_nn_lenet                                 ###"
  echo "###                                                                                 ###"
  echo "#######################################################################################"

  exit;
fi

current_script_dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
executable_file="${current_script_dir}/generate-lenet-answers-for-io-fs.sh"
answers="$($executable_file)"

( # It opens a subshell
  cd "$mpclib_path" || exit
  ./Scripts/run-online-interactive-with-answers.sh "$answers" "$lenet_path_relative_to_mpclib_path"
)
