#!/bin/bash

# Arguments:
# $1 Running_mode
# Running_mode = 0 means you will use run-online-interactive-with-answers.sh script to run your program
# Running_mode = 1 means you will use Player.x executable to run your program

#echo "############################################################"
#echo "###                        HELP                          ###"
#echo "###  to generate answers to run Pruned Resnet using      ###"
#echo "###  run-online-interactive-with-answers.sh script run:  ###"
#echo "###   ./generate-pruned-resnet-answers-fo-io-fs.sh 0     ###"
#echo "###                                                      ###"
#echo "###                                                      ###"
#echo "###  to generate answers to run Pruned Resnet using      ###"
#echo "###  Player.x executable run:                            ###"
#echo "###   ./generate-pruned-resnet-answers-fo-io-fs.sh 1     ###"
#echo "###                                                      ###"
#echo "############################################################"


if [[ $# -gt 0 ]]
  then
    running_mode=$1
  else
    running_mode=0
  fi

inputs_base_path="PrunedResnetData/Inputs"
answer_for_90050_channel="${inputs_base_path}/Execution_0/P0-Conv_layer_1.input.txt"

parameters_base_path="PrunedResnetData/all_parameters"
answer_for_10040_channel="${parameters_base_path}/weights/P0-conv_layer_1_weight.txt"
answer_for_10041_channel="${parameters_base_path}/weights/P0-conv_layer_2_weight.txt"
answer_for_10042_channel="${parameters_base_path}/weights/P0-conv_layer_3_weight.txt"
answer_for_10043_channel="${parameters_base_path}/weights/P0-conv_layer_4_weight.txt"
answer_for_10044_channel="${parameters_base_path}/weights/P0-conv_layer_5_weight.txt"
answer_for_10045_channel="${parameters_base_path}/weights/P0-conv_layer_6_weight.txt"
answer_for_10046_channel="${parameters_base_path}/weights/P0-linear_layer_1_weight.txt"
answer_for_10047_channel="${parameters_base_path}/weights/P0-linear_layer_2_weight.txt"

answer_for_10020_channel="${parameters_base_path}/bias/P0-conv_layer_1_bias.txt"
answer_for_10021_channel="${parameters_base_path}/bias/P0-conv_layer_2_bias.txt"
answer_for_10022_channel="${parameters_base_path}/bias/P0-conv_layer_3_bias.txt"
answer_for_10023_channel="${parameters_base_path}/bias/P0-conv_layer_4_bias.txt"
answer_for_10024_channel="${parameters_base_path}/bias/P0-conv_layer_5_bias.txt"
answer_for_10025_channel="${parameters_base_path}/bias/P0-conv_layer_6_bias.txt"
answer_for_10026_channel="${parameters_base_path}/bias/P0-linear_layer_1_bias.txt"
answer_for_10027_channel="${parameters_base_path}/bias/P0-linear_layer_2_bias.txt"

answer_for_10034_channel="${parameters_base_path}/new_gamma/P0-bn_layer_1_new_gamma.txt"
answer_for_10035_channel="${parameters_base_path}/new_gamma/P0-bn_layer_2_new_gamma.txt"
answer_for_10036_channel="${parameters_base_path}/new_gamma/P0-bn_layer_3_new_gamma.txt"
answer_for_10037_channel="${parameters_base_path}/new_gamma/P0-bn_layer_4_new_gamma.txt"
answer_for_10038_channel="${parameters_base_path}/new_gamma/P0-bn_layer_5_new_gamma.txt"
answer_for_10039_channel="${parameters_base_path}/new_gamma/P0-bn_layer_6_new_gamma.txt"

answer_for_10028_channel="${parameters_base_path}/new_beta/P0-bn_layer_1_new_beta.txt"
answer_for_10029_channel="${parameters_base_path}/new_beta/P0-bn_layer_2_new_beta.txt"
answer_for_10030_channel="${parameters_base_path}/new_beta/P0-bn_layer_3_new_beta.txt"
answer_for_10031_channel="${parameters_base_path}/new_beta/P0-bn_layer_4_new_beta.txt"
answer_for_10032_channel="${parameters_base_path}/new_beta/P0-bn_layer_5_new_beta.txt"
answer_for_10033_channel="${parameters_base_path}/new_beta/P0-bn_layer_6_new_beta.txt"

# Put in this array the filepaths in the correct question order
filepath_answers=(
  "$answer_for_90050_channel"
  "$answer_for_10040_channel"
  "$answer_for_10041_channel"
  "$answer_for_10042_channel"
  "$answer_for_10043_channel"
  "$answer_for_10044_channel"
  "$answer_for_10045_channel"
  "$answer_for_10046_channel"
  "$answer_for_10047_channel"
  "$answer_for_10020_channel"
  "$answer_for_10021_channel"
  "$answer_for_10022_channel"
  "$answer_for_10023_channel"
  "$answer_for_10024_channel"
  "$answer_for_10025_channel"
  "$answer_for_10026_channel"
  "$answer_for_10027_channel"
  "$answer_for_10034_channel"
  "$answer_for_10035_channel"
  "$answer_for_10036_channel"
  "$answer_for_10037_channel"
  "$answer_for_10038_channel"
  "$answer_for_10039_channel"
  "$answer_for_10028_channel"
  "$answer_for_10029_channel"
  "$answer_for_10030_channel"
  "$answer_for_10031_channel"
  "$answer_for_10032_channel"
  "$answer_for_10033_channel"
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