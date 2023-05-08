#!/bin/bash

join_array() {
  local IFS="$1"
  shift
  echo "$*"
}

get_answers_for() {
  common_answers=$1
  player=$2

  IFS=' ' read -r -a answers_list <<< "$common_answers"
  for index in "${!answers_list[@]}"
  do
    if [[ ${answers_list[index]} == *".txt"* ]]; then
      if  [[ ${answers_list[index]%/*.*} == "${answers_list[index]}" ]] # check answer is not in subfolder
      then
        answers_list[index]="P$player-${answers_list[index]##*/}"
      else
        answers_list[index]="${answers_list[index]%/*.*}/P$player-${answers_list[index]##*/}"
      fi
    fi
  done
  player_answers=$(join_array ' ' "${answers_list[@]}")
  echo "$player_answers"
}

killall player || echo "no previous processes running"
killall PlayerBinary.x || echo "no previous processes running"
sleep 0.5

declare -i players=$(sed -n 2p Data/NetworkData.txt)

common_answers=$1
params="-f 1 $2"
rem=$(("$players" - 2))

for player_number in $(seq 0 $rem); do
  player_answers=$(get_answers_for "$common_answers" "$player_number")
  echo "trying with player $player_number"
  >&2 echo Running echo "$player_answers" \| ../Player.x "$player_number" $params
  echo "$player_answers" | ./Player.x "$player_number" $params 2>&1  &
done

last_player=$(("$players" - 1))
last_player_answers=$(get_answers_for "$common_answers" "$last_player")
echo "$last_player_answers"
>&2 echo Running ../Player.x "$last_player" $params
echo "$last_player_answers" | ./Player.x "$last_player" $params || return 1
