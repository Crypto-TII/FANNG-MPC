#!/bin/bash

# Arguments:
# $1 Running_mode
# Running_mode = 0 means you will use run-online-interactive-with-answers.sh script to run your program
# Running_mode = 1 means you will use Player.x executable to run your program

#echo "############################################################"
#echo "###                        HELP                          ###"
#echo "###  to generate answers to run Lenet using              ###"
#echo "###  run-online-interactive-with-answers.sh script run:  ###"
#echo "###      ./generate-lenet-answers-for-io-fs.sh 0         ###"
#echo "###                                                      ###"
#echo "###                                                      ###"
#echo "###  to generate answers to run Lenet using              ###"
#echo "###  Player.x executable run:                            ###"
#echo "###      ./generate-lenet-answers-for-io-fs.sh 1         ###"
#echo "###                                                      ###"
#echo "############################################################"

if [[ $# -gt 0 ]]
  then
    running_mode=$1
  else
    running_mode=0
  fi

parameters_base_path="LenetData/Parameters/Default-parameters"
answer_for_100005_channel="${parameters_base_path}/P0-feature_extractor.0.weight.txt"
answer_for_100007_channel="${parameters_base_path}/P0-feature_extractor.3.weight.txt"
answer_for_100009_channel="${parameters_base_path}/P0-feature_extractor.6.weight.txt"
answer_for_100001_channel="${parameters_base_path}/P0-classifier.0.weight.txt"
answer_for_100003_channel="${parameters_base_path}/P0-classifier.2.weight.txt"

answer_for_100004_channel="${parameters_base_path}/P0-feature_extractor.0.bias.txt"
answer_for_100006_channel="${parameters_base_path}/P0-feature_extractor.3.bias.txt"
answer_for_100008_channel="${parameters_base_path}/P0-feature_extractor.6.bias.txt"
answer_for_100000_channel="${parameters_base_path}/P0-classifier.0.bias.txt"
answer_for_100002_channel="${parameters_base_path}/P0-classifier.2.bias.txt"

inputs_base_path="LenetData/Inputs"
answer_for_90000_channel="${inputs_base_path}/Execution_0/P0-feature_extractor_0.input.txt"
answer_for_90001_channel="${inputs_base_path}/Execution_1/P0-feature_extractor_0.input.txt"
answer_for_90002_channel="${inputs_base_path}/Execution_2/P0-feature_extractor_0.input.txt"

# Put in this array the filepaths in the correct question order
filepath_answers=(
  "$answer_for_100005_channel"
  "$answer_for_100007_channel"
  "$answer_for_100009_channel"
  "$answer_for_100001_channel"
  "$answer_for_100003_channel"
  "$answer_for_100004_channel"
  "$answer_for_100006_channel"
  "$answer_for_100008_channel"
  "$answer_for_100000_channel"
  "$answer_for_100002_channel"
  "$answer_for_90000_channel"
  "$answer_for_90001_channel"
  "$answer_for_90002_channel"
)

answers=""
for filepath_answer in "${filepath_answers[@]}"
  do
    if [[ $running_mode -eq 0 ]]
    then
      answers+="i ${filepath_answer//P0-/} " # Remove P0- from original filepaths because it will be added again when we use run-online-interactive-with-answers.sh script
    else
      answers+="i ${filepath_answer} "
    fi
  done

echo "$answers"