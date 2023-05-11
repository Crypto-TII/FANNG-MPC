#!/bin/bash
number_of_arguments="$#"
mpclib_path=$1
pruned_resnet_path_relative_to_mpclib_path=$2

if [[ $number_of_arguments -lt 2 ]]
then
  echo "########################################################################################"
  echo "###                                                                                  ###"
  echo "### The necessary number of arguments are 2:                                         ###"
  echo "### FANNG-MPC folder path and Pruned Resnet folder path relative to FANNG-MPC folder ###"
  echo "### EXAMPLE:                                                                         ###"
  echo "### ./run-pruned-resnet.sh FANNG-MPC ../test_obliv_nn_pruned_resnet                  ###"
  echo "###                                                                                  ###"
  echo "########################################################################################"

  exit;
fi

current_script_dir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
executable_file="${current_script_dir}/generate-pruned-resnet-answers-for-io-fs.sh"
answers="$($executable_file)"

( # It opens a subshell
  cd "$mpclib_path" || exit
  ./Scripts/run-online-interactive-with-answers.sh "$answers" "$pruned_resnet_path_relative_to_mpclib_path"
)
